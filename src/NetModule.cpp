#include "NetModule.h"

extern "C"
{
#include <libavcodec/avcodec.h>
};

#include <map>
#include <set>

#include "Status.h"
#include "PacketQueue.h"

#pragma comment(lib, "ws2_32.lib")

NetModule::NetModule(const char * fileName, Status* statusPtr, PacketQueue* packetQueuePtr)
	: _fileName(fileName), _sockfd(0), _statusPtr(statusPtr), _packetQueuePtr(packetQueuePtr)
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	memset(&_serverSockAddr, 0, sizeof(sockaddr_in));
	_serverSockAddr.sin_family = AF_INET;
	_serverSockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_serverSockAddr.sin_port = htons(6666);

	_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

NetModule::~NetModule()
{
	closesocket(_sockfd);
	WSACleanup();
}

void NetModule::init(ReadBuffer& readBuffer)
{

	int write_size = sendto(_sockfd, _fileName, strlen(_fileName), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));

	
	int read_size = recvfrom(_sockfd, readBuffer.buf(), readBuffer.sizeReadable(), 0, NULL, NULL);

	
}

void NetModule::loop()
{
	while (1)
	{
		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_WAIT)			// ����������ݻ����Ͱ�
		{
			av_log(nullptr, AV_LOG_INFO, "send wait message.\n");
			sendBreakMessageAndSetStatus();
			_statusPtr->setStatus(Status::PLAYER_STATUS_WAIT_SENT);
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_WAIT_SENT)		// ������������ݻ�����
		{
			const int sleep_time = 1000;	// FIXME: ˯��ʱ�䣬����Ϊ��λ������Ҫ��
			Sleep(sleep_time);
			continue;
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_REQUESET_SEND)
		{
			av_log(nullptr, AV_LOG_INFO, "send restart message.\n");
			sendRestartRequest();
			_statusPtr->setStatus(Status::PLAYER_STATUS_LOOP);
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_LOOP)		// ����AVPacket���ݣ��յ�����AVPacket���ŵ�queue��
		{
			/*
			while (!receiveAVPacket())		// ʧ��������������ط�
			{
				reSendPacketRequest();
			}*/

			receiveAVPacket();

			
		}
	}
}

void NetModule::sendBreakMessageAndSetStatus()
{
	char msg = 'w';

	int write_size = sendto(_sockfd, &msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
	
}

void NetModule::reSendPacketRequest(uint16_t fragNo)
{
	char msg[3] = { 0, 0, 0 };
	msg[0] = 'r';
	char* ptr = msg;
	ptr++;
	uint16_t *addr = reinterpret_cast<uint16_t*>(ptr);
	*addr = fragNo;

	int write_size = sendto(_sockfd, msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
}

void NetModule::sendACK()
{
	char msg = 'a';

	int write_size = sendto(_sockfd, &msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
}

void NetModule::sendRestartRequest()
{
	char msg = 's';

	int write_size = sendto(_sockfd, &msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
}

bool NetModule::receiveAVPacket()
{
	ReadBuffer readBuffer;
	// �յ�1��������ƺ��������ó�ʱ��1�������ղ�����˵��������100%
	int read_size = recvfrom(_sockfd, readBuffer.buf(), readBuffer.sizeReadable(), 0, NULL, NULL);		// FIXME: ���Ƿ��������ó�ʱ��select

	int32_t packetSequenceNumber = readBuffer.readUInt32();	// ����sequence number
	uint16_t totalFragCnt = readBuffer.readUInt16();			// ��Ƭ����
	uint16_t fragNum = readBuffer.readUInt16();				// �յ��ķ�Ƭ��

	if (packetSequenceNumber <= _maxFinishedPacketNumber)
		return true;

	// ִ�е��⣬˵���յ�����1��packet��1����Ƭ�����������packet����������ǰ����������������ڲ����޷��˳��������

	std::set<uint16_t> lostFragNumSet;		// ���set��δ�յ��İ��ķ�Ƭ�ţ����set��size����map��size������totalFragCnt
	for (auto x = 0; x < totalFragCnt; ++x)
	{
		if (x != fragNum)
			lostFragNumSet.insert(x);
	}

	// ��Ƭ�������while ѭ������ʣ���packet�����������û�ﵽ�ܷ�Ƭ�����������������ط����������ack
	std::map<uint16_t, ReadBuffer> bufferMap;		// key: fragNum ����Ƭ��ţ�
	bufferMap[fragNum] = readBuffer;

	while (bufferMap.size() != totalFragCnt)
	{
		// �ȳ��Խ�
		auto lostPacketSize = lostFragNumSet.size();
		for (auto i = 0; i < lostPacketSize; ++i)
		{
			timeval tv;
			tv.tv_sec = 0, tv.tv_usec = 100;		// FIXME: ��ʱֵ����Ҫ��
			fd_set readFDSet;
			FD_ZERO(&readFDSet);
			FD_SET(_sockfd, &readFDSet);
			int ret = select(_sockfd + 1, &readFDSet, nullptr, nullptr, &tv);

			if (ret < 0)
			{
				av_log(nullptr, AV_LOG_ERROR, "select error.\n");
			}

			if (FD_ISSET(_sockfd, &readFDSet))		// �յ�1����Ƭ	
			{
				ReadBuffer readBuffer_1;
				int read_size = recvfrom(_sockfd, readBuffer_1.buf(), readBuffer_1.sizeReadable(), 0, NULL, NULL);

				auto packetNo = readBuffer_1.readUInt32();
				auto fragCnt = readBuffer_1.readUInt16();
				auto fragNo = readBuffer_1.readUInt16();

				bufferMap[fragNo] = readBuffer_1;		// ����map
				lostFragNumSet.erase(fragNo);			// ��set��ȥ��
			}
			else		// ���˳�ʱʱ�䲻����
			{
				
			}
		}

		// ������������Ҫ�ش��ķֽڵ�1������
		for (auto it = lostFragNumSet.cbegin(); it != lostFragNumSet.cend(); ++it)
		{
			auto fragNo = *it;
			reSendPacketRequest(fragNo);
		}
	}


	consturctPacketAndPutOnQueue(bufferMap);

	_maxFinishedPacketNumber = packetSequenceNumber;

	sendACK();
	return true;
}

void NetModule::consturctPacketAndPutOnQueue(std::map<uint16_t, ReadBuffer>& bufferMap)
{
	// ��Ϊ��1�����Ѿ������ˣ����������¶�ָ��
	bufferMap[0].resetReadPos();

	int32_t packetSequenceNumber = bufferMap[0].readUInt32();	// ����sequence number
	uint16_t totalFragCnt = bufferMap[0].readUInt16();			// ��Ƭ����
	uint16_t fragNum = bufferMap[0].readUInt16();

	// ��data�ֶεĲ������ڵ�һ��buffer
	AVPacket* packet = av_packet_alloc();
	auto stream_index = bufferMap[0].readUInt8();
	auto pos = bufferMap[0].readInt64();
	auto pts = bufferMap[0].readInt64();
	auto dts = bufferMap[0].readInt64();
	auto duration = bufferMap[0].readInt64();
	auto size = bufferMap[0].readInt32();

	const size_t maxSize = ReadBuffer::sizeLength();	// ���UDP����
	const size_t headerSize = 8;						// �̶�ͷ�����ȣ�8�ֽ�
	const size_t packetParaSize = 37;					// packet����data���ֵĳ���
	const size_t remainingSizeFor1stBuf = maxSize - headerSize - packetParaSize;	// ��1����Ƭʣ��������data�ĳ���
	const size_t remainingSizeForOtherBuf = maxSize - headerSize;		// ���˵�1����Ƭ��������data�ĳ���

	bool needFrag = bufferMap.size() > 1;
	// ����data, av_new_packet��ѳ���size��data���ֶγ�ʼ����
	av_new_packet(packet, size);
	packet->stream_index = stream_index;
	packet->pos = pos;
	packet->pts = pts;
	packet->dts = dts;
	packet->duration = duration;

	uint8_t *ptr = packet->data;
	size_t bytesNeedToWrite = packet->size;
	if (!needFrag)
	{
		bufferMap[0].readChunk(ptr, packet->size);
		bytesNeedToWrite -= packet->size;
	}
	else
	{
		bufferMap[0].readChunk(ptr, remainingSizeFor1stBuf);
		ptr += remainingSizeFor1stBuf;
		bytesNeedToWrite -= remainingSizeFor1stBuf;
	}

	uint16_t i = 1;		// ���ڼ���buffer����Ƭ�� 
	while (bytesNeedToWrite > 0)
	{
		size_t bytesWrittenToThisFrag = bytesNeedToWrite > remainingSizeForOtherBuf ? remainingSizeForOtherBuf : bytesNeedToWrite;	// �÷�ƬҪд����ֽ���

		bufferMap[i].resetReadPos();
		auto x = bufferMap[i].readUInt32();
		auto y = bufferMap[i].readUInt16();
		auto z = bufferMap[i].readUInt16();


		bufferMap[i].readChunk(ptr, bytesWrittenToThisFrag);

		ptr += bytesWrittenToThisFrag;

		i++;
		bytesNeedToWrite -= bytesWrittenToThisFrag;
	}

	av_log(nullptr, AV_LOG_INFO, "\n~~~~~~~~~~~~~~~~~~~~~\n");
	av_log(nullptr, AV_LOG_INFO, "packetSequenceNumber: %d\n", packetSequenceNumber);
	av_log(nullptr, AV_LOG_INFO, "_maxFinishedPacketNumber: %d\n", _maxFinishedPacketNumber);
	av_log(nullptr, AV_LOG_INFO, "packet size: %d\n", packet->size);
	av_log(nullptr, AV_LOG_INFO, "~~~~~~~~~~~~~~~~~~~~~\n");

	// ����queue
	_packetQueuePtr->put(packet);
}

#include "NetModule.h"

extern "C"
{
#include <libavcodec/avcodec.h>
};

#include <map>


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

			while (!receiveAVPacket())		// ʧ��������������ط�
			{
				reSendPacketRequest();
			}

			
		}
	}
}

void NetModule::sendBreakMessageAndSetStatus()
{
	char msg = 'w';

	int write_size = sendto(_sockfd, &msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
	
}

void NetModule::reSendPacketRequest()
{
	char msg = 'r';

	int write_size = sendto(_sockfd, &msg, sizeof(msg), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));
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
	// �յ�1��������ƺ��������ó�ʱ����Ϊ������һ��ʱ��û�յ��ͻ��˵�ack���ط����ͻ��������յ�
	int read_size = recvfrom(_sockfd, readBuffer.buf(), readBuffer.sizeReadable(), 0, NULL, NULL);		// FIXME: ���Ƿ��������ó�ʱ��select

	int32_t packetSequenceNumber = readBuffer.readUInt32();	// ����sequence number
	uint16_t totalFragCnt = readBuffer.readUInt16();			// ��Ƭ����
	uint16_t fragNum = readBuffer.readUInt16();

	if (packetSequenceNumber <= _maxFinishedPacketNumber)
		return true;

	/*
	if (totalFragCnt == 1)	// ֻ��һ����Ƭ
	{
		AVPacket* packet = av_packet_alloc();
		packet->stream_index = readBuffer.readUInt8();
		packet->pos = readBuffer.readInt64();
		packet->pts = readBuffer.readInt64();
		packet->dts = readBuffer.readInt64();
		packet->duration = readBuffer.readInt64();
		packet->size = readBuffer.readInt32();

		// TODO: �ŵ�queue��

		av_log(nullptr, AV_LOG_INFO, "\n~~~~~~~~~~~~~~~~~~~~~\n");
		av_log(nullptr, AV_LOG_INFO, "packetSequenceNumber: %d\n", packetSequenceNumber);
		av_log(nullptr, AV_LOG_INFO, "_maxFinishedPacketNumber: %d\n", _maxFinishedPacketNumber);
		av_log(nullptr, AV_LOG_INFO, "packet size: %d\n", packet->size);
		av_log(nullptr, AV_LOG_INFO, "~~~~~~~~~~~~~~~~~~~~~\n");
		_maxFinishedPacketNumber = packetSequenceNumber;

		sendACK();
		return true;
	}
	*/

	// ��Ƭ�������while ѭ������ʣ���packet�����������û�ﵽ�ܷ�Ƭ�����������������ط����������ack
	std::map<uint16_t, ReadBuffer> bufferMap;		// key: fragNum ����Ƭ��ţ�
	bufferMap[fragNum] = readBuffer;

	timeval tv;
	tv.tv_sec = 0, tv.tv_usec = 100;		// FIXME: ��ʱֵ����Ҫ��
	fd_set readFDSet;
	FD_ZERO(&readFDSet);
	for (auto i = 1; i < totalFragCnt; ++i)
	{
		FD_SET(_sockfd, &readFDSet);
		int ret = select(_sockfd + 1, &readFDSet, nullptr, nullptr, &tv);

		if (ret < 0)
		{
			av_log(nullptr, AV_LOG_ERROR, "select error.\n");
		}

		if (FD_ISSET(_sockfd, &readFDSet))
		{
			ReadBuffer readBuffer_1;
			int read_size = recvfrom(_sockfd, readBuffer_1.buf(), readBuffer_1.sizeReadable(), 0, NULL, NULL);

			auto packetNo = readBuffer_1.readUInt32();
			auto fragCnt = readBuffer_1.readUInt16();
			auto fragNo = readBuffer_1.readUInt16();

			bufferMap[fragNo] = readBuffer_1;
		}
		else		// ���˳�ʱʱ��sockfdҲû�ж��¼�������˵����������
		{
			return false;
		}
	}

	if (bufferMap.size() < totalFragCnt)
	{
		av_log(nullptr, AV_LOG_INFO, "\n=====================\n");
		av_log(nullptr, AV_LOG_INFO, "���ַ�Ƭ��ʧ\n");
		av_log(nullptr, AV_LOG_INFO, "=====================\n");

		return false;
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

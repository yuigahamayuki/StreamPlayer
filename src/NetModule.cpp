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
		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_WAIT)			// 请求服务器暂缓发送包
		{
			av_log(nullptr, AV_LOG_INFO, "send wait message.\n");
			sendBreakMessageAndSetStatus();
			_statusPtr->setStatus(Status::PLAYER_STATUS_WAIT_SENT);
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_WAIT_SENT)		// 已请求服务器暂缓发送
		{
			const int sleep_time = 1000;	// FIXME: 睡眠时间，毫秒为单位，可能要改
			Sleep(sleep_time);
			continue;
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_REQUESET_SEND)
		{
			av_log(nullptr, AV_LOG_INFO, "send restart message.\n");
			sendRestartRequest();
			_statusPtr->setStatus(Status::PLAYER_STATUS_LOOP);
		}

		if (_statusPtr->getStatus() == Status::PLAYER_STATUS_LOOP)		// 接收AVPacket数据，收到后构造AVPacket，放到queue上
		{
			/*
			while (!receiveAVPacket())		// 失败则请求服务器重发
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
	// 收第1个，这个似乎不用设置超时，1个包都收不到，说明丢包率100%
	int read_size = recvfrom(_sockfd, readBuffer.buf(), readBuffer.sizeReadable(), 0, NULL, NULL);		// FIXME: 考虑非阻塞设置超时，select

	int32_t packetSequenceNumber = readBuffer.readUInt32();	// 包的sequence number
	uint16_t totalFragCnt = readBuffer.readUInt16();			// 分片数量
	uint16_t fragNum = readBuffer.readUInt16();				// 收到的分片号

	if (packetSequenceNumber <= _maxFinishedPacketNumber)
		return true;

	// 执行到这，说明收到了下1个packet的1个分片，在收齐这个packet的所有数据前，都会在这个函数内部，无法退出这个函数

	std::set<uint16_t> lostFragNumSet;		// 这个set存未收到的包的分片号，这个set的size加上map的size正好是totalFragCnt
	for (auto x = 0; x < totalFragCnt; ++x)
	{
		if (x != fragNum)
			lostFragNumSet.insert(x);
	}

	// 分片的情况，while 循环接收剩余的packet，如果最后个数没达到总分片数，给服务器发送重发命令，否则发送ack
	std::map<uint16_t, ReadBuffer> bufferMap;		// key: fragNum （分片序号）
	bufferMap[fragNum] = readBuffer;

	while (bufferMap.size() != totalFragCnt)
	{
		// 先尝试接
		auto lostPacketSize = lostFragNumSet.size();
		for (auto i = 0; i < lostPacketSize; ++i)
		{
			timeval tv;
			tv.tv_sec = 0, tv.tv_usec = 100;		// FIXME: 超时值可能要改
			fd_set readFDSet;
			FD_ZERO(&readFDSet);
			FD_SET(_sockfd, &readFDSet);
			int ret = select(_sockfd + 1, &readFDSet, nullptr, nullptr, &tv);

			if (ret < 0)
			{
				av_log(nullptr, AV_LOG_ERROR, "select error.\n");
			}

			if (FD_ISSET(_sockfd, &readFDSet))		// 收到1个分片	
			{
				ReadBuffer readBuffer_1;
				int read_size = recvfrom(_sockfd, readBuffer_1.buf(), readBuffer_1.sizeReadable(), 0, NULL, NULL);

				auto packetNo = readBuffer_1.readUInt32();
				auto fragCnt = readBuffer_1.readUInt16();
				auto fragNo = readBuffer_1.readUInt16();

				bufferMap[fragNo] = readBuffer_1;		// 放入map
				lostFragNumSet.erase(fragNo);			// 从set中去除
			}
			else		// 过了超时时间不处理
			{
				
			}
		}

		// 给服务器发需要重传的分节的1个个包
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
	// 因为第1个包已经读过了，所以重置下读指针
	bufferMap[0].resetReadPos();

	int32_t packetSequenceNumber = bufferMap[0].readUInt32();	// 包的sequence number
	uint16_t totalFragCnt = bufferMap[0].readUInt16();			// 分片数量
	uint16_t fragNum = bufferMap[0].readUInt16();

	// 非data字段的参数都在第一个buffer
	AVPacket* packet = av_packet_alloc();
	auto stream_index = bufferMap[0].readUInt8();
	auto pos = bufferMap[0].readInt64();
	auto pts = bufferMap[0].readInt64();
	auto dts = bufferMap[0].readInt64();
	auto duration = bufferMap[0].readInt64();
	auto size = bufferMap[0].readInt32();

	const size_t maxSize = ReadBuffer::sizeLength();	// 最大UDP长度
	const size_t headerSize = 8;						// 固定头部长度，8字节
	const size_t packetParaSize = 37;					// packet除了data部分的长度
	const size_t remainingSizeFor1stBuf = maxSize - headerSize - packetParaSize;	// 第1个分片剩余用来放data的长度
	const size_t remainingSizeForOtherBuf = maxSize - headerSize;		// 除了第1个分片可用来放data的长度

	bool needFrag = bufferMap.size() > 1;
	// 复制data, av_new_packet会把除了size和data的字段初始化掉
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

	uint16_t i = 1;		// 读第几个buffer（分片） 
	while (bytesNeedToWrite > 0)
	{
		size_t bytesWrittenToThisFrag = bytesNeedToWrite > remainingSizeForOtherBuf ? remainingSizeForOtherBuf : bytesNeedToWrite;	// 该分片要写入的字节数

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

	// 放入queue
	_packetQueuePtr->put(packet);
}

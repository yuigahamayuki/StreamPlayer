/*
	这个类主要接收来自服务器发来的包，包括初始化解码器的参数，以及AVPacket的数据
	有接收缓冲
	接到packet后，放到queue上，供解码器解码
	若queue上有太多未处理的packet，发消息给服务器，放慢发送数据的速度

	成员变量：server sockaddr; 请求文件路径; queue
*/ 

#pragma once

#include <WinSock2.h>

#include <map>

#include "ReadBuffer.h"


class Status;
class PacketQueue;

class NetModule
{
public:
	NetModule(const char* fileName, Status* statusPtr, PacketQueue* packetQueuePtr);
	~NetModule();
	// 发送：请求某个视频文件		接收：对应于请求的视频文件的解码器的参数
	void init(ReadBuffer& readBuffer);	
	// 根据状态，发送暂缓消息，还是持续接收AVPacket
	void loop();	
private:
	// 发送给服务器，让服务器暂缓发送包；然后更新状态   命令'w'
	void sendBreakMessageAndSetStatus();	
	// 让服务器重发AVPacket， 命令'r'
	void reSendPacketRequest();
	// 报告服务器成功发送packet  命令'a'
	void sendACK();
	// 请求某个特定视频文件
	//void sendMediaFileRequest();	
	// 接收AVPacket数据，构造AVPacket，并放到queue上
	bool receiveAVPacket();	
	// 接收解码器的参数
	//void receiveDecoderParameters();	
	// 把buffer接收到的packet数据构造出AVPacket，并放到queue上
	void consturctPacketAndPutOnQueue(std::map<uint16_t, ReadBuffer>& bufferMap);

	const char* _fileName;
	sockaddr_in _serverSockAddr;
	SOCKET _sockfd;
	Status* _statusPtr;
	// 已处理好的packet sequence number的最大值，用于客户端发送的ack丢掉，服务器重发了客户端已经处理过的packet，发来的packet的sequence number如果小于等于该值，直接跳过，客户端再发个ack
	int _maxFinishedPacketNumber = -1;
	PacketQueue* _packetQueuePtr;
};
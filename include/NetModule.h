/*
	这个类主要接收来自服务器发来的包，包括初始化解码器的参数，以及AVPacket的数据
	有接收缓冲
	接到packet后，放到queue上，供解码器解码
	若queue上有太多未处理的packet，发消息给服务器，放慢发送数据的速度

	成员变量：server sockaddr; 请求文件路径; queue
*/ 

#pragma once

#include <WinSock2.h>

#include "ReadBuffer.h"

class NetModule
{
public:
	NetModule(const char* fileName);
	~NetModule();
	void init(ReadBuffer& readBuffer);	// 发送：请求某个视频文件		接收：对应于请求的视频文件的解码器的参数
	void loop();	// 根据状态，发送暂缓消息，还是持续接收AVPacket
private:
	void sendBreakMessage();	// 发送给服务器，让服务器暂缓发送包
	void sendMediaFileRequest();	// 请求某个特定视频文件
	void receiveAVPacket();		// 接收AVPacket数据，构造AVPacket，并放到queue上
	void receiveDecoderParameters();	// 接收解码器的参数

	const char* _fileName;
	sockaddr_in _serverSockAddr;
	SOCKET _sockfd;
};
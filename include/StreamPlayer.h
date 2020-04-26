/*
	这个类的成员是播放器的各组件，包括网络模块，解码器，渲染器等

*/

#pragma once

#include "Status.h"
#include "VideoStatus.h"
#include "PacketQueue.h"
#include "FrameQueue.h"
#include "NetModule.h"
#include "Decoder.h"
#include "Renderer.h"


class StreamPlayer
{
public:
	StreamPlayer(const char* file_name);
	~StreamPlayer();
	void start();
private:
	// 网络状况（接收状态？暂停接收？etc）
	Status _playerStaus;
	// 视频播放状态（播放？暂停？）以及渲染分辨率等
	VideoStatus _videoStatus;
	PacketQueue _packetQueue;
	FrameQueue _frameQueue;
	NetModule _netModule;
	Decoder _decoder;
	Renderer _renderer;
};
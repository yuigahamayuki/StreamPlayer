/*
	这个类的成员是播放器的各组件，包括网络模块，解码器，渲染器等

*/

#pragma once

#include "NetModule.h"
#include "Decoder.h"
#include "Status.h"

class StreamPlayer
{
public:
	StreamPlayer(const char* file_name);
	~StreamPlayer();
	void start();
private:
	Status _playerStaus;
	NetModule _netModule;
	Decoder _decoder;
};
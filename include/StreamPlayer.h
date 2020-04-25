/*
	�����ĳ�Ա�ǲ������ĸ��������������ģ�飬����������Ⱦ����

*/

#pragma once

#include "NetModule.h"
#include "Decoder.h"
#include "Status.h"
#include "PacketQueue.h"

class StreamPlayer
{
public:
	StreamPlayer(const char* file_name);
	~StreamPlayer();
	void start();
private:
	Status _playerStaus;
	PacketQueue _packetQueue;
	NetModule _netModule;
	Decoder _decoder;

};
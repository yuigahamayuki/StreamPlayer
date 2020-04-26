/*
	�����ĳ�Ա�ǲ������ĸ��������������ģ�飬����������Ⱦ����

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
	// ����״��������״̬����ͣ���գ�etc��
	Status _playerStaus;
	// ��Ƶ����״̬�����ţ���ͣ�����Լ���Ⱦ�ֱ��ʵ�
	VideoStatus _videoStatus;
	PacketQueue _packetQueue;
	FrameQueue _frameQueue;
	NetModule _netModule;
	Decoder _decoder;
	Renderer _renderer;
};
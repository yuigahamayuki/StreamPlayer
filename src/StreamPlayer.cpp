#include "..\include\StreamPlayer.h"

#include <thread>

StreamPlayer::StreamPlayer(const char* file_name)
	: _netModule(file_name, &_playerStaus, &_packetQueue),
	_decoder(&_videoStatus, &_packetQueue, &_frameQueue),
	_renderer(&_playerStaus, &_videoStatus, &_frameQueue)
{
}

StreamPlayer::~StreamPlayer()
{
}

void StreamPlayer::start()
{
	{
		ReadBuffer readBuffer;
		_netModule.init(readBuffer);

		_decoder.initPara(readBuffer);

		_playerStaus.setStatus(Status::PLAYER_STATUS_LOOP);		// 设置状态为不停接收服务器发来的AVPacket数据
	}
	
	std::thread(&Decoder::loop, std::ref(_decoder)).detach();
	std::thread(&Renderer::loop, std::ref(_renderer)).detach();
	_netModule.loop();
}

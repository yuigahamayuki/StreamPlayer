#include "..\include\StreamPlayer.h"



StreamPlayer::StreamPlayer(const char* file_name)
	: _netModule(file_name, &_playerStaus),
	_decoder(&_playerStaus)
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
	

	_netModule.loop();
}

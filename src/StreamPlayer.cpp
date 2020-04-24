#include "..\include\StreamPlayer.h"



StreamPlayer::StreamPlayer(const char* file_name)
	: _netModule(file_name)
{
}

StreamPlayer::~StreamPlayer()
{
}

void StreamPlayer::start()
{
	ReadBuffer readBuffer;
	_netModule.init(readBuffer);

	_decoder.initPara(readBuffer);

}

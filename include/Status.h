/*
	这个类表示当前播放器的状态
*/

#pragma once

#include <mutex>

class Status
{
public:
	Status()	{}

	Status(const Status&) = delete;
	Status& operator=(const Status&) = delete;

	typedef enum	// 这里列出的状态是与网络模块有关的
	{
		PLAYER_STATUS_INIT = 0,		// 解码器参数未设置前处于此状态
		PLAYER_STATUS_LOOP,			// 不断请求服务器发送AVPacket
		PLAYER_STATUS_WAIT,			// 有太多Packet未处理，但未给服务器发暂缓处理请求
		PLAYER_STATUS_WAIT_SENT,	// 有太多Packet未处理，已给服务器发暂缓处理请求
		PLAYER_STATUS_REQUESET_SEND,	// 当处于PLAYER_STATUS_WAIT_SENT状态且渲染缓存的frame剩余的不多，重新请求服务器发送packet
	} eStatus;

	void setStatus(eStatus status) 
	{
		_mutex.lock();
		_status = status; 
		_mutex.unlock();
	}
	eStatus getStatus() 
	{ 
		_mutex.lock();
		auto ret = _status;
		_mutex.unlock();
		return ret;
	}
private:
	eStatus _status = PLAYER_STATUS_INIT;
	std::mutex _mutex;
};
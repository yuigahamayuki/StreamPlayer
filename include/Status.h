/*
	这个类表示当前播放器的状态
*/

#pragma once

#include <mutex>

class Status
{
public:
	typedef enum 
	{
		PLAYER_STATUS_INIT = 0,		// 解码器参数未设置前处于此状态
		PLAYER_STATUS_LOOP,			// 不断请求服务器发送AVPacket
		PLAYER_STATUS_WAIT,			// 有太多Packet未处理，但未给服务器发暂缓处理请求
		PLAYER_STATUS_WAIT_SENT,	// 有太多Packet未处理，已给服务器发暂缓处理请求
		PLAYER_STATUS_PAUSE,		// 用户暂停视频播放，即渲染器不update
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
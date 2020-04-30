/*
	这个类保存视频的一些状态，如渲染的分辨率，播放状态等
*/
#pragma once

#include <mutex>

class VideoStatus
{
public:
	VideoStatus();

	VideoStatus(const VideoStatus&) = delete;
	VideoStatus& operator=(const VideoStatus&) = delete;

	typedef enum	// 这里列出的状态是与视频播放有关的
	{
		PLAYER_STATUS_PLAY= 0,
		PLAYER_STATUS_PAUSE,		// 用户暂停视频播放，即渲染器不update
	} eVideoStatus;

	void setStatus(eVideoStatus status)
	{
		_mutex.lock();
		_videoStatus = status;
		_mutex.unlock();
	}
	eVideoStatus getStatus()
	{
		_mutex.lock();
		auto ret = _videoStatus;
		_mutex.unlock();
		return ret;
	}

	void setWidth(unsigned short width)
	{
		_mutex.lock();
		_width = width;
		_mutex.unlock();
	}
	unsigned short getWidth()
	{
		_mutex.lock();
		auto width = _width;
		_mutex.unlock();
		return width;
	}

	void setHeight(unsigned short height)
	{
		_mutex.lock();
		_height = height;
		_mutex.unlock();
	}
	unsigned short getHeight()
	{
		_mutex.lock();
		auto height = _height;
		_mutex.unlock();
		return height;
	}

private:
	// 视频状态（播放？暂停？）
	eVideoStatus _videoStatus = PLAYER_STATUS_PLAY;
	// 渲染宽度
	unsigned short _width = 0;
	// 渲染高度
	unsigned short _height = 0;

	std::mutex _mutex;
};

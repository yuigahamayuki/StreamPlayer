/*
	����ౣ����Ƶ��һЩ״̬������Ⱦ�ķֱ��ʣ�����״̬��
*/
#pragma once

#include <mutex>

class VideoStatus
{
public:
	VideoStatus();

	VideoStatus(const VideoStatus&) = delete;
	VideoStatus& operator=(const VideoStatus&) = delete;

	typedef enum	// �����г���״̬������Ƶ�����йص�
	{
		PLAYER_STATUS_PLAY= 0,
		PLAYER_STATUS_PAUSE,		// �û���ͣ��Ƶ���ţ�����Ⱦ����update
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
	// ��Ƶ״̬�����ţ���ͣ����
	eVideoStatus _videoStatus = PLAYER_STATUS_PLAY;
	// ��Ⱦ���
	unsigned short _width = 0;
	// ��Ⱦ�߶�
	unsigned short _height = 0;

	std::mutex _mutex;
};

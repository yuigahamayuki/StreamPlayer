/*
	������ʾ��ǰ��������״̬
*/

#pragma once

#include <mutex>

class Status
{
public:
	typedef enum 
	{
		PLAYER_STATUS_INIT = 0,		// ����������δ����ǰ���ڴ�״̬
		PLAYER_STATUS_LOOP,			// �����������������AVPacket
		PLAYER_STATUS_WAIT,			// ��̫��Packetδ��������δ�����������ݻ���������
		PLAYER_STATUS_WAIT_SENT,	// ��̫��Packetδ�������Ѹ����������ݻ���������
		PLAYER_STATUS_PAUSE,		// �û���ͣ��Ƶ���ţ�����Ⱦ����update
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
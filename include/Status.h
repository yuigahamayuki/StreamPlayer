/*
	������ʾ��ǰ��������״̬
*/

#pragma once

#include <mutex>

class Status
{
public:
	Status()	{}

	Status(const Status&) = delete;
	Status& operator=(const Status&) = delete;

	typedef enum	// �����г���״̬��������ģ���йص�
	{
		PLAYER_STATUS_INIT = 0,		// ����������δ����ǰ���ڴ�״̬
		PLAYER_STATUS_LOOP,			// �����������������AVPacket
		PLAYER_STATUS_WAIT,			// ��̫��Packetδ������δ�����������ݻ���������
		PLAYER_STATUS_WAIT_SENT,	// ��̫��Packetδ�����Ѹ����������ݻ���������
		PLAYER_STATUS_REQUESET_SEND,	// ������PLAYER_STATUS_WAIT_SENT״̬����Ⱦ�����frameʣ��Ĳ��࣬�����������������packet
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
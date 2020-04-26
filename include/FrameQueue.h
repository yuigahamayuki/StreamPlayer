/*
	Frame Queue
	�ɽ�������frame����queue
	����Ⱦ����queueȡ��frame
*/

#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>

struct AVFrame;

class FrameQueue
{
public:
	FrameQueue();
	~FrameQueue();

	FrameQueue(const FrameQueue&) = delete;
	FrameQueue& operator=(const FrameQueue&) = delete;

	void put(AVFrame* frame);
	AVFrame* take();

	// �������δ����Ҫ��������Ϊ��������Frame�ܶ࣬��Ҫ��������������ͣ����ʱ��ֻ������Decoder�߳���queue����Frame��ֻ�ᵼ�¸����frame
	bool needSendWait();

private:
	std::deque<AVFrame*> _queue;

	std::mutex _mutex;
	std::condition_variable _cond;
	bool _empty = true;
};

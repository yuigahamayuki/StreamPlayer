#include "..\include\FrameQueue.h"

FrameQueue::FrameQueue()
{
}

FrameQueue::~FrameQueue()
{
}

void FrameQueue::put(AVFrame * frame)
{
	std::unique_lock<std::mutex> lck(_mutex);
	_queue.push_back(frame);
	_empty = false;
	_cond.notify_all();
}

AVFrame * FrameQueue::take()
{
	AVFrame* frame = nullptr;

	std::unique_lock<std::mutex> lck(_mutex);
	while (_empty)
		_cond.wait(lck);

	frame = _queue.front();
	_queue.pop_front();
	if (_queue.empty())
		_empty = true;

	return frame;
}

bool FrameQueue::needSendWait()
{
	// 24fps, ����5�룬������120��frameʱ�������������ݻ�����
	size_t threadSholdSize = 120;		// FIXME: ���ֵ����Ҫ��

	return _queue.size() >= 120;
}

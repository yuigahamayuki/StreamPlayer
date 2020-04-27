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
	while (!_empty)
		_cond.wait(lck);

	_queue.push_back(frame);
	if (!_queue.empty())
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
	_cond.notify_all();

	return frame;
}

bool FrameQueue::needSendWait()
{
	// 24fps, 缓冲5秒，即超过120张frame时，给服务器发暂缓命令
	size_t threadSholdSize = 120;		// FIXME: 这个值可能要改

	return _queue.size() >= threadSholdSize;
}

bool FrameQueue::needSendMore()
{
	size_t threadSholdSize = 20;		// FIXME: 这个值可能要改
	return _queue.size() <= threadSholdSize;
}

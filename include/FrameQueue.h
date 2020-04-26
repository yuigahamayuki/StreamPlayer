/*
	Frame Queue
	由解码器把frame放入queue
	由渲染器从queue取出frame
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

	// 这个函数未必需要加锁，因为如果缓存的Frame很多，需要给服务器发送暂停命令时，只可能有Decoder线程往queue里塞Frame，只会导致更多的frame
	bool needSendWait();

private:
	std::deque<AVFrame*> _queue;

	std::mutex _mutex;
	std::condition_variable _cond;
	bool _empty = true;
};

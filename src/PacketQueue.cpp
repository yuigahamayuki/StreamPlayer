#include "..\include\PacketQueue.h"

PacketQueue::PacketQueue()
{
}

PacketQueue::~PacketQueue()
{
}

void PacketQueue::put(AVPacket * packet)
{
	//_mutex.lock();
	//_queue.push(packet);
	//_mutex.unlock();

	std::unique_lock<std::mutex> lck(_mutex);
	while (!_empty)
		_cond.wait(lck);

	_queue.push(packet);
	if (!_queue.empty())
		_empty = false;
}

AVPacket * PacketQueue::take()
{
	AVPacket* packet = nullptr;

	std::unique_lock<std::mutex> lck(_mutex);
	if (!_queue.empty())
	{
		packet = _queue.front();
		_queue.pop();
	}

	if (_queue.empty())
		_empty = true;

	_cond.notify_all();

	return packet;
}

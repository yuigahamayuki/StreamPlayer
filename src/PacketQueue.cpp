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
	_queue.push(packet);
	_empty = false;
	_cond.notify_all();
}

AVPacket * PacketQueue::take()
{
	AVPacket* packet = nullptr;

	std::unique_lock<std::mutex> lck(_mutex);
	while (_empty)
		_cond.wait(lck);

	packet = _queue.front();
	_queue.pop();
	return packet;
}

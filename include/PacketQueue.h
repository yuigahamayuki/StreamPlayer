/*
	packet queue
	由网络模块放入，
	由解码器取出，
*/

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

struct AVPacket;

class PacketQueue
{
public:
	PacketQueue();
	~PacketQueue();

	PacketQueue(const PacketQueue&) = delete;
	PacketQueue& operator=(const PacketQueue&) = delete;

	void put(AVPacket* packet);
	AVPacket* take();


private:
	std::queue<AVPacket*> _queue;
	std::mutex _mutex;
	std::condition_variable _cond;
	bool _empty = true;
};
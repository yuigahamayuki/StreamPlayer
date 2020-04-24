/*
	Read buffer
	不同操作系统可能存在字节序问题

*/

#pragma once

#include <stdint.h>

class ReadBuffer
{
public:
	ReadBuffer();

	size_t sizeAlreadyRead() { return read_pos - start_pos; }
	size_t sizeReadable() { return buffer_length - sizeAlreadyRead(); }

	char* buf() { return _buffer; }

	uint8_t readUInt8()
	{
		uint8_t val = 0;

		if (sizeReadable() >= sizeof(uint8_t))
		{
			val = static_cast<uint8_t>(*read_pos);
			read_pos++;
		}

		return val;
	}

	char readInt8()
	{
		char val = 0;

		if (sizeReadable() >= sizeof(char))
		{
			val = *read_pos;
			read_pos++;
		}

		return val;
	}

	uint16_t readUInt16()
	{
		uint16_t val = 0;

		if (sizeReadable() >= sizeof(uint16_t))
		{
			uint16_t *addr = reinterpret_cast<uint16_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(uint16_t);
		}

		return val;
	}

	int16_t readInt16()
	{
		int16_t val = 0;

		if (sizeReadable() >= sizeof(int16_t))
		{
			int16_t *addr = reinterpret_cast<int16_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(int16_t);
		}

		return val;
	}

	uint32_t readUInt32()
	{
		uint32_t val = 0;

		if (sizeReadable() >= sizeof(uint32_t))
		{
			uint32_t *addr = reinterpret_cast<uint32_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(uint32_t);
		}

		return val;
	}

	int32_t readInt32()
	{
		int32_t val = 0;

		if (sizeReadable() >= sizeof(int32_t))
		{
			int32_t *addr = reinterpret_cast<int32_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(int32_t);
		}

		return val;
	}

	uint64_t readUInt64()
	{
		uint64_t val = 0;

		if (sizeReadable() >= sizeof(uint64_t))
		{
			uint64_t *addr = reinterpret_cast<uint64_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(uint64_t);
		}

		return val;
	}


	int64_t readInt64()
	{
		int64_t val = 0;

		if (sizeReadable() >= sizeof(int64_t))
		{
			int64_t *addr = reinterpret_cast<int64_t*>(read_pos);
			val = *addr;
			read_pos += sizeof(int64_t);
		}

		return val;
	}

	void readChunk(void *dst, size_t size);		// 读连续内存到dst

private:
	static const size_t buffer_length = 1024;	// FIXME: 可能更改
	char _buffer[buffer_length];
	const char* start_pos;
	char* read_pos;
};
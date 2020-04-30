#include "ReadBuffer.h"

#include "string.h"

ReadBuffer::ReadBuffer()
	: start_pos(_buffer), read_pos(_buffer)
{
	memset(_buffer, 0, sizeof(_buffer));
}

ReadBuffer & ReadBuffer::operator=(const ReadBuffer & rhs)
{
	memcpy(_buffer, rhs.buf(), buffer_length);
	return *this;
}

void ReadBuffer::readChunk(void * dst, size_t size)
{
	if (sizeReadable() >= size)
	{
		memcpy(dst, read_pos, size);
		read_pos += size;
	}
}

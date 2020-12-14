#include "Buffer.h"
#include <assert.h>
#include <malloc.h>

namespace Lunia {

	StreamBuffer::StreamBuffer(int initialSize, bool isDynamicSize)
		: size(initialSize), autoGrowUp(isDynamicSize), readPos(0), writePos(0)
	{
		data = malloc(initialSize);
	}

	StreamBuffer::~StreamBuffer()
	{
		free(data);
	}

	unsigned int StreamBuffer::Read(unsigned char* buffer, unsigned int size)
	{
		int actualRead = 0;

		actualRead = size;
		if (this->size - readPos < (int)size)
			actualRead = this->size - readPos;
		if (actualRead <= 0) return 0;

		memcpy(buffer, (char*)data + readPos, actualRead);
		readPos += actualRead;
		return actualRead;
	}

	int StreamBuffer::SetReadCursor(int offset, IStream::CursorPosition base)
	{
		switch (base)
		{
		case IStream::CursorPosition::Begin:
			readPos = offset;
			break;
		case IStream::CursorPosition::Current:
			readPos += offset;
			break;
		case IStream::CursorPosition::End:
			readPos = size + offset;
			break;
		}

		if (readPos > size) readPos = size;
		else if (readPos < 0) readPos = 0;

		return readPos;
	}

	unsigned int StreamBuffer::Write(const unsigned char* buffer, unsigned size)
	{
		int actualWrite = 0;

		if (autoGrowUp)
		{
			while (this->size - writePos < (int)size)
				data = realloc(data, this->size <<= 1);
		}

		actualWrite = size;
		if (this->size - writePos < (int)size)
			actualWrite = this->size - writePos;

		if (actualWrite <= 0) return 0;

		memcpy((char*)data + writePos, buffer, actualWrite);
		writePos += actualWrite;
		return actualWrite;
	}

	int StreamBuffer::SetWriteCursor(int offset, IStream::CursorPosition base)
	{
		switch (base)
		{
		case IStream::CursorPosition::Begin:
			writePos = offset;
			break;
		case IStream::CursorPosition::Current:
			writePos += offset;
			break;
		case IStream::CursorPosition::End:
			writePos = size + offset;
			break;
		}

		if (writePos > size) writePos = size;
		else if (writePos < 0) writePos = 0;

		return writePos;
	}
}

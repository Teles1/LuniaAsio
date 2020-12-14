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


	//BinaryBuffer::BinaryBuffer(int initialSizeInByte, bool fixedSize, int maxBufferSize)
	//	:bufferSize(initialSizeInByte), maxBufferSize(maxBufferSize), data(0)
	//	,rCursor(0), wCursor(0)
	//{
	//	internalBufferSize=initialSizeInByte;
	//	if (fixedSize) this->maxBufferSize=initialSizeInByte;
	//	data=(char*)malloc(internalBufferSize);
	//}

 //   BinaryBuffer::~BinaryBuffer()
	//{
	//	if (data) free(data);
	//}

	//void BinaryBuffer::Truncate()
	//{
	//	internalBufferSize=bufferSize;
	//	data=(char*)realloc(data, internalBufferSize);
	//}

	//void BinaryBuffer::ReallocateBuffer(int size)
	//{
	//	assert(internalBufferSize<size);

	//	size>(internalBufferSize>>1)?
	//		internalBufferSize=size:internalBufferSize>>=1;

	//	if (internalBufferSize>maxBufferSize)
 //           internalBufferSize=maxBufferSize;

	//	data=(char*)realloc(data, internalBufferSize);
	//}

	//void BinaryBuffer::SetMaxSize(int size)
	//{
	//	maxBufferSize=size;
	//}

	//int BinaryBuffer::GetSize()
	//{
	//	return bufferSize;
	//}

	//void* BinaryBuffer::GetData(int pos)
	//{
	//	return data+pos;
	//}

	//int BinaryBuffer::Read(void* buf, int size, int pos)
	//{
	//	if (pos>bufferSize) return 0; // out of range

	//	int length=size;
	//	if (size+pos>bufferSize)
	//		length=bufferSize-pos;

	//	memcpy(buf, data+pos, length);
	//	return length;
	//}

	//int BinaryBuffer::Write(const void* bytes, int size, int pos)
	//{
	//	if (pos>bufferSize) // out of range
	//		return 0;

	//	int length=size;
	//	if (maxBufferSize<=bufferSize) // fixed buffer size
	//	{
	//		if (size+pos>bufferSize) // out of buffer size
	//			length=bufferSize-pos;
	//	} else // expendable buffer size
	//	{
	//		if (size+pos>bufferSize) // out of buffer
	//		{
	//			if (size+pos>maxBufferSize) // out of maximum buffer size
	//				length=maxBufferSize-pos;

 //               // need to grow buffer;
	//			if (length+pos>internalBufferSize) // need to grow the allocated buffer
	//				ReallocateBuffer(length+pos);

	//			bufferSize+=length;
	//		}
	//	}

	//	memcpy(data+pos, bytes, length);
	//	return length;
	//}

	//int BinaryBuffer::Append(const void* bytes, int size)
	//{
	//	return Write(bytes, size, bufferSize);
	//}

	//void SetCursor(int pos, IStreamBuffer::CursorPosition from, int& cursor, int endPos)
	//{
	//	switch(from)
	//	{
	//	case IStreamBuffer::CursorPosition::Current:
	//		cursor+=pos;
	//		break;
	//	case IStreamBuffer::CursorPosition::Begin:
	//		cursor=pos;
	//		break;
	//	case IStreamBuffer::CursorPosition::End:
	//		cursor=endPos+pos;
	//		break;
	//	}
	//	if (cursor>endPos) cursor=endPos;
	//	if (cursor<0) cursor=0;
	//}

	//void BinaryBuffer::SetReadCursor(int pos, IStreamBuffer::CursorPosition from)
	//{
	//	SetCursor(pos, from, rCursor, bufferSize);
	//}

	//void BinaryBuffer::SetWriteCursor(int pos, IStreamBuffer::CursorPosition from)
	//{
	//	SetCursor(pos, from, wCursor, bufferSize);
	//}

	//int BinaryBuffer::ReadStream(void* bytes, int size)
	//{
	//	int length=Read(bytes, size, rCursor);
	//	rCursor+=length;
	//	return length;
	//}

	//int BinaryBuffer::WriteStream(const void* bytes, int size)
	//{
	//	int length=Write(bytes, size, wCursor);
	//	wCursor+=length;
	//	return length;
	//}
}

#pragma once

#include "IStream.h"

namespace Lunia {
	class StreamBuffer
		: public IStreamWriter
		, public IStreamReader
	{
	private:
		void* data;
		bool autoGrowUp;
		int size;
		int readPos;
		int writePos;

	public:
		StreamBuffer(int initialSize = 1024, bool isDynamicSize = true);
		virtual ~StreamBuffer();

		inline const void* GetData() const { return data; }
		inline void* SetData(unsigned char* dataNew, int sizeNew) { data = dataNew; size = sizeNew; }

	public: /* IStream implements */
		inline virtual const wchar_t* GetName() const { return L"StreamBuffer"; }
		inline virtual unsigned int GetSize() const { return size; }


	public: /* IStreamReader implements */
		virtual unsigned int Read(unsigned char* buffer, unsigned int size);
		virtual int SetReadCursor(int offset, IStream::CursorPosition base);
		inline virtual int GetReadCursor() const { return readPos; }

	public: /* IStramWriter implements */
		virtual unsigned int Write(const unsigned char* buffer, unsigned size);
		virtual int SetWriteCursor(int position, IStream::CursorPosition base);
		inline virtual int GetWriteCursor() const { return writePos; }
	};
	// Deprecated /////////////////////////////////////////////////////////////////////////////////

	class IBuffer {
	public:
		virtual ~IBuffer() {}

		virtual void SetMaxSize(int size) = 0;
		virtual int GetSize() = 0;
		virtual void* GetData(int pos) = 0;
		virtual int Read(void* buf, int size, int pos) = 0; ///> returns the number of bytes read
		virtual int Write(const void* bytes, int size, int pos) = 0; ///> returns the number of bytes written
	};

	class IAdvancedBuffer : public IBuffer {
	public:
		virtual ~IAdvancedBuffer() {}

		virtual void Insert(const void* bytes, int size, int pos) = 0;
		virtual void Delete(int size, int pos) = 0;
	};

	class IStreamBuffer : public IBuffer {
	public:
		virtual ~IStreamBuffer() {}

		enum CursorPosition { Begin, Current, End };

		virtual int GetReadCursor() = 0;
		virtual int GetWriteCursor() = 0;
		virtual bool EndOfStream() = 0;
		virtual void SetReadCursor(int pos, CursorPosition from) = 0;
		virtual void SetWriteCursor(int pos, CursorPosition from) = 0;
		virtual int ReadStream(void* bytes, int size) = 0;
		virtual int WriteStream(const void* bytes, int size) = 0;
		virtual int Append(const void* bytes, int size) = 0; ///> append doesn't move write cursor
	};
}

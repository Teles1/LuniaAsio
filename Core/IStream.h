#pragma once 

#include <string>
#include "IReferenceCounted.h"

namespace Lunia {

	/**
	an abstract stream interface, which is used by the most data readers. Such as serializer.

	@author juhnu
	@date   1.1.2004
	*/

	class IStream {
	public:
		virtual ~IStream() {}

		enum CursorPosition {
			Begin,
			End,
			Current
		};

		/**
		name of the stream source, anonymous streams should return ""
		*/
		virtual const wchar_t* GetName() const = 0;

		/**
		@return the (current) size of the stream
		*/
		virtual unsigned int GetSize() const = 0;


		/**
		@return the size available in the stream from the current read/write cursor
		*/
		virtual unsigned int GetSizeLeft() const = 0;

	};

	class IStreamWriter : public IStream {
	public:
		/**
		writes data to the stream.
		moves writes cursor

		@param buffer destination buffer of the data
		@param size size in bytes to read
		@return actual bytes readed

		*/
		virtual unsigned int Write(const unsigned char* buffer, unsigned size) = 0;

		virtual unsigned int Write(const wchar_t* str) { return Write(reinterpret_cast<const unsigned char*>(str), unsigned int(wcslen(str)) * unsigned int(sizeof(wchar_t))); }
		virtual unsigned int Write(const std::wstring& str) { return Write(str.c_str()); }

		virtual unsigned int Write(const char* str) { return Write(reinterpret_cast<const unsigned char*>(str), unsigned int(strlen(str))); }
		virtual unsigned int Write(const std::string& str) { return Write(str.c_str()); }
		/**
		writes the read cursor number of bytes from a given position

		@params offset		offset from position
		@params origin		basis of offset
		@return				current offset of the file
		*/
		virtual int         SetWriteCursor(int position, CursorPosition base) = 0;

		/**
		@return the write cursor position in the stream
		*/
		virtual int          GetWriteCursor() const = 0;

		//template helper method for writing structs
		template <typename T> int WriteStruct(T* buffer, int count = 1) {
			return Write(reinterpret_cast<unsigned char*>(buffer), sizeof(T) * count);
		}
	};

	class IStreamReader : public IStream {
	public:

		/**
		Reads data from the stream. This method ca block if data is not available
		moves read cursor

		@param buffer destination buffer of the data
		@param size size in bytes to read
		@return actual bytes readed

		*/
		virtual unsigned int Read(unsigned char* buffer, unsigned int size) = 0;


		/**
		Moves the read cursor number of bytes from a given position

		@params offset		offset from position
		@params origin		basis of offset
		@return				current offset of the file
		*/
		virtual int SetReadCursor(int offset, CursorPosition base) = 0;

		/**
		@return the read cursor position in the stream
		*/
		virtual int GetReadCursor() const = 0;


		/* virtual void AppendData(unsigned char* buffer)=0; */



		//virtual int GetAvailable()=0; //returns data length that is available (read wont block)

		//template helper method for reading structs
		template <typename T> int ReadStruct(T* buffer, int count = 1) {
			return Read(reinterpret_cast<unsigned char*>(buffer), sizeof(T) * count);
		}

		template <typename T> T Read() {
			unsigned char buffer[sizeof(T)];
			Read(buffer, sizeof(T));
			return *reinterpret_cast<T*>(buffer);
		}

	};

	class IRefCountedStreamWriter : public IReferenceCounted, public IStreamWriter {};
	class IRefCountedStreamReader : public IReferenceCounted, public IStreamReader {};



}
#pragma once

#include "../ReferenceCounted.h"
#include "../IStream.h"

namespace Lunia {
	namespace FileIO {
		template <typename T> class TFileStreamReader : public T {

			File file;
			FileInfo info;
		public:

			virtual int inline SetReadCursor(int offset, IStream::CursorPosition seek) {
				File::SeekFrom sf = File::Top;
				if (seek == IStream::Current) sf = File::Cur;
				if (seek == IStream::End) sf = File::End;
				return file.Seek(offset, sf);
			}

			virtual int GetReadCursor() const {
				return const_cast<TFileStreamReader<T>*>(this)->file.Seek(0, File::Cur);
			}

			virtual unsigned int GetSize() const {
				return const_cast<TFileStreamReader<T>*>(this)->file.GetSize();
			}

			virtual unsigned int GetSizeLeft() const {
				return GetSize() - GetReadCursor();
			}

			virtual unsigned int Read(unsigned char* buffer, unsigned int size) {
				unsigned int readed = file.Read(buffer, size);
				if (readed != size) {
					throw Lunia::Exception(L"couldn't read requested number of bytes, requested={0}, readed={1}", size, readed);
				}
				return readed;
			}

			virtual const wchar_t* GetName() const {
				return info.Filename.c_str();
			}


			TFileStreamReader(const wchar_t* name) : file(name, File::ReadMode) {
				file.GetFileInfo(name, &info);
			}
		};

		template <typename T> class TCachedFileStreamReader : public T {

			std::vector<unsigned char> buffer;
			int cursor;
			std::wstring name;
		public:

			virtual int SetReadCursor(int offset, IStream::CursorPosition seek) {

				if (seek == IStream::Begin) cursor = offset;
				if (seek == IStream::Current) cursor += offset;
				if (seek == IStream::End) cursor = int(buffer.size()) + offset;

				return cursor;
			}

			virtual int GetReadCursor() const {
				return cursor;
			}

			virtual unsigned int GetSize() const {
				return int(buffer.size());
			}

			virtual unsigned int GetSizeLeft() const {
				return GetSize() - GetReadCursor();
			}


			virtual unsigned int Read(unsigned char* pBuffer, unsigned int size) {
				if (cursor + size >= (int)buffer.size()) {
					throw Lunia::Exception(L"trying to read past of the file +{0} bytes", buffer.size() - cursor + size);
				}
				for (unsigned int i = 0; i < size; ++i) {
					pBuffer[i] = buffer[cursor + i];
				}
				cursor += size;

				return size;
			}

			virtual const wchar_t* GetName() const {
				return name.c_str();
			}

			TCachedFileStreamReader(const wchar_t* pName) :name(pName), cursor(0) {
				File file(name, File::ReadMode);
				int size = file.GetSize();
				buffer.resize(size);
				file.Read(&buffer[0], size);
			}
		};

		template <typename T> class TFileStreamWriter : public T {

			File file;
			FileInfo info;
		public:

			virtual int SetWriteCursor(int offset, IStream::CursorPosition seek) {

				File::SeekFrom sf = File::Top;;
				if (seek == IStream::Current) sf = File::Cur;
				if (seek == IStream::End) sf = File::End;
				return file.Seek(offset, sf);
			}

			virtual int GetWriteCursor() const {
				return const_cast<TFileStreamWriter<T>*>(this)->file.Seek(0,File::Cur);
			}

			virtual unsigned int GetSize() const {
				return const_cast<TFileStreamWriter<T>*>(this)->file.GetSize();
			}

			virtual unsigned int GetSizeLeft() const {
				return GetSize() - GetWriteCursor();
			}

			virtual unsigned int Write(const unsigned char* buffer, unsigned int size) {
				file.Write(buffer, size);
				return size;
			}

			virtual const wchar_t* GetName() const {
				return info.Filename.c_str();
			}

			TFileStreamWriter(const wchar_t* name) : file(name,File::WriteMode) {
				file.GetFileInfo(name, &info);
			}
		};


		typedef TFileStreamReader<IStreamReader>										   FileStreamReader;
		typedef TFileStreamWriter<IStreamWriter>										   FileStreamWriter;
		typedef TFileStreamReader<ReferenceCountedImpl<IRefCountedStreamReader> >			RefCountedFileStreamReader;
		typedef TCachedFileStreamReader<ReferenceCountedImpl<IRefCountedStreamReader> >    RefCountedFileStreamReaderCache;
		typedef TFileStreamWriter<ReferenceCountedImpl<IRefCountedStreamWriter> >  RefCountedFileStreamWriter;


		template <typename T>
		class TMemoryStreamWriter : public T {

			uint8* stream;
			uint32 size;
			uint32 writePos;

		public:

			virtual int SetWriteCursor(int offset, IStream::CursorPosition seek) {
				if (seek == IStream::Begin) writePos = offset;
				if (seek == IStream::Current) writePos += offset;
				if (seek == IStream::End) writePos = this->size + offset;

				return writePos;
			}

			virtual int GetWriteCursor() const {
				return writePos;
			}

			virtual unsigned int GetSize() const {
				return this->size;
			}

			virtual unsigned int GetSizeLeft() const {
				return GetSize() - GetWriteCursor();
			}

			virtual unsigned int Write(const unsigned char* buffer, unsigned int size) {
				memcpy(stream, buffer, size);
				this->size += size;
				this->writePos += size;
				return size;
			}

			virtual const wchar_t* GetName() const {
				throw new Exception(L"GetName not supported for TMemoryStreamWriter");
			}

			TMemoryStreamWriter(uint8*& data, uint32& datasz)
				: stream(data), size(datasz), writePos(0)
			{
			}
		};

		template <typename T>
		class TMemoryStreamReader : public T {

			uint8* stream;

			uint32 size;
			uint32 readPos;

		public:

			virtual int SetReadCursor(int offset, IStream::CursorPosition seek) {
				if (seek == IStream::Begin) readPos = offset;
				if (seek == IStream::Current) readPos += offset;
				if (seek == IStream::End) readPos = this->size + offset;

				return readPos;
			}

			virtual int GetReadCursor() const {
				return readPos;
			}

			virtual unsigned int GetSize() const {
				return this->size;
			}

			virtual unsigned int GetSizeLeft() const {
				return GetSize() - GetReadCursor();
			}

			virtual unsigned int Read(unsigned char* buffer, unsigned int size) {
				int actualRead = 0;

				actualRead = size;
				if (this->size - readPos < (int)size)
					actualRead = this->size - readPos;
				if (actualRead <= 0) return 0;

				memcpy(buffer, (char*)stream + readPos, actualRead);
				readPos += actualRead;
				return actualRead;
			}

			virtual const wchar_t* GetName() const {
				throw new Exception(L"GetName not supported for MemoryStreamReader");
			}


			TMemoryStreamReader(uint8* data, uint32 datazs)
				: size(datazs), readPos(0), stream(data)
			{
				/* printf("TMemoryStreamReader initialized\n");
				for (int i = 0; i < datazs; i++){
					printf("%.2X ", data[i]);
				}
				printf("\n"); */
			}
		};

		typedef TMemoryStreamReader<IStreamReader>											MemoryStreamReader;
		typedef TMemoryStreamWriter<IStreamWriter>											MemoryStreamWriter;
		typedef TMemoryStreamReader<ReferenceCountedImpl<IRefCountedStreamReader> >   RefCountedMemoryStreamReader;
		typedef TMemoryStreamWriter<ReferenceCountedImpl<IRefCountedStreamWriter> >   RefCountedMemoryStreamWriter;
	}
}

#pragma once
#include "../ReferenceCounted.h"
#include "../IStream.h"

namespace Lunia {
	namespace FileIO {
		template <typename T> class TMemoryStreamReader : public T {

			char* stream;

			uint32 size;
			uint32 readPos;

		public:

			inline virtual int SetReadCursor(int offset, IStream::CursorPosition seek) {
				if (seek == IStream::Begin) readPos = offset;
				if (seek == IStream::Current) readPos += offset;
				if (seek == IStream::End) readPos = this->size + offset;

				return readPos;
			}

			inline virtual int GetReadCursor() const {
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
				return L"";
			}


			TMemoryStreamReader(char* data, uint32 bytes)
				: size(bytes), readPos(0)
			{
				stream = data;
			}
		};

		typedef TMemoryStreamReader<IStreamReader>											MemoryStreamReader;
		typedef TMemoryStreamReader<ReferenceCountedImpl<IRefCountedStreamReader> >			RefCountedMemoryStreamReader;
	}
}

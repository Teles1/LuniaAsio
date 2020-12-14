#pragma once
#include "../ReferenceCounted.h"
#include <Core/FileIO/FileBase.h>
namespace Lunia {
	namespace FileIO {

		/**
		this is a pure abstract interface

		NOTICE:

		it doesn't have static methods
		it doesn't have Open/Close methods, File class has

		*/

		/*
		enum SeekFrom { Top = FILE_BEGIN, End = FILE_END, Cur = FILE_CURRENT };
		enum Mode {
			ReadMode				=0x1,
			WriteMode				=0x2,
			AppendMode				=0x6
		};
		//*/

		class IFile : public IReferenceCounted {
		public:

			//*
			enum SeekFrom { Top = FILE_BEGIN, End = FILE_END, Cur = FILE_CURRENT };
			enum Mode {
				ReadMode = 0x1,
				WriteMode = 0x2,
				AppendMode = 0x6
			};
			//*/

			virtual long Seek(long offset, SeekFrom origin) = 0;
			virtual long GetSize() = 0;
			virtual int Read(unsigned char* buffer, long size) = 0;
			virtual bool Write(const unsigned char* buffer, long size) = 0;
			virtual bool Flush() = 0;
			virtual HANDLE GetHandle() = 0;
			virtual void GetFileInfo(FileInfo* fileInfo /* out */) = 0;
		};

	}
}

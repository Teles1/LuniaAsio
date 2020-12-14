#pragma once
#include <windows.h>
#include <list>
#include <vector>
#include "FileExceptions.h"
#include "../Types.h"

namespace Lunia {
	namespace FileIO {
		/**
		Common information of a file or a directory(folder)
		*/
		struct FileInfo
		{
			enum FileAttributes {
				Archive = FILE_ATTRIBUTE_ARCHIVE,
				Compressed = FILE_ATTRIBUTE_COMPRESSED,
				Folder = FILE_ATTRIBUTE_DIRECTORY,
				Encrypted = FILE_ATTRIBUTE_ENCRYPTED,
				Hidden = FILE_ATTRIBUTE_HIDDEN,
				Normal = FILE_ATTRIBUTE_NORMAL,
				Offline = FILE_ATTRIBUTE_OFFLINE,
				Readonly = FILE_ATTRIBUTE_READONLY,
				ReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT,
				SparseFile = FILE_ATTRIBUTE_SPARSE_FILE,
				System = FILE_ATTRIBUTE_SYSTEM,
				Directory = FILE_ATTRIBUTE_DIRECTORY,
				Temporary = FILE_ATTRIBUTE_TEMPORARY
			};

			String Filename;				///> File name
			long Size;						///> File size

			FILETIME Created;				///> Created date time
			FILETIME Modified;				///> Last modified date time

			FileAttributes Attributes;		///> File attributes

			String Description;				///> Description of the file @a (unused)
		};

		typedef std::vector<FileInfo*> FileInfoCollection;
	}
}

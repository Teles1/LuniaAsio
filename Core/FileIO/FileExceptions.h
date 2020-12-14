#pragma once

#include "../Types.h"
#include <Core/Utils/StringUtil.h>

namespace Lunia {
	namespace FileIO {

		class FileException : public Exception {
		private:
			String fileName;
			String what;

		public:
			inline FileException(const wchar_t* filename, const wchar_t* desc) :
				fileName(filename), Exception(L"{0} [{1}]", desc, filename)
			{
			}

			inline String& GetFileName() { return this->fileName; }
		};

		/******************************************************************************************/
		/** General file exceptions ***************************************************************/
		/******************************************************************************************/

		class FileNotFoundException : public FileException {
		public:
			inline FileNotFoundException(const wchar_t* filename) : FileException(filename, L"File not found") {}
		};

		class AccessDeniedException : public FileException {
		public:
			inline AccessDeniedException(const wchar_t* filename) : FileException(filename, L"Access denied") {}
		};

		class PathNotFoundException : public FileException {
		public:
			inline PathNotFoundException(const wchar_t* filename) : FileException(filename, L"Path not found") {}
		};

		class TooManyOpenFilesException : public FileException {
		public:
			inline TooManyOpenFilesException(const wchar_t* filename) : FileException(filename, L"Too many files are opened") {}
		};

		class FileExistsException : public FileException {
		public:
			inline FileExistsException(const wchar_t* filename) : FileException(filename, L"Already file exists") {}
		};

		class InvalidNameException : public FileException {
		public:
			inline InvalidNameException(const wchar_t* filename) : FileException(filename, L"Invalid file/directory name") {}
		};

		class BadUnitException : public FileException {
		public:
			inline BadUnitException(const wchar_t* filename) : FileException(filename, L"Bad unit") {}
		};

		class HandleEofException : public FileException {
		public:
			inline HandleEofException(const wchar_t* filename) : FileException(filename, L"End of file handle") {}
		};

		/******************************************************************************************/
		/** used in ArchiveFile *******************************************************************/
		/******************************************************************************************/

		class IndexNotFoundException : public FileException {
		public:
			inline IndexNotFoundException(const wchar_t* filename, const wchar_t* index) :
				indexName(index), FileException(filename, L"Index not found") {}
			inline String& GetIndexName() { return this->indexName; }
		private:
			String indexName;
		};

	}
}
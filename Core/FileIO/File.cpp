#include "FileHelper.h" // hided
#include <filesystem>
#include <iostream>

namespace Lunia {
	namespace FileIO {
		void File::Delete(const wchar_t* filename)
		{
			if (::DeleteFileW(filename))
				return;
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				if (::DeleteFileA(StringUtil::ToASCII(filename).c_str()))
					return;
			THROWFILEEXCEPTION(filename); // need to throw exception 
		}

		void File::DeleteDirectory(const wchar_t* fileName)
		{
			if (::RemoveDirectoryW(fileName))
				return;
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				if (::RemoveDirectoryA(StringUtil::ToASCII(fileName).c_str()))
					return;
			THROWFILEEXCEPTION(fileName); // need to throw exception 
		}

		void File::Copy(const wchar_t* existFilename, const wchar_t* newFilename, bool failExist)
		{
			if (::CopyFileW(existFilename, newFilename, failExist))
				return;
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				if (::CopyFileA(StringUtil::ToASCII(existFilename).c_str(), StringUtil::ToASCII(newFilename).c_str(), failExist))
					return;

			THROWFILEEXCEPTION(L"Copy {0} to {1} (exist {2})", existFilename, newFilename, (failExist ? L"True" : L"False")); // need to throw exception 
		}

		FileInfo::FileAttributes File::GetAttributes(const wchar_t* filename)
		{
			DWORD ret = ::GetFileAttributesW(filename);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				ret = ::GetFileAttributesA(StringUtil::ToASCII(filename).c_str());

			if (ret == INVALID_FILE_ATTRIBUTES)
			{
				THROWFILEEXCEPTION(filename); // need to throw exception 
			}
			return (FileInfo::FileAttributes)ret;
		}

		bool File::SetAttributes(const wchar_t* filename, FileInfo::FileAttributes attribute)
		{
			DWORD ret = ::SetFileAttributesW(filename, attribute);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				ret = ::SetFileAttributesA(StringUtil::ToASCII(filename).c_str(), attribute);
			if (ret == INVALID_FILE_ATTRIBUTES) return false;
			return true;
		}

		bool File::Exists(const wchar_t* filename)
		{
			DWORD ret = ::GetFileAttributesW(filename);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				ret = ::GetFileAttributesA(StringUtil::ToASCII(filename).c_str());

			if (ret == INVALID_FILE_ATTRIBUTES) return false;
			return true;
		}

		void File::GetFileInfo(const wchar_t* filename, FileInfo* fileInfo)
		{
			WIN32_FILE_ATTRIBUTE_DATA info;
			if (::GetFileAttributesExW(filename, GetFileExInfoStandard, &info))
			{
				//comming here instead
				fileInfo->Filename = filename;
				fileInfo->Size = (info.nFileSizeLow | (info.nFileSizeHigh << 32));
				fileInfo->Attributes = (FileInfo::FileAttributes)info.dwFileAttributes;
				fileInfo->Created = info.ftCreationTime;
				fileInfo->Modified = info.ftLastWriteTime;
				return;
			}
			else if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			{
				if (::GetFileAttributesExA(StringUtil::ToASCII(filename).c_str(), GetFileExInfoStandard, &info))
				{
					fileInfo->Filename = filename;
					fileInfo->Size = (info.nFileSizeLow | (info.nFileSizeHigh << 32));
					fileInfo->Attributes = (FileInfo::FileAttributes)info.dwFileAttributes;
					fileInfo->Created = info.ftCreationTime;
					fileInfo->Modified = info.ftLastWriteTime;
					return;
				}
			}

			THROWFILEEXCEPTION(filename);
		}

		File::File(const String& filename, Mode mode)
		{
			this->handle = NULL;
			Open(filename.c_str(), mode);
		}

		File::File(const wchar_t* filename, Mode mode)
		{
			this->handle = NULL;
			Open(filename, mode);
		}

		File::File()
		{
			this->handle = NULL;
		}

		File::~File()
		{
			Close();
		}

		void File::Open(const wchar_t* filename, Mode mode)
		{
			DWORD desiredAccess = 0;
			if (mode & ReadMode) desiredAccess |= GENERIC_READ;
			if (mode & WriteMode) desiredAccess |= GENERIC_WRITE;

			DWORD creationDisposition = 0;
			if (mode == AppendMode) creationDisposition = CREATE_NEW;
			else if (mode & WriteMode) creationDisposition = CREATE_ALWAYS;
			else if (mode & ReadMode) creationDisposition = OPEN_EXISTING;

			HANDLE h;    

			if (mode == WriteMode) {
				String _path(StringUtil::Replace(filename, L"/", L"\\"));
				if (_path.substr(_path.size() - 1) == L"\\") 
					_path = _path.substr(0, _path.size() - 1);
				_path.insert(0, L".\\");
				std::wstring PrefixRemovingFileName = _path.substr(0, _path.find_last_of(L"\\"));
				if (!std::filesystem::exists(PrefixRemovingFileName))
				{
					std::filesystem::create_directories(PrefixRemovingFileName);
				}
			}

			h = ::CreateFileW(filename, desiredAccess, FILE_SHARE_READ, NULL /* SecurityAttributes */
				, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL /* TemplateFile */);

			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			{
				h = ::CreateFileA(StringUtil::ToASCII(filename).c_str(), desiredAccess, FILE_SHARE_READ, NULL /* SecurityAttributes */
					, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL /* TemplateFile */);

				if ((mode & AppendMode) && h == INVALID_HANDLE_VALUE)
					h = ::CreateFileA(StringUtil::ToASCII(filename).c_str(), desiredAccess, FILE_SHARE_READ, NULL /* SecurityAttributes */
						, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL /* TemplateFile */);
			}
			else if ((mode & AppendMode) && h == INVALID_HANDLE_VALUE)
			{
				creationDisposition = OPEN_EXISTING;
				h = ::CreateFileW(filename, desiredAccess, FILE_SHARE_READ, NULL /* SecurityAttributes */
					, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL /* TemplateFile */);
			}
			if (h == INVALID_HANDLE_VALUE)
				THROWFILEEXCEPTION(filename);

			this->filename = filename;
			this->handle = h;
			if (mode & AppendMode)
				Seek(0, End);

			return;
		}

		void File::Close()
		{
			if (this->handle)
				::CloseHandle(this->handle);
			this->handle = NULL;
			this->filename = L"";
		}

		long File::Seek(long offset, SeekFrom origin)
		{
			DWORD ret = ::SetFilePointer(this->handle, offset, NULL, origin);
			if (ret == INVALID_SET_FILE_POINTER)
				THROWFILEEXCEPTION(this->filename.c_str());
			return (long)ret;
		}

		void File::Flush()
		{
			if (::FlushFileBuffers(this->handle)) return;
			THROWFILEEXCEPTION(this->filename.c_str());
		}

		int File::Read(unsigned char* buffer, long size)
		{
			DWORD read;
			if (::ReadFile(this->handle, buffer, size, &read, NULL) == 0)
				THROWFILEEXCEPTION(this->filename.c_str());
			return read;
		}

		void File::Write(const unsigned char* buffer, long size)
		{
			DWORD written;
			if (::WriteFile(this->handle, buffer, size, &written, NULL) == 0)
				THROWFILEEXCEPTION(this->filename.c_str());
		}

		HANDLE File::GetHandle()
		{
			return this->handle;
		}
	}
}
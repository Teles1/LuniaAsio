
#include "Directory.h"

namespace Lunia {
	namespace FileIO {

		int Directory::Count(const wchar_t* path)
		{
			//std::string _path=UnicodeToASCII(path);
			std::wstring _path(path);

			if (_path.substr(_path.length() - 1, 1) == L"\\")
				_path += L"*.*";
			else
				_path += L"\\*.*";

			WIN32_FIND_DATAW findFileData = { 0, };
			WIN32_FIND_DATAA findFileDataA = { 0, };
			int count = 0;

			HANDLE file = ::FindFirstFileW(_path.c_str(), &findFileData);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			{
				file = ::FindFirstFileA(StringUtil::ToASCII(_path).c_str(), &findFileDataA);
				BOOL loop = (file != INVALID_HANDLE_VALUE);
				if (!loop)
					THROWFILEEXCEPTION(path);

				while (loop)
				{
					if (!(findFileDataA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						count++;
					loop = ::FindNextFileA(file, &findFileDataA);
				}
				::FindClose(file);
			}
			else
			{
				BOOL loop = (file != INVALID_HANDLE_VALUE);
				if (!loop)
					THROWFILEEXCEPTION(path);

				while (loop)
				{
					if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						count++;
					loop = ::FindNextFileW(file, &findFileData);
				}
				::FindClose(file);
			}

			return count;
		}

		String Directory::GetCurrentDirectory()
		{
			wchar_t buf[MAX_PATH];
			::GetCurrentDirectoryW(MAX_PATH, buf);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			{
				char bufA[MAX_PATH];
				::GetCurrentDirectoryA(MAX_PATH, bufA);
				return StringUtil::ToUnicode(bufA);
			}

			return buf;
		}

		void Directory::SetCurrentDirectory(const wchar_t* path)
		{
			::SetCurrentDirectoryW(path);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				::SetCurrentDirectoryA(StringUtil::ToASCII(path).c_str());
		}

		void Directory::Create(const wchar_t* path)
		{
			String _path(StringUtil::Replace(path, L"/", L"\\"));

			if (_path.substr(_path.size() - 1) == L"\\") // remove '\\' ending
				_path = _path.substr(0, _path.size() - 1);

			if (Directory::Exists(path))
				return;

			Create(_path.substr(0, _path.find_last_of(L"\\")).c_str()); // recursive call, one less directory

			::CreateDirectoryW(_path.c_str(), NULL); // actual work
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
				::CreateDirectoryA(StringUtil::ToASCII(_path).c_str(), NULL);

		}

		bool Directory::Exists(const wchar_t* path)
		{
			try {
				if (File::GetAttributes(path) & FILE_ATTRIBUTE_DIRECTORY)
					return true;
			}
			catch (FileException&) {}
			return false;
		}

		Directory::Directory(String& path)
		{
			Open(path.c_str());
		}

		Directory::Directory(const wchar_t* path)
		{
			Open(path);
		}

		void Directory::Open(const wchar_t* path)
		{
			Close();
			this->path = path;

			std::wstring _path(path);

			StringUtil::Filename filename(path);
			if (_wcsnicmp(filename.Extension.c_str(), L"zip", 3) == 0)
			{
				OpenZip(path);
				return;
			}
			////////////////////////////////////////////////////////////////////////////////

			if (_path.substr(_path.length() - 1, 1) == L"\\")
				_path += L"*.*";
			else
				_path += L"\\*.*";

			WIN32_FIND_DATAW findFileData = { 0, };
			WIN32_FIND_DATAA findFileDataA = { 0, };

			HANDLE file = ::FindFirstFileW(_path.c_str(), &findFileData);
			if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
			{
				file = ::FindFirstFileA(StringUtil::ToASCII(_path).c_str(), &findFileDataA);
				BOOL loop = (file != INVALID_HANDLE_VALUE);
				if (!loop)
					THROWFILEEXCEPTION(path);

				while (loop)
				{
					FileInfo* info = new FileInfo;

					info->Filename = StringUtil::ToUnicode(findFileDataA.cFileName);
					info->Size = findFileData.nFileSizeLow | (findFileDataA.nFileSizeHigh << 32);
					info->Created = findFileDataA.ftCreationTime;
					info->Modified = findFileDataA.ftLastWriteTime;
					info->Attributes = (FileInfo::FileAttributes)findFileDataA.dwFileAttributes;

					files.push_back(info);

					loop = ::FindNextFileA(file, &findFileDataA);
				}
			}
			else
			{
				BOOL loop = (file != INVALID_HANDLE_VALUE);
				if (!loop)
					THROWFILEEXCEPTION(path);

				while (loop)
				{
					FileInfo* info = new FileInfo;

					info->Filename = findFileData.cFileName;
					info->Size = findFileData.nFileSizeLow | (findFileData.nFileSizeHigh << 32);
					info->Created = findFileData.ftCreationTime;
					info->Modified = findFileData.ftLastWriteTime;
					info->Attributes = (FileInfo::FileAttributes)findFileData.dwFileAttributes;

					files.push_back(info);

					loop = ::FindNextFileW(file, &findFileData);
				}
			}
			::FindClose(file);
		}

		void Directory::OpenZip(const wchar_t* /*path*/)
		{
			//Zip::Compression zip(path);
			//int cnt = zip.zip->GetNoEntries();
			////Zip::IZipArchive::FileInfo fi;
			//for (int a=0;a<cnt;a++) {
			//	FileInfo* info=new FileInfo;

			//	info->Filename = ASCIIToUnicode(zip.zip->GetFileInfo(a).filename);
			//	info->Size = zip.zip->GetFileInfo(a).size;
			//	info->Modified.dwLowDateTime = zip.zip->GetFileInfo(a).dwLowDateTime;
			//	info->Modified.dwHighDateTime = zip.zip->GetFileInfo(a).dwHighDateTime;
			//	//info->Created=findFileData.ftCreationTime;
			//	//info->Attributes=(FileInfo::FileAttributes)findFileData .dwFileAttributes;

			//	files.push_back(info);


			//}
		}

		void Directory::Close()
		{
			if (files.size() > 0)
			{
				for (FileInfoCollection::iterator i = files.begin(); i != files.end(); ++i)
					delete (*i);
				files.clear();
			}
		}

		Directory::~Directory()
		{
			Close();
		}

		FileInfoCollection& Directory::GetFiles() { return files; }

	}
}


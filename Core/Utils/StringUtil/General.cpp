#include <stdio.h>
#include <stdarg.h>
#include <Core/FileIO/Directory.h>

#include "General.h"

namespace Lunia {
	namespace StringUtil {

		std::string Format(char* format, ...) {
			char txt[1024 * 32];
			static const int size = (sizeof(txt) / sizeof(char)) - 1;

			txt[size] = 0;
			va_list l;
			va_start(l, format);
			//_vsnprintf(txt, size, format, l);
			_vsnprintf_s(txt, size, format, l);
			va_end(l);
			return txt;
		}

		std::wstring Format(wchar_t* format, ...) {
			wchar_t txt[1024 * 32];
			static const int size = (sizeof(txt) / sizeof(wchar_t)) - 1;

			txt[size] = 0;
			va_list l;
			va_start(l, format);
			//_vsnwprintf(txt, size, format, l);
			_vsnwprintf_s(txt, size, format, l);
			va_end(l);
			return txt;
		}

		unsigned int Hash(const wchar_t* str)
		{
			//wchar_t* p=const_cast<wchar_t*>(str); <--
			//replaced this, we shouldnt be doing const_cast's for no reason @juhnu
			const wchar_t* p = str;
			unsigned int hashcode = 0;
			wchar_t c = 0;
			while (*p)
			{
				c = *p; hashcode = (31 * hashcode + (c % 31)) % 64000000; ++p;
			}
			return hashcode;
		}

		std::wstring RemoveDots(const std::wstring& str2) {
			std::wstring str = str2;
			size_t pos1 = str.find(L"/..");
			while (pos1 != std::wstring::npos) {
				bool found = false;
				for (int i = int(pos1) - 1; i >= 0; --i) {
					if (str[i] == L'/') {
						str = str.substr(0, i) + str.substr(pos1 + 3);
						found = true;
						break;
					}

				}
				if (!found) {
					str = str.substr(pos1 + 3);
				}


				pos1 = str.find(L"/..");
			}
			return str;
		}

		std::string Tokenize(const char* str, const char* token, int& pos) {
			std::string tmp(str);
			std::string a;
			std::string ret;
			size_t endPos;

			if (pos < 0)
				return ret;
			endPos = tmp.find_first_of(token, pos);
			a = tmp.substr(pos, endPos - pos);
			ret = a.c_str();
			if (endPos == tmp.npos) {
				pos = -1;
			}
			else {
				endPos = tmp.find_first_not_of(token, endPos);
				if (endPos == tmp.npos)
					pos = -1;
				else pos = (int)endPos;
			}
			return ret;
		}

		void Replace(std::wstring& string, wchar_t findChar, wchar_t replaceChar)
		{
			for (size_t i = 0; i < string.size(); ++i)
			{
				if (string[i] == findChar) string[i] = replaceChar;
			}
		}

		//template<>
		std::wstring Replace(const std::wstring& string, const std::wstring& findString, const std::wstring& replaceString)
		{
			std::wstring changeString;
			size_t offset = findString.size();

			size_t lastFindPos = string.find_first_of(findString);

			if (lastFindPos == string.npos)
				return string;

			changeString = string.substr(0, lastFindPos);

			for (size_t i = lastFindPos; i < string.size(); ++i)
			{
				std::wstring compareString(&string[i], offset);

				if (compareString == findString)
				{
					changeString += string.substr(lastFindPos, i - lastFindPos);
					changeString += replaceString;

					i += offset - 1;
					lastFindPos = i + 1;
				}
			}

			changeString += string.substr(lastFindPos);

			return changeString;
		}
		//template<>
		std::string  Replace(const std::string& string, const std::string& findString, const std::string& replaceString)
		{
			std::string changeString;
			size_t offset = findString.size();

			size_t lastFindPos = string.find_first_of(findString);

			if (lastFindPos == string.npos)
				return string;

			changeString = string.substr(0, lastFindPos);

			for (size_t i = lastFindPos; i < string.size(); ++i)
			{
				std::string compareString(&string[i], offset);

				if (compareString == findString)
				{
					changeString += string.substr(lastFindPos, i - lastFindPos);
					changeString += replaceString;

					i += offset - 1;
					lastFindPos = i + 1;
				}
			}

			changeString += string.substr(lastFindPos);

			return changeString;
		}

		Filename::Filename()
		{
			Parse(L"");
		}

		Filename::Filename(const Filename& filename)
			: FileName(filename.FileName), Extension(filename.Extension), Drive(filename.Drive), Path(filename.Path)
		{
		}

		Filename::Filename(const std::wstring& filename)
		{
			Parse(filename);
		}

		void Filename::Parse(const std::wstring& filename)
		{
			if (filename.empty()) Parse(FileIO::Directory::GetCurrentDirectory());// Why doesn't it return here?

			size_t pos = filename.find(':');
			if (pos == std::wstring::npos) // no drives - need to set current working directory
			{
				std::wstring currentDirectory = FileIO::Directory::GetCurrentDirectory();
				if (filename[0] != L'\\' && filename[0] != L'/' &&
					currentDirectory[currentDirectory.size() - 1] != L'\\' && currentDirectory[currentDirectory.size() - 1] != L'/')
					currentDirectory += L'/';

				if (filename.find(L"..") == 0) // finding relative path
				{
					std::wstring f = filename;

					while (f.find(L"..") == 0)
					{
						if (f.size() < 3) break;
						if (f[2] != L'\\' && f[2] != L'/') break;
						currentDirectory = currentDirectory.substr(0, currentDirectory.find_last_of(L"\\/", currentDirectory.size() - 2) + 1);
						f = f.substr(3);
					}
					currentDirectory += f;
				}
				else
					currentDirectory += filename;

				pos = currentDirectory.find(':');
				Drive = currentDirectory.substr(0, pos + 1); // set drive

				size_t lastDotPos = currentDirectory.find_last_of(L'.');
				size_t lastSlashPos = currentDirectory.find_last_of(L"\\/");
				if (lastDotPos < lastSlashPos || lastDotPos == std::wstring::npos) // no filename (only directory)
				{
					Path = currentDirectory + filename; // set path
					if (Path[Path.size() - 1] != L'\\' && Path[Path.size() - 1] != L'/')
						Path += L'/';
					return;
				}

				Path = currentDirectory.substr(pos + 1, lastSlashPos - pos); // set path
				Extension = currentDirectory.substr(lastDotPos); // set extension
				FileName = currentDirectory.substr(lastSlashPos + 1, lastDotPos - (lastSlashPos + 1));
			}
			else // with drives - abolusute path
			{
				Drive = filename.substr(0, pos + 1); // set drive
				size_t lastDotPos = filename.find_last_of(L'.');
				size_t lastSlashPos = filename.find_last_of(L"\\/");
				if (lastDotPos < lastSlashPos || lastDotPos == std::wstring::npos) // no filename (only directory)
				{
					Path = filename; // set path
					if (Path[Path.size() - 1] != L'\\' && Path[Path.size() - 1] != L'/')
						Path += L'/';
					return;
				}
				Path = filename.substr(pos + 1, lastSlashPos - pos);
				Extension = filename.substr(lastDotPos); // set extension
				FileName = filename.substr(lastSlashPos + 1, lastDotPos - (lastSlashPos + 1));
			}
		}
	}
}
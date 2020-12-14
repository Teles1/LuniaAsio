#include "helper.h"

namespace Lunia {

	std::string UnicodeToASCII(const wchar_t* str)
	{
		int cSize = (int)wcslen(str) + 1;
		char* buf = new char[cSize * 2 + 1];
		::WideCharToMultiByte(CP_ACP, 0, str, cSize, buf, cSize * 2, NULL, NULL);
		std::string _str(buf);
		SAFE_DELETE(buf);
		return _str;
	}

	std::wstring ASCIIToUnicode(const char* str)
	{
		int cSize = (int)strlen(str) + 1;
		wchar_t* buf = new wchar_t[cSize];
		::MultiByteToWideChar(CP_ACP, 0, str, cSize, buf, cSize * 2);
		String _str(buf);
		SAFE_DELETE(buf);

		return _str;
	}
	std::string IntToASCII(int value) {
		return std::to_string(value);
	}

	std::wstring IntToUnicode(int value) {
		return std::to_wstring(value);
	}

	std::string  FloatToASCII(double value) {
		return std::to_string(value);
	}

	std::wstring FloatToUnicode(double value) {
		return std::to_wstring(value);
	}
}
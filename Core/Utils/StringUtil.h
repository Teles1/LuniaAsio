#pragma once
#include "../Core.h"
#include <iomanip>

namespace StringUtil {
	inline uint32 Hash(std::wstring str)
	{
		const wchar_t* p = str.c_str();
		unsigned int hashcode = 0;
		wchar_t c = 0;
		while (*p)
		{
			c = *p; hashcode = (31 * hashcode + (c % 31)) % 64000000; ++p;
		}
		return hashcode;
	}
	inline std::string GetString(uint8* data, uint32 length) {
		std::ostringstream ret;
		ret << std::hex << std::setfill('0');
		for (uint32 i = 1; i < length + 1; i++) {
			ret << "0x" << std::setw(2) << static_cast<unsigned int>(data[i - 1]) << " ";
			if (i % 16 == 0) { ret << "\n"; }
		}
		return ret.str();
	}
	inline std::string GetString(uint8* data, uint32 length, bool commasepareted) {
		std::ostringstream ret;
		ret << std::hex << std::setfill('0');
		for (uint32 i = 0; i < length; i++) {
			ret << (i == 0 ? " 0x" : ", 0x" ) << std::setw(2) << static_cast<unsigned int>(data[i]) << " ";
			//if (i % 16 == 0) { ret << "\n"; }
		}
		return ret.str();
	}
}

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
	inline std::string GetString(uint8 data[], uint32 lenght) {
		std::stringstream ss; // added include [ crazy in common.h ] 
		for (uint32 i = 0; i < lenght; i++)
			ss << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i];
		
		return ss.str();
	}
}

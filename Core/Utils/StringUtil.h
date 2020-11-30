#pragma once
#include "./Common.h"

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
}

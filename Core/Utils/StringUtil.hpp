#ifndef StringUtil_HEADER_GUARD
#define StringUtil_HEADER_GUARD

#pragma once
#include "../Core.h"
#include <iomanip>
#include <stdarg.h>

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
	class StringBuilder {
	public:
		StringBuilder() {
		}
		StringBuilder(const char* in) {
			sData.assign(std::move(in));
		}
		StringBuilder(std::string& in) {
			sData = std::move(in);
		}
		template<typename T>
		StringBuilder& operator << (const T& inData) {
			Append(inData);
			return *this;
		}
		std::string& GetString() {
			return sData;
		}
		void Append(const std::string& in) {
			sData.append(in);
		}
		void Append(const char* in) {
			sData.append(std::move(in));
		}
		void Append(const short& in) {
			sData.append(std::move(std::to_string(in)));
		}
		void Append(const int& in) {
			sData.append(std::move(std::to_string(in)));
		}
		~StringBuilder() {}
	private:
		std::string sData = "";
	};
}
#endif // !StringUtil_HEADER_GUARD
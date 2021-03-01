#pragma once
namespace Lunia {
	namespace StringUtil {
		namespace UTF8 {
			std::wstring Decode(const char*);
			std::wstring Decode(const std::string&);

			std::string  Encode(const std::wstring&);
			std::string  Encode(const wchar_t*);
		}
	}
}
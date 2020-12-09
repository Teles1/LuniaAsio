#include "GeneralConversions.h"
namespace Lunia {
	namespace StringUtil{

#pragma region Unicode
		template<> std::wstring inline To(const unsigned char& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned short& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned int& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned long long& value) { return std::to_wstring(value); }

		template<> std::wstring inline To(const char& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const short& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const int& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long double& value) { return std::to_wstring(value); }
#pragma endregion
#pragma region ASCII
		template<> std::string inline To(const unsigned char& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned short& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned int& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned long& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned long long& value) { return std::to_string(value); }

		template<> std::string inline To(const char& value) { return std::to_string(value); }
		template<> std::string inline To(const short& value) { return std::to_string(value); }
		template<> std::string inline To(const int& value) { return std::to_string(value); }
		template<> std::string inline To(const long& value) { return std::to_string(value); }
		template<> std::string inline To(const long long& value) { return std::to_string(value); }
		template<> std::string inline To(const long double& value) { return std::to_string(value); }
#pragma endregion
	}
}
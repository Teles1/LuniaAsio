#include <sstream>
#include <codecvt>
#include <iostream>
#include <fmt/format.h>
#include "Header.h"

#pragma region Unicode
template<> std::wstring inline To(const unsigned char& value) { return std::to_wstring(value); }
template<> std::wstring inline To(const unsigned short& value) { return std::to_wstring(value);}
template<> std::wstring inline To(const unsigned int& value) {	return std::to_wstring(value);}
template<> std::wstring inline To(const unsigned long& value) { return std::to_wstring(value);}
template<> std::wstring inline To(const unsigned long long& value) { return std::to_wstring(value);}

template<> std::wstring inline To(const char& value) {	return std::to_wstring(value);}
template<> std::wstring inline To(const short& value) { return std::to_wstring(value);}
template<> std::wstring inline To(const int& value) { return std::to_wstring(value);}
template<> std::wstring inline To(const long& value) {	return std::to_wstring(value);}
template<> std::wstring inline To(const long long& value) { return std::to_wstring(value);}
template<> std::wstring inline To(const unsigned long long& value) { return std::to_wstring(value); }
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
template<> std::string inline To(const unsigned long long& value) { return std::to_string(value); }
#pragma endregion
template<typename T>
std::wstring ToUnicode(const T& value ) {
	return To(value);
}
template<typename T>
std::string ToUnicode(const T& value) {
	return To(value);
}

int main() {
	unsigned char value = 100;
	std::wstring aux = ToUnicode(value);
	return 0;
}
//return StringUtil::Format(L"%04d-%02d-%02d", dateValue.Year, dateValue.Month, dateValue.Day);
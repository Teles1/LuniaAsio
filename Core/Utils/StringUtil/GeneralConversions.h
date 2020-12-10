#ifndef GeneralConversions_GUARD
#define GeneralConversions_GUARD

#pragma once
#include <sstream>
#include <codecvt>
namespace Lunia {
	namespace StringUtil {

        template<typename T1, typename T2>
        T1 To(T2 value);

        template <typename T> T To(const char* str);
        template <typename T> inline T To(char* str) { return To<T>((const char*)str); }
        template <typename T> inline T To(const std::string& str) { return To<T>(str.c_str()); }

        template <typename T> T To(const wchar_t* wstr);
        template <typename T> inline T To(wchar_t* wstr) { return To<T>((const wchar_t*)wstr); }
        template <typename T> inline T To(const std::wstring& wstr) { return To<T>(wstr.c_str()); }

        #pragma region Type Conversions
        template<typename T> double inline ToDouble(const T& str) { return To<double>(str); }
        template<typename T> double inline ToDouble(const T* str) { return To<double>(str); }

        template<typename T> float ToFloat(const T& str) {  return To<float>(str);  }
        template<typename T> float inline ToFloat(const T* str) {  return To<float>(str);  }

        template<typename T> unsigned long long int inline ToUint64(const T& str) { return To<unsigned long long int>(str); }
        template<typename T> unsigned int inline ToUint32(const T& str) { return To<unsigned int>(str); }
        template<typename T> unsigned short inline ToUint16(const T& str) { return To<unsigned int>(str); }
        template<typename T> unsigned char inline ToUint8(const T& str) { return To<unsigned int>(str);}

        template<typename T> unsigned long long int inline ToUint64(const T* str) { return To<unsigned long long int>(str); }
        template<typename T> unsigned int inline ToUint32(const T* str) { return To<unsigned int>(str); }
        template<typename T> unsigned short inline ToUint16(const T* str) { return To<unsigned int>(str); }
        template<typename T> unsigned char inline ToUint8(const T* str) { return To<unsigned int>(str);}

		template<typename T> long long int inline ToInt64(const T& str) { return To<long long int>(str); }
        template<typename T> int inline ToInt32(const T& str) { return To<int>(str); }
        template<typename T> short inline ToInt16(const T& str) { return To<short>(str); }

		template<typename T> long long int inline ToInt64(const T* str) { return To<long long int>(str); }
        template<typename T> int inline ToInt32(const T* str) { return To<int>(str); }
        template<typename T> short inline ToInt16(const T* str) { return To<short>(str); }

        char ToInt8(const std::string& str);
        char ToInt8(const char* str);

        #pragma endregion

        template<typename T>
        std::wstring inline ToUnicode(const T& p) { return To<std::wstring>(p); }
        template<typename T>
        std::wstring inline ToUnicode(const T* p) { return To<std::wstring>(p); }

        template<typename T>
        std::string inline ToASCII(const T& p) { return To<std::string>(p); }
        template<typename T>
        std::string inline ToASCII(const T* p) { return To<std::string>(p); }

		#pragma region To Unicode From Number
        template<> std::wstring inline To(unsigned char value) { return std::to_wstring(value); }
        template<> std::wstring inline To(unsigned short value) { return std::to_wstring(value); }
        template<> std::wstring inline To(unsigned int value) { return std::to_wstring(value); }
        template<> std::wstring inline To(unsigned long value) { return std::to_wstring(value); }
        template<> std::wstring inline To(unsigned long long value) { return std::to_wstring(value); }

        template<> std::wstring inline To(char value) { return std::to_wstring(value); }
        template<> std::wstring inline To(short value) { return std::to_wstring(value); }
        template<> std::wstring inline To(int value) { return std::to_wstring(value); }
        template<> std::wstring inline To(long value) { return std::to_wstring(value); }
        template<> std::wstring inline To(long long value) { return std::to_wstring(value); }
        template<> std::wstring inline To(long double value) { return std::to_wstring(value); }
        template<> std::wstring inline To(double value) { return std::to_wstring(value); }
        template<> std::wstring inline To(float value) { return std::to_wstring(value); }
		#pragma endregion

		#pragma region To ASCII From Number
		template<> std::string inline To(unsigned char value) { return std::to_string(value); }
		template<> std::string inline To(unsigned short value) { return std::to_string(value); }
		template<> std::string inline To(unsigned int value) { return std::to_string(value); }
		template<> std::string inline To(unsigned long value) { return std::to_string(value); }
		template<> std::string inline To(unsigned long long value) { return std::to_string(value); }

		template<> std::string inline To(char value) { return std::to_string(value); }
		template<> std::string inline To(short value) { return std::to_string(value); }
		template<> std::string inline To(int value) { return std::to_string(value); }
		template<> std::string inline To(long value) { return std::to_string(value); }
		template<> std::string inline To(long long value) { return std::to_string(value); }

		template<> std::string inline To(long double value) { return std::to_string(value); }
		template<> std::string inline To(double value) { return std::to_string(value); }
		template<> std::string inline To(float value) { return std::to_string(value); }
		#pragma endregion

		#pragma region Unicode/ASCII
		template<> std::string inline To(const std::wstring utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> std::string inline To(std::wstring& utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> std::wstring inline To(const std::string utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		template<> std::wstring inline To(std::string& utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		template<> std::string inline To(const wchar_t* utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> std::string inline To(wchar_t* utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> std::wstring inline To(const char* utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		template<> std::wstring inline To(char* utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		#pragma endregion

		#pragma region To Number from Unicode

		template<> unsigned char inline To(const wchar_t* str) { return _wtoi(str); }
		template<> unsigned short inline To(const wchar_t* str) { return _wtoi(str); }
		template<> unsigned int inline To(const wchar_t* str) { return _wtoi(str); }
		template<> unsigned long inline To(const wchar_t* str) { return _wtol(str); }
		template<> unsigned long long inline To(const wchar_t* str) { return _wtoll(str); }

		template<> char inline To(const wchar_t* str) { return _wtoi(str); }
		template<> short inline To(const wchar_t* str) { return _wtoi(str); }
		template<> int inline To(const wchar_t* str) { return _wtoi(str); }
		template<> long inline To(const wchar_t* str) { return _wtol(str); }
		template<> long long inline To(const wchar_t* str) { return _wtoll(str); }
		template<> float inline To(const wchar_t* str) { return (float)_wtof(str); }

		template<> double inline To(const wchar_t* str) { return _wtof(str); }

		#pragma endregion

		#pragma region To Number from ASCII

		template<> unsigned char inline To(const char* str) { return atoi(str); }
		template<> unsigned short inline To(const char* str) { return atoi(str); }
		template<> unsigned int inline To(const char* str) { return atoi(str); }
		template<> unsigned long inline To(const char* str) { return atol(str); }
		template<> unsigned long long inline To(const char* str) { return atoll(str); }

		template<> char inline To(const char* str) { return atoi(str); }
		template<> short inline To(const char* str) { return atoi(str); }
		template<> int inline To(const char* str) { return atoi(str); }
		template<> long inline To(const char* str) { return atol(str); }
		template<> long long inline To(const char* str) { return atoi(str); }
		template<> float inline To(const char* str) { return (float)atof(str); }

		template<> double inline To(const char* str) { return atof(str); }

#pragma endregion

    }
}

#endif // !GeneralConversions_GUARD

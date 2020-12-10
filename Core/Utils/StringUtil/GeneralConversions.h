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
        template<typename T>
        const double ToDouble(const T& str) {
            return To<double>(str);
        }
        template<typename T>
        const float ToFloat(const T& str) {
            return To<float>(str);
        }

        template<typename T>
        const unsigned long long int ToUint64(const T& str) {
            return To<unsigned long long int>(str);
        }
        template<typename T>
        const unsigned int ToUint32(const T& str) {
            return To<unsigned int>(str);
        }
        template<typename T>
        const unsigned short ToUint16(const T& str) {
            return To<unsigned int>(str);
        }
        template<typename T>
        const unsigned char ToUint8(const T& str) {
            return To<unsigned int>(str);
        }

        template<typename T>
        const long long int ToInt64(const T& str) {
            return To<long long int>(str);
        }
        template<typename T>
        const int ToInt32(const T& str) {
            return To<int>(str);
        }
        template<typename T>
        const short ToInt16(const T& str) {
            return To<short>(str);
        }

        const char ToInt8(const std::string& str);

        #pragma endregion

        template<typename T>
        std::wstring ToUnicode(const T& p) { return To<std::wstring>(p); }
        template<typename T>
        std::wstring ToUnicode(const T* p) { return To<std::wstring>(p); }

        template<typename T>
        std::string ToASCII(const T& p) { return To<std::string>(p); }
        template<typename T>
        std::string ToASCII(const T* p) { return To<std::string>(p); }
    }
}

#endif // !GeneralConversions_GUARD

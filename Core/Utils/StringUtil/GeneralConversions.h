#ifndef GeneralConversions_GUARD
#define GeneralConversions_GUARD

#pragma once
#include <sstream>
#include <codecvt>
namespace Lunia {
	namespace StringUtil {
        template<typename T1, typename T2>
        T1 To(T2 value);
        #pragma region Base Conversions
        template<typename T>
        const T To(const std::wstring& str) {
            std::wstring temp;
            for (size_t i = 0; i < str.size(); i++) {
                if (std::isdigit(str[i]))
                    temp.push_back(str[i]);
            }
            std::wistringstream ss(std::move(temp));
            T number(0);
            ss >> number;
            return std::move(number);
        }

        template<typename T>
        const T To(const std::string& str) {
            std::string temp;
            for (size_t i = 0; i < str.size(); i++) {
                if (std::isdigit(str[i]))
                    temp.push_back(str[i]);
            }
            std::stringstream ss(std::move(temp));
            T number(0);
            ss >> number;
            return std::move(number);
        }
        #pragma endregion

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
        const char ToInt8(const std::string& str) {
            return To<char>(str);
        }

        #pragma endregion

        template<typename T>
        std::wstring ToUnicode(const T& p) { return To<std::wstring>(p); }
        template<typename T>
        std::wstring ToUnicode(const T* p) { return To<std::wstring>(p); }

        template<typename T>
        std::string ToASCII(const T& p) { return To<std::string>(p); }
        template<typename T>
        std::string ToASCII(const T* p) { return To<std::string>(p); }

        #pragma region String Conversions
        const std::wstring ToUnicode(const std::string& utf8) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
            return wcu8.from_bytes(utf8);
        }
        const std::string ToASCII(const std::wstring& utf16) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
            return wcu8.to_bytes(utf16);
        }
        #pragma endregion
    }
}

#endif // !GeneralConversions_GUARD

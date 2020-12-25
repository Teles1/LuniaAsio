#include "GenericConversion.h"

//#include "../Base.h"
#include <windows.h>
#include <stdlib.h>
#include <string>

namespace Lunia {
	namespace StringUtil {

		template<> std::string To(const wchar_t* wstr) {
			//int iWstrSize = (int)wcslen(wstr);
			int iSize = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			std::string str(iSize - 1, 0);
			::WideCharToMultiByte(CP_ACP, 0, wstr, -1, &str[0], iSize, NULL, NULL);
			return str;
		}

		template <> std::string To(const wstringIteratorPair& StrPair) {
			//int iWstrSize = (int)(StrPair.second - StrPair.first);
			int iSize = WideCharToMultiByte(CP_ACP, 0, &(*StrPair.first), -1, NULL, 0, NULL, NULL);
			//int iSize = (int)(StrPair.second - StrPair.first);
			std::string str(iSize - 1, 0);
			::WideCharToMultiByte(CP_ACP, 0, &(*StrPair.first), -1, &str[0], iSize, NULL, NULL);
			return str;
		}

		template<> std::wstring To(const char* str) {
			//int iStrSize = (int)strlen(str);
			int iSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
			//int iSize = (int)strlen(str);
			std::wstring wstr(iSize - 1, 0);
			::MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], iSize);
			return wstr;
		}

		template<> std::wstring To(const wchar_t* str)
		{
			return std::wstring(str);
		}

		template<> std::string To(const char* str)
		{
			return std::string(str);
		}

		template <> std::wstring To(const stringIteratorPair& StrPair) {
			//int iStrSize =(int)(StrPair.second - StrPair.first);
			int iSize = MultiByteToWideChar(CP_ACP, 0, &(*StrPair.first), -1, NULL, 0);
			//int iSize = (int)(StrPair.second - StrPair.first);
			std::wstring wstr(iSize - 1, 0);
			::MultiByteToWideChar(CP_ACP, 0, &(*StrPair.first), -1, &wstr[0], iSize);
			return wstr;
		}

		template <> std::string To(int value) {
			return std::to_string(value);
		}

		template <> std::wstring To(int value) {
			return std::to_wstring(value);
		}

		template <> std::string To(short value) {
			return std::to_string(value);
		}

		template <> std::wstring To(short value) {
			return std::to_wstring(value);
		}

		template <> std::string To(unsigned short value) {
			return std::to_string(value);
		}

		template <> std::wstring To(unsigned short value) {
			return std::to_wstring(value);
		}

		template <> std::string To(unsigned int value) {
			return std::to_string(value);
		}

		template <> std::wstring To(unsigned int value) {
			return std::to_wstring(value);
		}

		template <> std::wstring To(unsigned __int64 value) {
			return std::to_wstring(value);
		}

		template <> std::wstring To(__int64 value) {
			return std::to_wstring(value);
		}

		template <> std::string To(unsigned __int64 value) {
			return std::to_string(value);
		}

		template <> std::string To(__int64 value) {
			return std::to_string(value);
		}

		template <> std::string To(long value) {
			return std::to_string(value);
		}

		template <> std::wstring To(long value) {
			return std::to_wstring(value);
		}

		template <> std::string To(unsigned long value) {
			return std::to_string(value);
		}

		template <> std::wstring To(unsigned long value) {
			return std::to_wstring(value);
		}

		template <> std::wstring To(float value) {
			return std::to_wstring(value);
		}

		template <> std::string To(float value) {
			return std::to_string(value);
		}

		template <> std::wstring To(double value) {
			return std::to_wstring(value);
		}

		template <> std::string To(double value) {
			return std::to_string(value);
		}

		template <> std::wstring To(bool value) {
			return std::wstring(value ? L"true" : L"false");
		}

		template <> std::string To(bool value) {
			return std::string(value ? "true" : "false");
		}

		template <> std::wstring To(char value) {
			return std::to_wstring(value);
		}

		template <> std::string To(char value) {
			return std::to_string(value);
		}

		template <> std::wstring To(unsigned char value) {
			return std::to_wstring(value);
		}

		template <> std::string To(unsigned char value) {
			return std::to_string(value);
		}

		template <> std::wstring To(float2 value) {
			wchar_t wszBuffer[100 * 3];
			swprintf_s(wszBuffer, L"(%f, %f)", value.x, value.y);
			return std::wstring(wszBuffer);
		}

		template <> std::wstring To(float3 value) {
			wchar_t wszBuffer[100 * 4];
			swprintf_s(wszBuffer,  L"(%f, %f, %f)", value.x, value.y, value.z);
			return std::wstring(wszBuffer);
		}

		template <> std::string To(float3 value) {
			char szBuffer[100 * 4];
			sprintf_s(szBuffer, "(%f, %f, %f)", value.x, value.y, value.z);
			return std::string(szBuffer);
		}

		template <> std::wstring To(float4 value) {
			wchar_t wszBuffer[100 * 5];
			swprintf_s(wszBuffer, L"(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
			return std::wstring(wszBuffer);
		}

		template <> std::wstring To(float4x4 value) {
			wchar_t wszBuffer[100 * 5 * 5];
			swprintf_s(wszBuffer, sizeof(wszBuffer), L"(%f, %f, %f, %f,", value[0].x, value[0].y, value[0].z, value[0].w);
			swprintf_s(wszBuffer, sizeof(wszBuffer), L"%f, %f, %f, %f,", value[1].x, value[1].y, value[1].z, value[1].w);
			swprintf_s(wszBuffer, sizeof(wszBuffer), L"%f, %f, %f, %f,", value[2].x, value[2].y, value[2].z, value[2].w);
			swprintf_s(wszBuffer, sizeof(wszBuffer), L"%f, %f, %f, %f)", value[3].x, value[3].y, value[3].z, value[3].w);
			return std::wstring(wszBuffer);
		}

		template <> double	To(const char* str) { return atof(str); }
		template <> double	To(const stringIteratorPair& StrPair) { return To<double>(std::string(StrPair.first, StrPair.second)); }

		template <> float	To(const char* str) { return (float)To<double>(str); }
		template <> float	To(const stringIteratorPair& StrPair) { return (float)To<double>(StrPair); }

		template <> int		To(const char* str) { return atoi(str); }
		template <> int		To(const stringIteratorPair& StrPair) { return To<int>(std::string(StrPair.first, StrPair.second)); }

		template <> __int64	To(const char* str) { return _atoi64(str); }
		template <> __int64	To(const stringIteratorPair& StrPair) { return To<int>(std::string(StrPair.first, StrPair.second)); }

		template <> unsigned __int64	To(const char* str) { return _atoi64(str); }
		template <> unsigned __int64	To(const stringIteratorPair& StrPair) { return To<int>(std::string(StrPair.first, StrPair.second)); }

		template <> double	To(const wchar_t* wstr) { return _wtof(wstr); }
		template <> double	To(const wstringIteratorPair& StrPair) { return To<double>(std::wstring(StrPair.first, StrPair.second)); }

		template <> float	To(const wchar_t* wstr) { return (float)To<double>(wstr); }
		template <> float	To(const wstringIteratorPair& StrPair) { return (float)To<double>(StrPair); }

		template <> int		To(const wchar_t* str) { return _wtoi(str); }
		template <> int		To(const wstringIteratorPair& StrPair) { return To<int>(std::wstring(StrPair.first, StrPair.second)); }

		template <> __int64	To(const wchar_t* str) { return _wtoi64(str); }
		template <> __int64	To(const wstringIteratorPair& StrPair) { return To<__int64>(std::wstring(StrPair.first, StrPair.second)); }

		template <> unsigned __int64	To(const wchar_t* str) { return _wtoi64(str); }
		template <> unsigned __int64	To(const wstringIteratorPair& StrPair) { return To<__int64>(std::wstring(StrPair.first, StrPair.second)); }

		template <> short	To(const wchar_t* str) { return (short)_wtoi(str); }
		template <> short	To(const wstringIteratorPair& StrPair) { return To<short>(std::wstring(StrPair.first, StrPair.second)); }

		template <> long	To(const wchar_t* str) { return _wtol(str); }
		template <> long	To(const wstringIteratorPair& StrPair) { return To<long>(std::wstring(StrPair.first, StrPair.second)); }

		template <> unsigned int To(const wchar_t* str) { return wcstoul(str, 0, 0); }
		template <> unsigned int To(const wstringIteratorPair& StrPair) { return To<unsigned int>(std::wstring(StrPair.first, StrPair.second)); }

		template <> unsigned int To(const char* str) { return strtoul(str, 0, 0); }
		template <> unsigned int To(const stringIteratorPair& StrPair) { return To<unsigned int>(std::string(StrPair.first, StrPair.second)); }

		template <> unsigned short To(const wchar_t* str) { return (unsigned short)wcstoul(str, 0, 0); }
		template <> unsigned short To(const wstringIteratorPair& StrPair) { return To<unsigned short>(std::wstring(StrPair.first, StrPair.second)); }

		template <> unsigned long	To(const wchar_t* str) { return (unsigned long)wcstoul(str, 0, 0); }
		template <> unsigned long	To(const wstringIteratorPair& StrPair) { return To<unsigned long>(std::wstring(StrPair.first, StrPair.second)); }

		template <> bool	To(const wchar_t* str) { return _wcsicmp(str, L"false") != 0; }
		template <> bool	To(const wstringIteratorPair& StrPair) { return To<bool>(std::wstring(StrPair.first, StrPair.second).c_str()); }

		template <> char	To(const wchar_t* str) { return (char)To<int>(str); }
		template <> char	To(const wstringIteratorPair& StrPair) { return (char)To<int>(StrPair); }

		template <> unsigned char	To(const wchar_t* str) { return (unsigned char)To<unsigned int>(str); }
		template <> unsigned char	To(const wstringIteratorPair& StrPair) { return (unsigned char)To<unsigned int>(StrPair); }

		namespace Private {
			template<typename T>
			class Tokens {};

			template<>
			class Tokens<char> {
			public:
				static const char		Minus = '-';
				static const char		LeftParenthesis = '(';
				static const char		RightParenthesis = ')';
				static const char		Comma = ',';
				static const char		Point = '.';
				static const char		Null = '\0';
				static const char		Zero = '0';
				static const char		Nine = '9';
				static const char		Space = ' ';
				static const char		Tab = '\t';
				static const char		LineFeed = '\n';
				static const char		CarriageReturn = '\r';
			};

			template<>
			class Tokens<wchar_t> {
			public:
				static const wchar_t	Minus = L'-';
				static const wchar_t	LeftParenthesis = L'(';
				static const wchar_t	RightParenthesis = L')';
				static const wchar_t	Comma = L',';
				static const wchar_t	Point = L'.';
				static const wchar_t	Null = L'\0';
				static const wchar_t	Zero = L'0';
				static const wchar_t	Nine = L'9';
				static const wchar_t	Space = L' ';
				static const wchar_t	Tab = L'\t';
				static const wchar_t	LineFeed = L'\n';
				static const wchar_t	CarriageReturn = L'\r';
			};

			template<typename T>
			bool GetToken(T** pStr, T tToken) {
				if (**pStr == tToken) { (*pStr)++; return true; }
				return false;
			}

			template<typename T>
			bool IsNumber(T tChar) {
				return (Tokens<T>::Zero <= tChar && tChar <= Tokens<T>::Nine) ? true : false;
			}

			template<typename T>
			bool IsWhiteSpace(T tChar) {
				return (tChar == Tokens<T>::Space || tChar == Tokens<T>::Tab ||
					tChar == Tokens<T>::LineFeed || tChar == Tokens<T>::CarriageReturn) ? true : false;
			}

			template<typename T>
			void SkipWhitespace(T** pStr) {
				while (IsWhiteSpace(**pStr))	(*pStr)++;
			}

			template<typename T>
			void SkipNumbers(T** pStr) {
				while (IsNumber(**pStr))		(*pStr)++;
			}

			template<typename T>
			void SkipFloat(T** pStr) {
				//sign
				GetToken(pStr, Tokens<T>::Minus);
				//number before point
				SkipNumbers(pStr);
				//point
				GetToken(pStr, Tokens<T>::Point);
				//number after point
				SkipNumbers(pStr);
			}
		}


		template <> float2	To(const char* str) {
			using namespace Private;

			float2 ret;
			char* pRead = const_cast<char*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, '(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float2	To(const stringIteratorPair& StrPair) { return To<float2>(std::string(StrPair.first, StrPair.second).c_str()); }

		template <> float3	To(const char* str) {
			using namespace Private;

			float3 ret;
			char* pRead = const_cast<char*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, '(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//z
			SkipWhitespace(&pRead);
			ret.z = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float3	To(const stringIteratorPair& StrPair) { return To<float3>(std::string(StrPair.first, StrPair.second).c_str()); }

		template <> float4	To(const char* str) {
			using namespace Private;

			float4 ret;
			char* pRead = const_cast<char*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, '(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//z
			SkipWhitespace(&pRead);
			ret.z = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, ',')) return ret;
			//w
			SkipWhitespace(&pRead);
			ret.w = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float4	To(const stringIteratorPair& StrPair) { return To<float4>(std::string(StrPair.first, StrPair.second).c_str()); }


		template <> float2	To(const wchar_t* str) {
			using namespace Private;

			float2 ret;
			wchar_t* pRead = const_cast<wchar_t*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L'(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float2	To(const wstringIteratorPair& StrPair) { return To<float2>(std::wstring(StrPair.first, StrPair.second).c_str()); }

		template <> float3	To(const wchar_t* str) {
			using namespace Private;

			float3 ret;
			wchar_t* pRead = const_cast<wchar_t*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L'(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//z
			SkipWhitespace(&pRead);
			ret.z = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float3	To(const wstringIteratorPair& StrPair) { return To<float3>(std::wstring(StrPair.first, StrPair.second).c_str()); }

		template <> float4	To(const wchar_t* str) {
			using namespace Private;

			float4 ret;
			wchar_t* pRead = const_cast<wchar_t*>(str);

			//(
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L'(')) return ret;
			//x
			SkipWhitespace(&pRead);
			ret.x = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//y
			SkipWhitespace(&pRead);
			ret.y = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//z
			SkipWhitespace(&pRead);
			ret.z = To<float>(pRead);
			SkipFloat(&pRead);
			//,
			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L',')) return ret;
			//w
			SkipWhitespace(&pRead);
			ret.w = To<float>(pRead);
			SkipFloat(&pRead);
			//need not check )
			return ret;
		}
		template<> float4	To(const wstringIteratorPair& StrPair) { return To<float4>(std::wstring(StrPair.first, StrPair.second).c_str()); }


		template <> float4x4	To(const wchar_t* str) {
			using namespace Private;
			float4x4 ret;
			wchar_t* pRead = const_cast<wchar_t*>(str);

			SkipWhitespace(&pRead);
			if (!GetToken(&pRead, L'(')) return ret;
			for (int i = 0; i < 16; ++i) {
				SkipWhitespace(&pRead);
				ret[0][i] = To<float>(pRead);
				SkipFloat(&pRead);
				SkipWhitespace(&pRead);
				if (!GetToken(&pRead, L',')) return ret;
			}
			return ret;
		}
		template<> float4x4	To(const wstringIteratorPair& StrPair) { return To<float4x4>(std::wstring(StrPair.first, StrPair.second).c_str()); }





	}
}
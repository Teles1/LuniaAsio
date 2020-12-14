#ifndef ALLM_STRINGUTIL_GENERICCONVERSION_H
#define ALLM_STRINGUTIL_GENERICCONVERSION_H

#include "../Math/math.h"
//#include "../Exception.h"
namespace Lunia {

	/**

	Conversions work like


	To<float>("123");
	To<float>(L"123");

	To<float2>(L"123,23,23,34");
	...

	To<std::string>(L"hello world");
	To<std::wstring("hello world");


	*/
	namespace StringUtil {

		typedef std::pair<std::string::const_iterator, std::string::const_iterator> stringIteratorPair;
		typedef std::pair<std::wstring::const_iterator, std::wstring::const_iterator> wstringIteratorPair;

		template <typename T1, typename T2> T1 To(T2 value);
		//generic conversion functions from string to actual type, also supports single(ascii)-wide(unicode conversions) character, if conversion to
		//some type is not supported you will get a LINKING error
		template <typename T> T To(const char* str);
		template <typename T> inline T To(char* str) { return To<T>((const char*)str); }
		template <typename T> inline T To(const std::string& str) { return To<T>(str.c_str()); }
		template <typename T>  T To(const stringIteratorPair& StrPair);

		template <typename T> T To(const wchar_t* wstr);
		template <typename T> inline T To(wchar_t* wstr) { return To<T>((const wchar_t*)wstr); }
		template <typename T> inline T To(const std::wstring& wstr) { return To<T>(wstr.c_str()); }
		template <typename T> T To(const wstringIteratorPair& StrPair);


		//methods for converting between upper and lower case, supports both single and wide byte conversions
		template <typename fc> inline void ToUpperCase(fc& s) { std::transform(s.begin(), s.end(), s.begin(), toupper); }
		template <typename fc> inline void ToLowerCase(fc& s) { std::transform(s.begin(), s.end(), s.begin(), tolower); }


		//@deprecated: convenience methods for people that get scared by templates, however I strongly recommend using templates directly  -juhnu
		template <typename T> inline float		ToFloat(const T& p) { return To<float>(p); }
		template <typename T> inline float		ToFloat(const T* p) { return To<float>(p); }

		template <typename T> inline double	ToDouble(const T& p) { return To<double>(p); }
		template <typename T> inline double	ToDouble(const T* p) { return To<double>(p); }

		template <typename T> inline int		ToInt(const T& p) { return To<int>(p); }
		template <typename T> inline int		ToInt(const T* p) { return To<int>(p); }

		template <typename T> inline __int64 ToInt64(const T& p) { return To<__int64>(p); }
		template <typename T> inline __int64 ToInt64(const T* p) { return To<__int64>(p); }

		template <typename T> inline float2	ToFloat2(const T& p) { return To<float2>(p); }
		template <typename T> inline float2	ToFloat2(const T* p) { return To<float2>(p); }

		template <typename T> inline float3	ToFloat3(const T& p) { return To<float3>(p); }
		template <typename T> inline float3	ToFloat3(const T* p) { return To<float3>(p); }

		template <typename T> inline float4	ToFloat4(const T& p) { return To<float4>(p); }
		template <typename T> inline float4	ToFloat4(const T* p) { return To<float4>(p); }


		//@deprecated: convenience methods for single - wide byte conversions
		template <typename T> inline std::string	ToASCII(const T& p) { return To<std::string>(p); }
		template <typename T> inline std::string	ToASCII(const T* p) { return To<std::string>(p); }

		template <typename T> inline std::wstring	ToUnicode(const T& p) { return To<std::wstring>(p); }
		template <typename T> inline std::wstring	ToUnicode(const T* p) { return To<std::wstring>(p); }


		/**
		  there are two way of string conversion. first, overloading the To method by template specialization can be solution but
		  that makes link-time error by no-supporting the conversion. next, overriding ToString and Parse method (IConvertable interface)
		  can be another to more code-descriptable(compile-time check), but that makes vtable and more overhead.<br>
		  compare
		  @code
			  SomeStruct s;
			  IConvertable& c(s);
			  wcout << c.ToString();
		  @endcode
		  to
		  @code
			  SomeStruct s;
			  wcout << To<wstring>(s);
		  @endcode
		
		class IConvertable
		{
		public:
			class ParseException : public Exception { public: ParseException(const wchar_t* msg) : Exception(msg) { } };

			virtual std::wstring ToString() const = 0;
			virtual void Parse(const std::wstring&) = 0;
		};
		*/
	}
}


#endif
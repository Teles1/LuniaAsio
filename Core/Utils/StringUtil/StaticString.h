#ifndef StaticString_GUARD
#define StaticString_GUARD
#pragma once

#include <assert.h>
#include <string>

namespace Lunia {
	namespace StringUtil
	{
		/**
				replacement for plain wchar_t/char arrays

				static_wstring
				static_string

				A string class which uses statically allocated memory. this string class can be safely passed through dll-boundaries and should be used
				in the situations where std::string and std::wstring cannot be used due to dll or the performance reasons.

				@date 2004.9.7
				@author juhnu

				static_wstring<512> hello=L"hello world";
				static_wstring<100> tmp;
				tmp=hello;
				static_wstring<200> bla=std::wstring(L"hello");
				hello+=tmp;
				tmp= hello + tmp + L"blabal";

				*/
		template <typename T, int Size>
		class StaticString
		{

		public:
			enum
			{
				MaxSize = Size
			};
			template <int FS>
			StaticString<T, Size>(const StaticString<T, FS>& value)
			{
				set(value.c_str());
			}

			StaticString<T, Size>() : len(0)
			{
				buffer[0] = 0;
			}

			StaticString<T, Size>(const T* value)
			{
				set(value);
			}

			StaticString<T, Size>(const T* begin, const T* end)
			{
				set(begin, int(end - begin));
			}

			StaticString<T, Size>(const std::wstring& value)
			{
				set(value.c_str());
			}

			StaticString<T, Size>(const std::string& value)
			{
				set(value.c_str());
			}

			template <int FS>
			StaticString<T, Size>& operator=(const StaticString<T, FS>& value)
			{
				set(value.c_str());
				return *this;
			}

			template <int FS>
			StaticString<T, Size>& operator=(const std::wstring& value)
			{
				set(value.c_str());
				return *this;
			}

			template <int FS>
			StaticString<T, Size>& operator=(const std::string& value)
			{
				set(value.c_str());
				return *this;
			}

			const T* c_str() const { return buffer; }
			T* c_str() { return buffer; }

			size_t size() const
			{
				return len;
				//return getSize(buffer);
			}

			bool empty() const
			{
				return (size() == 0);
			}

			size_t max_size() const { return MaxSize; }

			const T* begin() const
			{
				return buffer;
			}

			T* begin()
			{
				return buffer;
			}

			const T* end() const
			{
				return buffer + size();
			}

			T* end()
			{
				return buffer + size();
			}

			const T& operator[](size_t index) const
			{
				assert((index >= 0) && (index < (Size - 1)));
				return buffer[index];
			}

			T& operator[](size_t index)
			{
				assert((index >= 0) && (index < (Size - 1)));
				return buffer[index];
			}

			template <int FS>
			StaticString<T, Size> operator+(const StaticString<T, FS>& value) const
			{
				StaticString<T, Size> tmp(*this);
				tmp.append(value.c_str());
				return tmp;
			}

			void operator+=(const T* value)
			{
				append(value);
			}

			void operator+=(const std::wstring& value)
			{
				append(value.c_str());
			}

			void operator+=(const std::string& value)
			{
				append(value.c_str());
			}

			StaticString<T, Size> operator+(const T* value) const
			{
				StaticString<T, Size> tmp(*this);
				tmp.append(value);
				return tmp;
			}

			StaticString<T, Size> operator+(const std::wstring& value) const
			{
				StaticString<T, Size> tmp(*this);
				tmp.append(value.c_str());
				return tmp;
			}

			StaticString<T, Size> operator+(const std::string& value) const
			{
				StaticString<T, Size> tmp(*this);
				tmp.append(value.c_str());
				return tmp;
			}

			template <typename FT>
			bool operator==(const FT& value) const { return (compare(value) == 0); }
			template <typename FT>
			bool operator<(const FT& value) const { return (compare(value) < 0); }
			template <typename FT>
			bool operator<=(const FT& value) const { return (compare(value) <= 0); }
			template <typename FT>
			bool operator>=(const FT& value) const { return (compare(value) >= 0); }
			template <typename FT>
			bool operator>(const FT& value) const { return (compare(value) > 0); }
			template <typename FT>
			bool operator!=(const FT& value) const { return (compare(value) != 0); }

			bool operator==(const T* value) const { return (compare(value) == 0); }
			bool operator!=(const T* value) const { return (compare(value) != 0); }
			bool operator<(const T* value) const { return (compare(value) < 0); }
			bool operator<=(const T* value) const { return (compare(value) <= 0); }
			bool operator>=(const T* value) const { return (compare(value) >= 0); }
			bool operator>(const T* value) const { return (compare(value) > 0); }

		private:
			int compare(const char* value) const
			{
				return strcmp(buffer, value);
			}

			int compare(const wchar_t* value) const
			{
				return wcscmp(buffer, value);
			}

			template <typename FT>
			int compare(const FT& value) const
			{
				return compare(value.c_str());
			}

			void append(const char* value)
			{
				assert(getSize(value) < MaxSize - len);
				strncat(buffer, value, MaxSize - size());
				len = getSize(buffer);
			}

			void append(const wchar_t* value)
			{
				assert(getSize(value) < MaxSize - len);
				//wcsncat(buffer, value, MaxSize - size());
				wcsncpy_s(buffer, value, MaxSize - size());
				len = getSize(buffer);
			}

			size_t len;
			T buffer[Size + 1];

			size_t getSize(const wchar_t* buffer) const
			{
				return wcslen(buffer);
			}

			size_t getSize(const char* buffer) const
			{
				return strlen(buffer);
			}

			void set(const wchar_t* value)
			{
				//wcsncpy(buffer, value, Size - 1);
				wcsncpy_s(buffer, value, Size - 1);
				buffer[Size - 1] = 0;
				len = getSize(buffer);
			}

			void set(const char* value)
			{
				strncpy(buffer, value, Size - 1);
				buffer[Size - 1] = 0;
				len = getSize(buffer);
			}

			void set(const wchar_t* value, int len)
			{
				if (len >= Size)
					len = Size - 1;
				//wcsncpy(buffer, value, len);
				wcsncpy_s(buffer, value, len);
				buffer[len] = 0;
				this->len = len;
			}

			void set(const char* value, int len)
			{
				if (len >= Size)
					len = Size - 1;
				strncpy(buffer, value, len);
				buffer[len] = 0;
				this->len = len;
			}
		};

	} // namespace StringUtil

	//we declare this part of AllM for convenience

	template <int Size = 512>
	class static_wstring : public StringUtil::StaticString<wchar_t, Size>
	{
	public:
		template <int FS>
		static_wstring<Size>(const StringUtil::StaticString<wchar_t, FS>& value) : StringUtil::StaticString<wchar_t, Size>(value) {}

		static_wstring<Size>() {}
		static_wstring<Size>(const wchar_t* value) : StringUtil::StaticString<wchar_t, Size>(value) {}
		static_wstring<Size>(const wchar_t* begin, const wchar_t* end) : StringUtil::StaticString<wchar_t, Size>(begin, end) {}
		static_wstring<Size>(const std::wstring& value) : StringUtil::StaticString<wchar_t, Size>(value) {}
	};

	template <int Size = 512>
	class static_string : public StringUtil::StaticString<char, Size>
	{
	public:
		template <int FS>
		static_string<Size>(const StringUtil::StaticString<char, FS>& value) : StringUtil::StaticString<char, Size>(value) {}

		static_string<Size>() {}
		static_string<Size>(const char* value) : StringUtil::StaticString<char, Size>(value) {}
		static_string<Size>(const char* begin, const char* end) : StringUtil::StaticString<char, Size>(begin, end) {}
		static_string<Size>(const std::string& value) : StringUtil::StaticString<char, Size>(value) {}
	};
}

#endif // !StaticString_GUARD
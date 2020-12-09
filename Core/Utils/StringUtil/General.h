#ifndef StringUtil_General_GUARD
#define StringUtil_General_GUARD

#include <string>

namespace Lunia {
	namespace StringUtil {

		inline std::pair<const wchar_t*, const wchar_t*> Split(const wchar_t* ibegin, const wchar_t* iend, wchar_t delimiter) {
			for (const wchar_t* i = ibegin; i != iend; ++i) {
				if ((*i) == delimiter) {
					if (i == ibegin) {
						++ibegin;
						continue;
					}
					return std::pair<const wchar_t*, const wchar_t*>(ibegin, i);
				}
			}
			return std::pair<const wchar_t*, const wchar_t*>(ibegin, iend);
		}

		template <typename fc> inline std::pair<fc, fc> Split(fc ibegin, const fc& iend, typename fc::value_type delimiter) {

			for (fc i = ibegin; i != iend; ++i) {
				if ((*i) == delimiter) {
					if (i == ibegin) {
						++ibegin;
						continue;
					}
					return std::pair<fc, fc>(ibegin, i);
				}
			}
			return std::pair<fc, fc>(ibegin, iend);
		}

		template <typename iterator, typename container> void
			Split(iterator ibegin, const iterator& iend, typename iterator::value_type delimiter,
				container& result)
		{
			result.clear();

			typename container::value_type buffer;

			for (iterator i = ibegin; i != iend; ++i)
			{
				if ((*i) == delimiter)
				{
					result.push_back(buffer);
					buffer.clear();
				}
				else
				{
					buffer += (*i);
				}
			}
			result.push_back(buffer);
		}

		inline void LeftTrim(std::string& s) { s.erase(0, s.find_first_not_of(" \r\n\t")); }
		inline void LeftTrim(std::wstring& s) { s.erase(0, s.find_first_not_of(L" \r\n\t")); }

		inline void RightTrim(std::string& s) { s.resize(s.find_last_not_of(" \r\n\t") + 1); }
		inline void RightTrim(std::wstring& s) { s.resize(s.find_last_not_of(L" \r\n\t") + 1); }

		template <typename fc > inline void Trim(fc& s)
		{
			LeftTrim(s);
			RightTrim(s);
		}

		unsigned int Hash(const wchar_t* str);

		std::string Format(char* format, ...);
		std::wstring Format(wchar_t* format, ...);

		std::string Tokenize(const char* str, const char* token, int& pos);


		template <typename fc> inline fc ParseFileNameNoPath(const fc& str) {
			fc tmp = ParseFileName(str);

			typename fc::size_type pos = tmp.find_last_of(L"\\/:");

			if (pos != std::wstring::npos)
				return tmp.substr(pos + 1);
			return tmp;
		}


		/**
		Extract filename,  "filename|itemname" -> filename

		file name includes path

		 "c:\allm\data\bla|something" -> "c:\allm\data\bla"

		*/
		template <typename fc> inline fc ParseFileName(const fc& str) {
			std::pair<typename fc::const_iterator, typename fc::const_iterator> pair = Split(str.begin(), str.end(), L'|');
			return fc(pair.first, pair.second);
		}

		/**
		extracts the item name "filename|itenmae" -> itenname

		"c:\data\blabla.xml\hellsemething" -> "something"

		*/

		template <typename fc> inline fc ParseItemName(const fc& str) {
			std::pair<typename fc::const_iterator, typename fc::const_iterator> pair = Split(str.begin(), str.end(), L'|');
			if (pair.second == str.end()) return fc();
			return fc(++pair.second, str.end());
		}


		/**
		"c:\data\blabla.xml\hellsemething" -> "c:\data"

		*/

		inline std::wstring ParseFilePath(const std::wstring& str) {
			std::wstring::size_type pos = str.find_last_of(L"\\/:");
			if (pos != std::wstring::npos)
				return str.substr(0, pos);
			return str;
		}

		std::wstring RemoveDots(const std::wstring& str2);


		/**
			use like this.
			Replace(L"bla bla ~",L"bla",L"hello");
			"bla bla ~" -> "hello hello ~"
		*/
		std::wstring Replace(const std::wstring& string, const std::wstring& findString, const std::wstring& replaceString);
		std::string  Replace(const std::string& string, const std::string& findString, const std::string& replaceString);

		template<typename fc> size_t Find(const fc& string, const fc& findString) {
			size_t offset = findString.size();
			size_t lastFindPos = string.find_first_of(findString);

			if (lastFindPos == string.npos)
				return string.npos;

			for (size_t i = lastFindPos; i < string.size(); ++i)
			{
				fc compareString(&string[i], offset);

				if (compareString == findString)
					return i;
			}

			return string.npos;
		}

		/**
			If there is a FileLocation as L"\Database\Something\Bla"
			Replace( theLocation , L"\\", L/" )
			theLocation => L"/Database/Something/Bla"
		*/
		void Replace(std::wstring& string, wchar_t findChar, wchar_t replaceChar);

		/** filename struct to parse
			FullPath : "Drive:/rootdir/subdir/Filename.Extension"

			if Filename struct is created without any path information, path gonna be current directory
		*/
		struct Filename
		{
			std::wstring FileName;
			std::wstring Extension; // starts with '.' character
			std::wstring Drive; // ends with ':' character
			std::wstring Path; // starts and ends with '/' or '\\' character

			std::wstring GetFullPath() { return Path + FileName + Extension; }
			std::wstring GetFullName() { return FileName + Extension; }

			Filename();
			Filename(const Filename&);
			Filename(const std::wstring&);
			void Parse(const std::wstring&);
		};


	}
}

#endif
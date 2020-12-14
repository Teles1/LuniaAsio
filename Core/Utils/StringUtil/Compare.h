#ifndef LUNIA_STRINGUTIL_COMPARE_H
#define LUNIA_STRINGUTIL_COMPARE_H

namespace Lunia {
	namespace StringUtil {
		template <typename fc> inline bool Compare(fc i1, const fc& iend1, fc i2, const fc& iend2) {
			for (; (i1 != iend1) && (i2 != iend2); ++i1, ++i2) {
				if ((*i1) != (*i2)) return false;
			}
			if ((i1 == iend1) && (i2 == iend2)) return true;
			return false;
		}

		template <typename fc> inline bool Compare(std::pair<fc, fc>& a, std::pair<fc, fc>& b) {
			Compare(a.first, a.second, b.first, b.second);
		}


		inline bool Compare(std::string::const_iterator i1, const std::string::const_iterator& iend1, const char* i2) {
			for (; (i1 != iend1) && (*i2 != 0); ++i1, ++i2) {
				if ((*i1) != (*i2)) return false;
			}
			if ((i1 == iend1) && (*i2 == 0)) return true;
			return false;
		}

		inline bool Compare(std::wstring::const_iterator i1, const std::wstring::const_iterator& iend1, const wchar_t* i2) {
			for (; (i1 != iend1) && (*i2 != 0); ++i1, ++i2) {
				if ((*i1) != (*i2)) return false;
			}
			if ((i1 == iend1) && (*i2 == 0)) return true;
			return false;
		}

		inline bool Compare(const std::pair<std::string::const_iterator, std::string::const_iterator>& a, const char* b) {
			return Compare(a.first, a.second, b);
		}

		inline bool EqualsCaseInsensitive(const char* a, const char* b) {
			return (_stricmp(a, b) == 0);
		}

		inline bool EqualsCaseInsensitive(const wchar_t* a, const wchar_t* b) {
			return (_wcsicmp(a, b) == 0);
		}

		inline bool EqualsCaseInsensitive(const std::string& a, const std::string& b) {
			return EqualsCaseInsensitive(a.c_str(), b.c_str());
		}

		inline bool EqualsCaseInsensitive(const std::wstring& a, const std::wstring& b) {
			return EqualsCaseInsensitive(a.c_str(), b.c_str());
		}

	}
}

#endif
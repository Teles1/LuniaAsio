/* internal helper header - not exposed */
#pragma once

#include "base.h"
#include <windows.h>
#include <string>

namespace Lunia {

	std::string UnicodeToASCII(const wchar_t* str);
	std::wstring ASCIIToUnicode(const char* str);


	std::string  IntToASCII(int value);
	std::wstring IntToUnicode(int value);

	std::string  FloatToASCII(double value);
	std::wstring FloatToUnicode(double value);


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

	template <typename tc> class Splitter {

		tc& line;
		typename tc::value_type delimiter;
		void operator=(const Splitter<tc>&) {}

	public:

		Splitter<tc>(tc& pLine, typename tc::value_type pDelimiter) : line(pLine), delimiter(pDelimiter) {}


		struct iterator {
			void operator=(const iterator& i) {
				iters = i.iters;
			}

			std::pair< typename tc::iterator, typename tc::iterator > iters;
			Splitter& outer;


			iterator(Splitter& pOuter, typename tc::iterator a, typename tc::iterator b) :outer(pOuter), iters(a, b) {}

			std::pair<typename tc::iterator, typename tc::iterator> operator*() {
				return iters;
			}
			bool operator==(const iterator& iter) const {
				return ((iters.second == iter.second) && (iters.first == iter.first));
			}

			bool operator!=(const iterator& iter) const {
				return (((iters.second != iter.iters.second) || (iters.first != iter.iters.first)));
			}

			void operator++() {
				iters = Split(iters.second, outer.line.end(), outer.delimiter);
			}
		};

		iterator end() {
			return iterator(*this, line.end(), line.end());
		}

		iterator begin() {
			iterator i = iterator(*this, line.begin(), line.begin());
			++i;
			return i;
		}

	};



	template <typename fc> inline bool Compare(std::pair<fc, fc>& a, std::pair<fc, fc>& b) {
		Compare(a.first, a.second, b.first, b.second);
	}

	template <typename fc> inline bool Compare(fc i1, const fc& iend1, fc i2, const fc& iend2) {
		for (; (i1 != iend1) && (i2 != iend2); ++i1, ++i2) {
			if ((*i1) != (*i2)) return false;
		}
		if ((i1 == iend1) && (i2 == iend2)) return true;
		return false;
	}



	//i would like to make these into a one function and templated, but couldnt get it compiled 
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




	/*inline bool Compare(const stringIteratorPair& a, const char* b) {
	   return Compare(a.first,a.second,b);
	}*/

	inline float Random(float value) {
		return (rand() / float(RAND_MAX)) * value;
	}



}


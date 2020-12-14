#pragma once

#include <string>
#include <stdexcept>
#include <stack>
#include <windows.h>

#define SAFE_DELETE(x) if (x) { delete x; x=NULL; }

#include "CriticalSection.h"

namespace Lunia {
	/* collections(list and map) which contain critical section handlers for thread-safty */
	template< class _Ty, class _Ax = std::allocator<_Ty> > class List : public std::list<_Ty, _Ax>, public CriticalSection {};
	template< class _Kty, class _Ty, class _Tr = std::hash_compare<_Kty, _STD less<_Kty> >
		, class _Alloc = _STD allocator< _STD pair<const _Kty, _Ty> > > class Map
			: public std::unordered_map<_Kty, _Ty, _Tr, _Alloc>, public CriticalSection {};

		template <typename fc> inline void SafeDelete(fc& bla) {
			delete bla;
			bla = 0;
		}
}

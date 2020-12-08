#ifndef Common_HEADER_GUARD
#define Common_HEADER_GUARD
#pragma once
#include <thread>
#include <optional>
#include <sstream> // added 
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <map>

#pragma region Type Definition

typedef unsigned long		uint64;
typedef uint32_t			uint32;
typedef uint16_t			uint16;
typedef unsigned char		uint8;

typedef signed long			int64;
typedef int					int32;
typedef short int			int16;
typedef char				int8;

typedef std::wstring		String;
typedef uint16				HashType;
typedef unsigned short		LengthType;
#pragma endregion

#pragma region  Utils Includes

#include "Utils/StringUtil.hpp"
#include "Exceptions/Expcetion.h"
#include "Utils/Logger.h"

#pragma endregion

#pragma region Global Constants

#ifndef MAX_BUFFER_SIZE 
	#define MAX_BUFFER_SIZE 4096
#endif

namespace Constants {
	const uint32 Version = 0x17;
	const HashType NetStreamHash = StringUtil::Hash(L"NetStream");
	const uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
}

#pragma endregion

#endif // !Common_HEADER_GUARD
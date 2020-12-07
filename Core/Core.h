#pragma once
#ifndef Common_HEADER_GUARD
#define Common_HEADER_GUARD
#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <sstream> // added 
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <set>
#include <utility>
#include <map>
#include "boost/format.hpp"

#pragma region Type Definition

typedef unsigned long		uint64;
typedef uint32_t			uint32;
typedef uint16_t			uint16;
typedef unsigned char		uint8;

typedef signed long			int64;
typedef int					int32;
typedef short int			int16;
typedef char				int8;

#pragma endregion

typedef std::wstring		String;
typedef uint16				HashType;
typedef unsigned short		LengthType;

#pragma region Global Constants

#ifndef MAX_BUFFER_SIZE
	#define MAX_BUFFER_SIZE 4096
#endif

#pragma region  Utils Includes

#include "./Utils/StringUtil.hpp"
#include "./Exceptions/Expcetion.h"

#pragma endregion
namespace Constants {
	const uint32 Version = 0x17;
	const HashType NetStreamHash = StringUtil::Hash(L"NetStream");
	const uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
}
#pragma endregion

/*
	LOG RELATED BEGIN
*/
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#ifndef INFO_LOG 
#define INFO_LOG(...) spdlog::info(fmt::format(__VA_ARGS__));
#endif
#ifndef ERROR_LOG 
#define ERROR_LOG(...) spdlog::error(fmt::format(__VA_ARGS__)); throw Exception("Look at the goddman console.");
#endif
#ifndef WARN_LOG 
#define WARN_LOG(...) spdlog::warn(fmt::format(__VA_ARGS__));
#endif
#ifndef CRITICAL_LOG 
#define CRITICAL_LOG(...) spdlog::critical(fmt::format(__VA_ARGS__));
#endif
#ifndef DEBUG_LOG 
#define DEBUG_LOG(...) spdlog::debug(fmt::format(__VA_ARGS__));
#endif
/*
	LOG STUFF END
*/
#pragma endregion
#endif // !Common_HEADER_GUARD
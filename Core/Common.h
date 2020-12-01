#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <sstream> // added 
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <set>
#include <utility>
#include "boost/format.hpp"

#pragma region Type Definition

typedef std::wstring		String;

typedef unsigned long		uint64;
typedef uint32_t			uint32;
typedef uint16_t			uint16;
typedef unsigned char		uint8;

typedef signed long			int64;
typedef int					int32;
typedef short int			int16;
typedef char				int8;

#pragma endregion
#pragma region Global Constants

#ifndef MAX_BUFFER_SIZE
	#define MAX_BUFFER_SIZE 4096
#endif

#pragma endregion

#pragma region  Utils Includes

#include "./Utils/Log.h"
#include "./Utils/StringUtil.h"
#include "./Exceptions/Expcetion.h"

#pragma endregion

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include "./Utils/Log.h"
#include "./Exceptions/Expcetion.h"
#include "boost/format.hpp"

#pragma region Type Definition

typedef unsigned long uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef unsigned char uint8;

typedef signed long int64;
typedef int int32;
typedef short int int16;
typedef char int8;

#pragma endregion
#pragma region Global Constants

#ifndef MAX_BUFFER_SIZE
	#define MAX_BUFFER_SIZE 4096
#endif

#pragma endregion

#include "./Utils/StringUtil.h"
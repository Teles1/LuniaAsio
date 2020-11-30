#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <set>
#include <utility>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


typedef std::wstring       String;

/* basic value types */
typedef signed char        int8;
typedef short int          int16;
typedef int                int32;
typedef __int64            int64;

typedef unsigned char      uint8;
typedef unsigned short int uint16;
typedef unsigned int       uint32;
typedef unsigned __int64   uint64;
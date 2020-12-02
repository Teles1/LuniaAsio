#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


#ifndef INFO_LOG 
	#define INFO_LOG(x,...) spdlog::info(fmt::format(x, __VA_ARGS__));
#endif
#ifndef ERROR_LOG 
	#define ERROR_LOG(x,...) spdlog::error(fmt::format(x, __VA_ARGS__));
#endif
#ifndef WARN_LOG 
	#define WARN_LOG(x,...) spdlog::warn(fmt::format(x, __VA_ARGS__));
#endif
#ifndef CRITICAL_LOG 
	#define CRITICAL_LOG(x,...) spdlog::critical(fmt::format(x, __VA_ARGS__));
#endif
#ifndef DEBUG_LOG 
	#define DEBUG_LOG(x,...) spdlog::debug(fmt::format(x, __VA_ARGS__));
#endif
/*
	Core Logging method.
*/
#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
auto coreConsole = spdlog::stdout_color_mt("Core");

#define INFO(x,...) spdlog::get("Core")->info(fmt::format(x, __VA_ARGS__));
#define ERROR(x,...) spdlog::get("Core")->error(fmt::format(x, __VA_ARGS__));
#define WARN(x,...) spdlog::get("Core")->warn(fmt::format(x, __VA_ARGS__));
#define CRITICAL(x,...) spdlog::get("Core")->critical(fmt::format(x, __VA_ARGS__));
#define DEBUG(x,...) spdlog::get("Core")->debug(fmt::format(x, __VA_ARGS__));
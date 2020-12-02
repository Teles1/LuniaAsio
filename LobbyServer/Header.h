#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define INFO(x,...) spdlog::get("LobbyServer")->info(fmt::format(x, __VA_ARGS__));
#define ERROR(x,...) spdlog::get("LobbyServer")->error(fmt::format(x, __VA_ARGS__));
#define WARN(x,...) spdlog::get("LobbyServer")->warn(fmt::format(x, __VA_ARGS__));
#define CRITICAL(x,...) spdlog::get("LobbyServer")->critical(fmt::format(x, __VA_ARGS__));
#define DEBUG(x,...) spdlog::get("LobbyServer")->debug(fmt::format(x, __VA_ARGS__));
    

#ifndef Logger_GUARD
#define Logger_GUARD
#pragma once

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

struct Logger{
public:
	template <typename... Args>
	inline void Info(const std::string& message, const Args &... args) {
		m_logger->info(fmt::format(message, args...));
	}
	template <typename... Args>
	inline void Warn(const std::string& message, const Args &... args) {
		m_logger->warn(fmt::format(message, args...));
	}
	template <typename... Args>
	inline void Debug(const std::string& message, const Args &... args) {
		m_logger->debug(fmt::format(message, args...));
	}
	template <typename... Args>
	inline void Error(const std::string& message, const Args &... args) {
		m_logger->error(fmt::format(message, args...));
	}
	template <typename... Args>
	inline void Exception(const std::string& message, const Args &... args) {
		m_logger->critical(fmt::format(message, args...));
		throw fmt::format(message, args...);
	}
private:
	inline Logger(const std::string& name) {
		m_logger = spdlog::stdout_color_mt(name);
		m_logger->set_pattern("[%X %^%l%$] => %v");
	}
	std::shared_ptr<spdlog::logger>					m_logger; //spd_logger sharedpointer
#pragma region Sigleton
public:
	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;
	Logger(const Logger&) = delete;
	Logger& operator= (const Logger&) = delete;
	~Logger() {}
	inline static Logger& GetInstance(const std::string& name = "DefaultName") {
		static Logger m_instance(name);
		return m_instance;
	}
#pragma endregion
};
#endif // !Logger_GUARD



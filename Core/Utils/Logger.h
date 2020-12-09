#ifndef Logger_GUARD
#define Logger_GUARD
#pragma once

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>
#include <mutex>
#include "../Exceptions/Expcetion.h"

struct Logger{
public:
	template <typename... Args>
	void Info(const std::string& message, const Args &... args) {
		m_logger->info(fmt::format(message, args...));
	}
	template <typename... Args>
	void Warn(const std::string& message, const Args &... args) {
		m_logger->warn(fmt::format(message, args...));
	}
	template <typename... Args>
	void Debug(const std::string& message, const Args &... args) {
		m_logger->debug(fmt::format(message, args...));
	}
	template <typename... Args>
	void Error(const std::string& message, const Args &... args) {
		m_logger->error(fmt::format(message, args...));
	}
	template <typename... Args>
	void Exception(const std::string& message, const Args &... args) {
		m_logger->critical(fmt::format(message, args...));
		throw fmt::format(message, args...);
	}
private:
	Logger(const std::string& name); // Private because I want only members to be able to construct which is GetInstance
	std::shared_ptr<spdlog::logger>					m_logger; 
#pragma region Sigleton
public:
	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;
	Logger(const Logger&) = delete;
	Logger& operator= (const Logger&) = delete;
	~Logger() {}
	static std::shared_ptr<Logger>& GetInstance(const std::string& name = "DefaultName");
private:
	static std::mutex								m_lock;
	static std::shared_ptr<Logger>					m_instance;
#pragma endregion
};
typedef std::shared_ptr<Logger> LoggerPtr;
#endif // !Logger_GUARD



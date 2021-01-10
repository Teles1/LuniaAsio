#ifndef Logger_GUARD
#define Logger_GUARD
#pragma once

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_LEVEL_NAMES { "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "OFF" }
#include <spdlog/spdlog.h>
namespace Lunia {
	struct Logger {
	public:
		/*
			Error
		*/
		template<typename... Args>
		inline void Error(const std::wstring& message, const Args &... args) {
			m_logger->error(message, fmt::make_format_args<fmt::wformat_context>(args...));
		}

		template<typename... Args>
		inline void Error(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			m_logger->error(aux);
		}

		template<typename... Args>
		inline void Error(const std::string& message, const Args &... args) {
			m_logger->error(message, fmt::make_format_args<fmt::format_context>(args...));
		}

		template<typename... Args>
		inline void Error(const char* message, const Args &... args) {
			m_logger->error(message, args...);
		}
		/*
			Info
		*/
		template<typename... Args>
		inline void Info(const std::wstring& message, const Args &... args) {
			m_logger->info(message, fmt::make_format_args<fmt::wformat_context>(args...));
		}

		template<typename... Args>
		inline void Info(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			m_logger->info(aux);
		}

		template<typename... Args>
		inline void Info(const std::string& message, const Args &... args) {
			m_logger->info(message, fmt::make_format_args<fmt::format_context>(args...));
		}

		template<typename... Args>
		inline void Info(const char* message, const Args &... args) {
			m_logger->info(message, args...);
		}
		/*
			Warn
		*/
		template<typename... Args>
		inline void Warn(const std::wstring& message, const Args &... args) {
			m_logger->warn(message, fmt::make_format_args<fmt::wformat_context>(args...));
		}

		template<typename... Args>
		inline void Warn(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			m_logger->warn(aux);
		}

		template<typename... Args>
		inline void Warn(const std::string& message, const Args &... args) {
			m_logger->warn(message, fmt::make_format_args<fmt::format_context>(args...));
		}

		template<typename... Args>
		inline void Warn(const char* message, const Args &... args) {
			m_logger->warn(message, args...);
		}

		/*
			Debug
		*/
		template<typename... Args>
		inline void Debug(const std::wstring& message, const Args &... args) {
			m_logger->debug(message, fmt::make_format_args<fmt::wformat_context>(args...));
		}

		template<typename... Args>
		inline void Debug(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			m_logger->debug(aux);
		}

		template<typename... Args>
		inline void Debug(const std::string& message, const Args &... args) {
			m_logger->debug(message, fmt::make_format_args<fmt::format_context>(args...));
		}

		template<typename... Args>
		inline void Debug(const char* message, const Args &... args) {
			m_logger->debug(message, args...);
		}
		/*
			Exception
		*/
		template<typename... Args>
		inline void Exception(const std::wstring& message, const Args &... args) {
			m_logger->critical(message, fmt::make_format_args<fmt::wformat_context>(args...));
			throw message.c_str();
		}

		template<typename... Args>
		inline void Exception(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			m_logger->critical(aux);
			throw aux.c_str();
		}

		template<typename... Args>
		inline void Exception(const std::string& message, const Args &... args) {
			m_logger->critical(message, fmt::make_format_args<fmt::format_context>(args...));
			std::string tmpMessage(fmt::format(message, args...));
			throw tmpMessage.c_str();
		}

		template<typename... Args>
		inline void Exception(const char* message, const Args &... args) {
			m_logger->critical(message, args...);
			std::string tmpMessage(fmt::format(message, args...));
			throw tmpMessage.c_str();
		}

	private:
		Logger(const std::string& name);
		std::shared_ptr<spdlog::logger> m_logger; //spd_logger sharedpointer
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
	inline Logger& LoggerInstance() {
		return Logger::GetInstance();
	}
}

#endif // !Logger_GUARD
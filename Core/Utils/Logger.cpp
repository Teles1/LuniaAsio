#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
namespace Lunia {
	Logger::Logger(const std::string& name)
	{
		std::vector<spdlog::sink_ptr> sinks;
		{
			sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			auto& sink = sinks.back();
			sink->set_pattern("[%X %^%l%$] => %v");
			sink->set_level(spdlog::level::level_enum::err);
		}
		{
			sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.log", true));
			auto& sink = sinks.back();
			sink->set_pattern("[%X %^%l%$] => %v");
			sink->set_level(spdlog::level::level_enum::trace);
		}
		m_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
		spdlog::register_logger(m_logger);
		spdlog::flush_every(std::chrono::seconds(5));
	}
}


#include "Logger.h"

#pragma region Singleton
/*
	Definitions os the static methods/variables MUST be in the cpp.
	I went for shared_ptr in order to avoid deletion of memory manually.
	Let the compiler does it's thing.
*/

LoggerPtr Logger::m_instance;
std::mutex Logger::m_lock;

LoggerPtr& Logger::GetInstance(const std::string& name) {
	LoggerPtr pTmp = m_instance;
	if (pTmp == NULL)
	{
		m_lock.lock(); // thread safe
		if (pTmp == NULL)
			m_instance = LoggerPtr(new Logger(name));
		m_lock.unlock();
	}
	return m_instance;
}
/*
	End of static definitions.
*/
#pragma endregion
Logger::Logger(const std::string& name) {
	m_logger = spdlog::stdout_color_mt(name);
	m_logger->set_pattern("[%X %^%l%$] => %v");
}

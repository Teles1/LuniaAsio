#include "./ThreadPool.h"
#include <thread>

namespace Lunia {
	namespace Utils {
		ThreadPoolPtr thread_pool::m_instance;
		std::mutex thread_pool::_mutex;

		ThreadPoolPtr& thread_pool::GetInstance()
		{
			ThreadPoolPtr pTmp = m_instance;
			if (pTmp == NULL) {
				_mutex.lock();
				if (pTmp == NULL) {
					int nThreads = std::thread::hardware_concurrency();
					m_instance = std::shared_ptr<thread_pool>(new thread_pool(nThreads));
				}
				_mutex.unlock();
			}
				
			return m_instance;
		}
	}
}



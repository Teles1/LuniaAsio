#include "fwPacketListener.h"
#include "UserRegistry.h"

std::shared_ptr<fwPacketListener> fwPacketListener::m_instance;
std::mutex fwPacketListener::m_lock;

std::shared_ptr<fwPacketListener>& fwPacketListener::GetInstance() {
	std::shared_ptr<fwPacketListener> pTmp = m_instance;
	if (pTmp == NULL)
	{
		m_lock.lock(); // thread safe
		if (pTmp == NULL)
			m_instance = std::shared_ptr<fwPacketListener>(new fwPacketListener());
		m_lock.unlock();
	}
	return m_instance;
}
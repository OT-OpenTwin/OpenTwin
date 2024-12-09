// Service header
#include "MainLockManager.h"

MainLockManager& MainLockManager::instance(void) {
	static MainLockManager g_instance;
	return g_instance;
}

std::mutex& MainLockManager::getMutex(void) {
	return MainLockManager::instance().m_mutex;
}

// @otlicense

// AK header
#include <akCore/aUidMangager.h>
#include <akCore/aException.h>

ak::aUidManager::aUidManager() : m_currentId(1) {}

ak::aUidManager::~aUidManager() {}

// ###########################################################

ak::UID ak::aUidManager::getId(void) {
	m_mutex.lock();
	ak::UID id = m_currentId++;
	if (id == 0xffffffffffffffff) {
		assert(0);
		m_mutex.unlock();
		throw aException("Max UID reached!", "aUidManager::getId()", aException::ArrayOutOfBounds);
	}
	m_mutex.unlock();
	return id;
}

void ak::aUidManager::reset(void) {
	m_mutex.lock();
	m_currentId = 1;
	m_mutex.unlock();
}

void ak::aUidManager::setLatestUid(ak::UID _uid) {
	m_mutex.lock();
	m_currentId = _uid;
	m_mutex.unlock();
}
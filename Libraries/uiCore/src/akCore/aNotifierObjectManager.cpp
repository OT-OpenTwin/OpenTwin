// @otlicense

// AK header
#include <akCore/aException.h>
#include <akCore/aNotifierObjectManager.h>
#include <akGui/aObjectManager.h>

ak::aNotifierObjectManager::aNotifierObjectManager(
	aObjectManager *					_manager
) {
	try {
		if (_manager == nullptr) { throw aException("Is nullptr", "Check manager", aException::exceptionType::Nullptr); }
		m_manager = _manager;
	}
	catch (const aException & e) { throw aException(e, "aNotifierObjectManager::aNotifierObjectManager()"); }
	catch (const std::exception & e) { throw aException(e.what(), "aNotifierObjectManager::aNotifierObjectManager()"); }
	catch (...) { throw aException("Unknown error", "aNotifierObjectManager::aNotifierObjectManager()"); }
}

ak::aNotifierObjectManager::~aNotifierObjectManager(void) {}

// *****************************************************************************************

// Message IO

void ak::aNotifierObjectManager::notify(
	UID							_senderId,
	eventType					_event,
	int							_info1,
	int							_info2
) {
	try {
		if (_event == etDestroyed) {
			assert(isEnabled()); // Message received even if notifier was disabled
			m_manager->creatorDestroyed(_senderId);
		}
	}
	catch (const aException & e) { throw aException(e, "ak::notifierObjectManager::notify()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::notifierObjectManager::notify()"); }
	catch (...) { throw aException("Unknown error", "ak::notifierObjectManager::notify()"); }
}

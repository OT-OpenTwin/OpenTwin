//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/API.h>
#include <aci/InterpreterCore.h>
#include <aci/OS.h>
#include <aci/ScriptLoader.h>
#include <cassert>

bool initialized{ false };

bool aci::API::initialize(AbstractPrinter * _printer, AbstractInterpreterNotifier * _notifier, AbstractOSHandler * _osHandler) {
	if (initialized) {
		assert(0);		// API already initialized
		return false;
	}
	if (_printer == nullptr) {
		assert(0);		// No valid printer provided
		return false;
	}
	if (_notifier == nullptr) {
		assert(0);		// No valid notifier provided
		return false;
	}

	OS::instance()->attachOSHandler(_osHandler);
	InterpreterCore::instance()->attachNotifier(_notifier);
	InterpreterCore::instance()->attachPrinter(_printer);
	initialized = true;
	return true;
}

void aci::API::cleanUp(void) noexcept {
	if (!initialized) {
		assert(0);		// API was not initialized before
		return;
	}
	InterpreterCore::instance()->scriptLoader()->unloadScripts();
	OS::clearInstance();
	InterpreterCore::clearInstance();
	initialized = false;
}

aci::InterpreterCore * aci::API::core(void) {
	if (!initialized) {
		assert(0);		// API not initialized
		return nullptr;
	}
	return InterpreterCore::instance();
}

std::wstring aci::API::currentPath(void) {
	if (!initialized) {
		assert(0);		// API not initialized
		return std::wstring();
	}
	return InterpreterCore::instance()->currentPath();
}

void aci::API::setCurrentPath(const std::wstring& _path) {
	if (!initialized) {
		assert(0);		// API not initialized
		return;
	}
	InterpreterCore::instance()->setCurrentPath(_path);
}

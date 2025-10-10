//! @file WindowAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"

// Public API

void ot::WindowAPI::lockUI(bool _flag) {
	WindowAPI::instance()->lockUIAPI(_flag);
}

void ot::WindowAPI::lockSelectionAndModification(bool _flag) {
	WindowAPI::instance()->lockSelectionAndModificationAPI(_flag);
}

void ot::WindowAPI::setProgressBarVisibility(QString _progressMessage, bool _progressBaseVisible, bool _continuous) {
	WindowAPI::instance()->setProgressBarVisibilityAPI(_progressMessage, _progressBaseVisible, _continuous);
}

void ot::WindowAPI::setProgressBarValue(int _progressPercentage) {
	WindowAPI::instance()->setProgressBarValueAPI(_progressPercentage);
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::WindowAPI::WindowAPI() {
	if (getInstanceReference()) {
		OT_LOG_EA("WindowAPI instance already exists");
		throw Exception::ObjectAlreadyExists("WindowAPI instance already exists");
	}
	getInstanceReference() = this;
}

ot::WindowAPI::~WindowAPI() {
	OTAssert(getInstanceReference(), "WindowAPI instance does not exist");
	getInstanceReference() = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Instance management

ot::WindowAPI* ot::WindowAPI::instance() {
	ot::WindowAPI* instance = getInstanceReference();
	if (!instance) {
		throw Exception::ObjectNotFound("WindowAPI instance not available");
	}
	return instance;
}

ot::WindowAPI*& ot::WindowAPI::getInstanceReference() {
	static WindowAPI* g_instance = { nullptr };
	return g_instance;
}


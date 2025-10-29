// @otlicense

//! @file CurrentProjectAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/CurrentProjectAPI.h"

// Public API

void ot::CurrentProjectAPI::activateModelVersion(const std::string& _versionName) {
	CurrentProjectAPI::instance()->activateModelVersionAPI(_versionName);
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::CurrentProjectAPI::CurrentProjectAPI() {
	if (getInstanceReference()) {
		OT_LOG_EA("CurrentProjectAPI instance already exists");
		throw Exception::ObjectAlreadyExists("CurrentProjectAPI instance already exists");
	}
	getInstanceReference() = this;
}

ot::CurrentProjectAPI::~CurrentProjectAPI() {
	OTAssert(getInstanceReference(), "CurrentProjectAPI instance does not exist");
	getInstanceReference() = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Instance management

ot::CurrentProjectAPI* ot::CurrentProjectAPI::instance() {
	CurrentProjectAPI* instance = getInstanceReference();
	if (!instance) {
		OT_LOG_EA("CurrentProjectAPI instance not available");
		throw Exception::ObjectNotFound("CurrentProjectAPI instance not available");
	}
	return instance;
}

ot::CurrentProjectAPI*& ot::CurrentProjectAPI::getInstanceReference() {
	static CurrentProjectAPI* instance = nullptr;
	return instance;
}
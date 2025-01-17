//! @file CopyInformationFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/CopyInformationFactory.h"

ot::CopyInformationFactory& ot::CopyInformationFactory::instance(void) {
	static CopyInformationFactory g_instance;
	return g_instance;
}

ot::CopyInformation* ot::CopyInformationFactory::create(const ConstJsonObject& _object) {
	// Create default instance
	ot::CopyInformation* newInfo = CopyInformationFactory::instance().createFromKey(json::getString(_object, CopyInformation::getCopyTypeJsonKey()));

	if (newInfo) {
		// Ensure serialized version is current version
		if (newInfo->getCopyVersion() != json::getInt64(_object, CopyInformation::getCopyVersionJsonKey())) {
			OT_LOG_W("Copy information has unsupported version: \"" + std::to_string(json::getInt64(_object, CopyInformation::getCopyVersionJsonKey())) + "\"");
			delete newInfo;
		}
		else {
			newInfo->setFromJsonObject(_object);
		}
	}

	return newInfo;
}

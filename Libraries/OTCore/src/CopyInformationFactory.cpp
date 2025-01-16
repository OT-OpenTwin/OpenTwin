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
	return CopyInformationFactory::instance().createFromJSON(_object, CopyInformation::getCopyTypeJsonKey());
}

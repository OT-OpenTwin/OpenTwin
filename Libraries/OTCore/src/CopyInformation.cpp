//! @file CopyInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/CopyInformation.h"

std::string ot::CopyInformation::getCopyTypeJsonKey(void) {
	return "CopyType";
}

void ot::CopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(CopyInformation::getCopyTypeJsonKey(), _allocator), JsonString(this->getCopyType(), _allocator), _allocator);
}

void ot::CopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {

}
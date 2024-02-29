//! @file PropertyDirectory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyDirectory.h"

void ot::PropertyDirectory::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Path", JsonString(m_path, _allocator), _allocator);
}

void ot::PropertyDirectory::setPropertyData(const ot::ConstJsonObject& _object) {
	m_path = json::getString(_object, "Path");
}
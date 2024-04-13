//! @file PropertyDirectory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/PropertyDirectory.h"
#include "OTCore/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyDirectory> propertyDirectoryRegistrar(OT_PROPERTY_TYPE_Directory);

ot::PropertyDirectory::PropertyDirectory(const PropertyDirectory* _other) 
	: Property(_other), m_path(_other->m_path)
{}

ot::Property* ot::PropertyDirectory::createCopy(void) const {
	return new PropertyDirectory(this);
}

void ot::PropertyDirectory::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Path", JsonString(m_path, _allocator), _allocator);
}

void ot::PropertyDirectory::setPropertyData(const ot::ConstJsonObject& _object) {
	m_path = json::getString(_object, "Path");
}
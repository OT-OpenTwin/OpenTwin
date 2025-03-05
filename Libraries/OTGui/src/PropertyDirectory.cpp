//! @file PropertyDirectory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyDirectory.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyDirectory> propertyDirectoryRegistrar(ot::PropertyDirectory::propertyTypeString());

ot::PropertyDirectory::PropertyDirectory(const PropertyDirectory* _other) 
	: Property(_other), m_path(_other->m_path)
{}

ot::PropertyDirectory::PropertyDirectory(const PropertyBase& _base)
	: Property(_base)
{}

void ot::PropertyDirectory::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	if (_mergeMode & PropertyBase::MergeValues) {
		const PropertyDirectory* other = dynamic_cast<const PropertyDirectory*>(_other);
		OTAssertNullptr(other);
		m_path = other->m_path;
	}
}

ot::Property* ot::PropertyDirectory::createCopy(void) const {
	return new PropertyDirectory(this);
}

void ot::PropertyDirectory::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Path", JsonString(m_path, _allocator), _allocator);
}

void ot::PropertyDirectory::setPropertyData(const ot::ConstJsonObject& _object) {
	m_path = json::getString(_object, "Path");
}
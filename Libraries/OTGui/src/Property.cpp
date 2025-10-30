// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"

ot::Property::Property(const Property* _other)
	: PropertyBase(*_other), m_parentGroup(nullptr)
{}

ot::Property::Property(const PropertyBase& _base) 
	: PropertyBase(_base), m_parentGroup(nullptr)
{}

ot::Property::Property(PropertyFlags _flags)
	: PropertyBase(_flags), m_parentGroup(nullptr)
{}

ot::Property::Property(const std::string& _name, PropertyFlags _flags)
	: PropertyBase(_name, _flags), m_parentGroup(nullptr)
{}

void ot::Property::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	PropertyBase::addToJsonObject(_object, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Property_Type, JsonString(this->getPropertyType(), _allocator), _allocator);
	this->getPropertyData(_object, _allocator);
}

void ot::Property::setFromJsonObject(const ot::ConstJsonObject& _object) {
	PropertyBase::setFromJsonObject(_object);
	this->setPropertyData(_object); 
}

ot::Property::~Property() {
	if (m_parentGroup) {
		m_parentGroup->forgetProperty(this);
		delete m_parentGroup;
		m_parentGroup = nullptr;
	}
}

ot::Property* ot::Property::createCopyWithParents(void) const {
	Property* newProperty = this->createCopy();
	const PropertyGroup* oldParent = m_parentGroup;
	PropertyGroup* newParent = nullptr;
	while (oldParent) {
		PropertyGroup* tmp = oldParent->createCopy(false);
		if (newParent) {
			tmp->addChildGroup(newParent);
		}
		else {
			tmp->addProperty(newProperty);
		}
		newParent = tmp;
		oldParent = oldParent->getParentGroup();
	}

	return newProperty;
}

void ot::Property::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	PropertyBase::mergeWith(*_other, _mergeMode);
}

ot::PropertyGroup* ot::Property::getRootGroup(void) const {
	if (m_parentGroup) return m_parentGroup->getRootGroup();
	else return nullptr;
}

std::string ot::Property::getPropertyPath(char _delimiter) const {
	if (m_parentGroup) return m_parentGroup->getGroupPath() + _delimiter + this->getPropertyName();
	else return this->getPropertyName();
}
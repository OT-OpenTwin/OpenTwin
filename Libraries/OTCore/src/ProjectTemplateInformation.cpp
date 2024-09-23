//! @file ProjectTemplateInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/ProjectTemplateInformation.h"

ot::ProjectTemplateInformation::ProjectTemplateInformation() 
	: m_isDefault(false)
{

}

ot::ProjectTemplateInformation::ProjectTemplateInformation(const ProjectTemplateInformation& _other) {
	*this = _other;
}

ot::ProjectTemplateInformation::~ProjectTemplateInformation() {

}

ot::ProjectTemplateInformation& ot::ProjectTemplateInformation::operator = (const ProjectTemplateInformation& _other) {
	if (this == &_other) return *this;

	m_name = _other.m_name;
	m_description = _other.m_description;
	m_iconSubPath = _other.m_iconSubPath;
	m_isDefault = _other.m_isDefault;

	return *this;
}

void ot::ProjectTemplateInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	_object.AddMember("IconSubPath", JsonString(m_iconSubPath, _allocator), _allocator);
	_object.AddMember("IsDefault", m_isDefault, _allocator);
}

void ot::ProjectTemplateInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_description = json::getString(_object, "Description");
	m_iconSubPath = json::getString(_object, "IconSubPath");
	m_name = json::getBool(_object, "IsDefault");
}

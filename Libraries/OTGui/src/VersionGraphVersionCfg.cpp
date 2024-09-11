//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VersionGraphVersionCfg.h"

ot::VersionGraphVersionCfg::VersionGraphVersionCfg() 
	: m_directParentIsHidden(false)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const std::string& _name, const std::string& _label, const std::string& _description)
	: m_name(_name), m_label(_label), m_description(_description), m_directParentIsHidden(false)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const VersionGraphVersionCfg& _other) {
	*this = _other;
}

ot::VersionGraphVersionCfg::~VersionGraphVersionCfg() {
	this->clear();
}

ot::VersionGraphVersionCfg& ot::VersionGraphVersionCfg::operator = (const VersionGraphVersionCfg& _other) {
	if (this == &_other) return *this;

	this->clear();

	m_name = _other.m_name;
	m_label = _other.m_label;
	m_description = _other.m_description;
	m_childVersions = _other.m_childVersions;
	m_directParentIsHidden = _other.m_directParentIsHidden;

	return *this;
}

void ot::VersionGraphVersionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	_object.AddMember("ParentHidden", m_directParentIsHidden, _allocator);

	JsonArray childArr;
	for (const VersionGraphVersionCfg& child : m_childVersions) {
		JsonObject childObj;
		child.addToJsonObject(childObj, _allocator);
		childArr.PushBack(childObj, _allocator);
	}
	_object.AddMember("Childs", childArr, _allocator);
}

void ot::VersionGraphVersionCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();

	m_name = json::getString(_object, "Name");
	m_label = json::getString(_object, "Label");
	m_description = json::getString(_object, "Description");
	m_directParentIsHidden = json::getBool(_object, "ParentHidden");

	std::list<ConstJsonObject> childArr = json::getObjectList(_object, "Childs");
	for (const ConstJsonObject& childObj : childArr) {
		VersionGraphVersionCfg child;
		child.setFromJsonObject(childObj);
		m_childVersions.push_back(child);
	}
}

void ot::VersionGraphVersionCfg::addChildVersion(const std::string& _name, const std::string& _label, const std::string& _description) {
	this->addChildVersion(VersionGraphVersionCfg(_name, _label, _description));
}

void ot::VersionGraphVersionCfg::addChildVersion(const VersionGraphVersionCfg& _child) {
	m_childVersions.push_back(_child);
}

void ot::VersionGraphVersionCfg::applyConfigOnly(const VersionGraphVersionCfg& _other) {
	m_name = _other.m_name;
	m_label = _other.m_label;
	m_description = _other.m_description;
	m_directParentIsHidden = _other.m_directParentIsHidden;
}

void ot::VersionGraphVersionCfg::clear(void) {
	m_childVersions.clear();
}

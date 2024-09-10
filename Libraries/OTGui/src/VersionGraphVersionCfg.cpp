//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VersionGraphVersionCfg.h"

ot::VersionGraphVersionCfg::VersionGraphVersionCfg() {

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const std::string& _name, const std::string& _label, const std::string& _description)
	: m_name(_name), m_label(_label), m_description(_description)
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

	return *this;
}

void ot::VersionGraphVersionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);

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

void ot::VersionGraphVersionCfg::clear(void) {
	m_childVersions.clear();
}

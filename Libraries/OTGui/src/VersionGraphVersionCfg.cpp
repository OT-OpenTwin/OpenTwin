//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VersionGraphVersionCfg.h"

ot::VersionGraphVersionCfg::VersionGraphVersionCfg() 
	: m_directParentIsHidden(false), m_parentVersion(nullptr)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const std::string& _name, const std::string& _label, const std::string& _description)
	: m_name(_name), m_label(_label), m_description(_description), m_directParentIsHidden(false), m_parentVersion(nullptr)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const VersionGraphVersionCfg& _other)
	: m_parentVersion(nullptr), m_directParentIsHidden(false)
{
	*this = _other;
}

ot::VersionGraphVersionCfg::~VersionGraphVersionCfg() {
	if (m_parentVersion) {
		m_parentVersion->eraseChildVersion(this);
	}
	m_parentVersion = nullptr;
	this->clear();
}

ot::VersionGraphVersionCfg& ot::VersionGraphVersionCfg::operator = (const VersionGraphVersionCfg& _other) {
	if (this == &_other) return *this;

	this->clear();

	m_name = _other.m_name;
	m_label = _other.m_label;
	m_description = _other.m_description;

	for (const VersionGraphVersionCfg* child : _other.m_childVersions) {
		VersionGraphVersionCfg* newChild = new VersionGraphVersionCfg(*child);
		newChild->setParentVersion(this);
		m_childVersions.push_back(newChild);
	}
	m_directParentIsHidden = _other.m_directParentIsHidden;

	return *this;
}

void ot::VersionGraphVersionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	_object.AddMember("ParentHidden", m_directParentIsHidden, _allocator);

	JsonArray childArr;
	for (const VersionGraphVersionCfg* child : m_childVersions) {
		JsonObject childObj;
		child->addToJsonObject(childObj, _allocator);
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
		VersionGraphVersionCfg* child = new VersionGraphVersionCfg;
		child->setFromJsonObject(childObj);
		child->setParentVersion(this);
		m_childVersions.push_back(child);
	}
}

ot::VersionGraphVersionCfg* ot::VersionGraphVersionCfg::addChildVersion(const std::string& _name, const std::string& _label, const std::string& _description) {
	VersionGraphVersionCfg* newVersion = new VersionGraphVersionCfg(_name, _label, _description);
	this->addChildVersion(newVersion);
	return newVersion;
}

void ot::VersionGraphVersionCfg::addChildVersion(VersionGraphVersionCfg* _child) {
	_child->setParentVersion(this);
	m_childVersions.push_back(_child);
}

void ot::VersionGraphVersionCfg::setChildVersions(const std::list<VersionGraphVersionCfg*>& _versions) {
	this->clearChilds();
	m_childVersions = _versions;
	for (VersionGraphVersionCfg* child : _versions) {
		child->setParentVersion(this);
	}
}

ot::VersionGraphVersionCfg* ot::VersionGraphVersionCfg::findVersion(const std::string& _versionName) {
	if (_versionName == m_name) return this;
	else {
		for (VersionGraphVersionCfg* child : m_childVersions) {
			VersionGraphVersionCfg* result = child->findVersion(_versionName);
			if (result) return result;
		}
	}
	return nullptr;
}

ot::VersionGraphVersionCfg* ot::VersionGraphVersionCfg::getLastBranchVersion(void) {
	if (m_childVersions.empty()) return this;
	else return m_childVersions.front()->getLastBranchVersion();
}

void ot::VersionGraphVersionCfg::applyConfigOnly(const VersionGraphVersionCfg& _other) {
	m_name = _other.m_name;
	m_label = _other.m_label;
	m_description = _other.m_description;
	m_directParentIsHidden = _other.m_directParentIsHidden;
}

void ot::VersionGraphVersionCfg::eraseChildVersion(VersionGraphVersionCfg* _child) {
	auto it = std::find(m_childVersions.begin(), m_childVersions.end(), _child);
	if (it != m_childVersions.end()) m_childVersions.erase(it);
}

void ot::VersionGraphVersionCfg::clear(void) {
	this->clearChilds();
}

void ot::VersionGraphVersionCfg::clearChilds(void) {
	for (VersionGraphVersionCfg* child : m_childVersions) {
		child->setParentVersion(nullptr);
		delete child;
	}
	m_childVersions.clear();
}

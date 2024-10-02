//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/VersionGraphCfg.h"
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

void ot::VersionGraphVersionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator, const std::string& _customParentVersion) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	if (!_customParentVersion.empty()) {
		_object.AddMember("ParentVersion", JsonString(_customParentVersion, _allocator), _allocator);
	}
	else if (m_parentVersion) {
		_object.AddMember("ParentVersion", JsonString(m_parentVersion->getName(), _allocator), _allocator);
	}
	else {
		_object.AddMember("ParentVersion", JsonNullValue(), _allocator);
	}
	
}

bool ot::VersionGraphVersionCfg::setFromJsonObject(const ot::ConstJsonObject& _object, VersionGraphCfg* _graph) {
	this->clear();

	m_name = json::getString(_object, "Name");
	m_label = json::getString(_object, "Label");
	m_description = json::getString(_object, "Description");

	if (_graph->findVersion(m_name)) {
		OT_LOG_E("Graph already has version \"" + m_name + "\"");
		return false;
	}

	if (_object.HasMember("ParentVersion")) {
		if (_object["ParentVersion"].IsString()) {
			VersionGraphVersionCfg* parent = _graph->findVersion(_object["ParentVersion"].GetString());
			if (!parent) {
				OT_LOG_EA("Parent version not found");
				return false;
			}
			parent->addChildVersion(this);
		}
		else {
			_graph->setRootVersion(this);
		}
		return true;
	}
	else {
		OT_LOG_EA("ParentVersion is missing");
		return false;
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

const ot::VersionGraphVersionCfg* ot::VersionGraphVersionCfg::findVersion(const std::string& _versionName) const {
	if (_versionName == m_name) return this;
	else {
		for (VersionGraphVersionCfg* child : m_childVersions) {
			VersionGraphVersionCfg* result = child->findVersion(_versionName);
			if (result) return result;
		}
	}
	return nullptr;
}

bool ot::VersionGraphVersionCfg::versionStartingWithNameExists(const std::string& _prefix) const {
	size_t ix = m_name.find(_prefix);
	if (ix == 0) return true;
	else {
		for (const VersionGraphVersionCfg* child : m_childVersions) {
			if (child->versionStartingWithNameExists(_prefix)) return true;
		}
	}
	return false;
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

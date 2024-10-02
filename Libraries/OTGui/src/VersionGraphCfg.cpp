//! @file VersionGraphCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VersionGraphCfg.h"

ot::VersionGraphCfg::VersionGraphCfg() {
	m_rootVersion = new VersionGraphVersionCfg;
}

ot::VersionGraphCfg::VersionGraphCfg(const VersionGraphCfg& _other) {
	*this = _other;
}

ot::VersionGraphCfg::~VersionGraphCfg() {
	if (m_rootVersion) delete m_rootVersion;
}

ot::VersionGraphCfg& ot::VersionGraphCfg::operator=(const VersionGraphCfg& _other) {
	if (this == &_other) return *this;

	if (m_rootVersion) delete m_rootVersion;

	m_rootVersion = new VersionGraphVersionCfg(*_other.m_rootVersion);
	m_activeVersionName = _other.m_activeVersionName;
	m_activeBranchVersionName = _other.m_activeBranchVersionName;

	return *this;
}

void ot::VersionGraphCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Active", JsonString(m_activeVersionName, _allocator), _allocator);
	_object.AddMember("ActiveBranch", JsonString(m_activeBranchVersionName, _allocator), _allocator);

	ot::JsonArray versionsArr;
	this->addVersionAndChildsToArray(m_rootVersion, versionsArr, _allocator);
	_object.AddMember("Versions", versionsArr, _allocator);
}

void ot::VersionGraphCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (m_rootVersion) delete m_rootVersion;
	m_rootVersion = nullptr;

	m_activeVersionName = json::getString(_object, "Active");
	m_activeBranchVersionName = json::getString(_object, "ActiveBranch");

	// Get versions
	ConstJsonObjectList versions = json::getObjectList(_object, "Versions");
	for (const ConstJsonObject& version : versions) {
		ot::VersionGraphVersionCfg* newVersion = new ot::VersionGraphVersionCfg;
		if (!newVersion->setFromJsonObject(version, this)) {
			delete newVersion;
		}
	}

	// Ensure root exists
	if (!m_rootVersion) {
		m_rootVersion = new VersionGraphVersionCfg;
	}
}

void ot::VersionGraphCfg::setRootVersion(const std::string& _name, const std::string& _label, const std::string& _description) {
	if (m_rootVersion) {
		m_rootVersion->setName(_name);
		m_rootVersion->setLabel(_label);
		m_rootVersion->setDescription(_description);
	}
	else {
		m_rootVersion = new VersionGraphVersionCfg(_name, _label, _description);
	}
	
}

void ot::VersionGraphCfg::setRootVersion(VersionGraphVersionCfg* _version) {
	if (m_rootVersion == _version) return;
	if (m_rootVersion) delete m_rootVersion;
	m_rootVersion = _version;
}

ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) {
	if (_version.empty()) return m_rootVersion;
	else if (m_rootVersion) return m_rootVersion->findVersion(_version);
	else return nullptr;
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) const {
	if (_version.empty()) return m_rootVersion;
	else if (m_rootVersion) return m_rootVersion->findVersion(_version);
	else return nullptr;
}

bool ot::VersionGraphCfg::versionStartingWithNameExists(const std::string& _prefix) {
	if (m_rootVersion) {
		return m_rootVersion->versionStartingWithNameExists(_prefix);
	}
	else {
		return false;
	}
	
}

void ot::VersionGraphCfg::removeVersion(const std::string& _version) {
	VersionGraphVersionCfg* version = this->findVersion(_version);
	if (!version) return;

	if (version == m_rootVersion) {
		delete m_rootVersion;
		m_rootVersion = new VersionGraphVersionCfg;
	}
	else {
		delete version;
	}

	if (_version == m_activeVersionName) m_activeVersionName.clear();
	if (_version == m_activeBranchVersionName) m_activeBranchVersionName.clear();
}

void ot::VersionGraphCfg::clear(void) {
	if (m_rootVersion) {
		delete m_rootVersion;
	}
	m_rootVersion = new VersionGraphVersionCfg;
	m_activeBranchVersionName.clear();
	m_activeVersionName.clear();
}

void ot::VersionGraphCfg::addVersionAndChildsToArray(const VersionGraphVersionCfg* _version, JsonArray& _versionsArray, JsonAllocator& _allocator) const {
	if (_version) {
		JsonObject obj;
		_version->addToJsonObject(obj, _allocator, "");
		_versionsArray.PushBack(obj, _allocator);
		for (VersionGraphVersionCfg* child : _version->getChildVersions()) {
			this->addVersionAndChildsToArray(child, _versionsArray, _allocator);
		}
	}
}


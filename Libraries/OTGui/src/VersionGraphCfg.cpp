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

	JsonObject rootObj;
	m_rootVersion->addToJsonObject(rootObj, _allocator);
	_object.AddMember("RootVersion", rootObj, _allocator);
}

void ot::VersionGraphCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (m_rootVersion) delete m_rootVersion;
	m_rootVersion = new VersionGraphVersionCfg;

	m_activeVersionName = json::getString(_object, "Active");
	m_activeBranchVersionName = json::getString(_object, "ActiveBranch");
	m_rootVersion->setFromJsonObject(json::getObject(_object, "RootVersion"));
}

void ot::VersionGraphCfg::setRootVersion(const std::string& _name, const std::string& _label, const std::string& _description) {
	m_rootVersion->setName(_name);
	m_rootVersion->setLabel(_label);
	m_rootVersion->setDescription(_description);
}

void ot::VersionGraphCfg::setRootVersion(VersionGraphVersionCfg* _version) {
	if (m_rootVersion == _version) return;
	if (m_rootVersion) delete m_rootVersion;
	m_rootVersion = _version;
}

ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) {
	return m_rootVersion->findVersion(_version);
}

void ot::VersionGraphCfg::removeVersion(const std::string& _version) {
	VersionGraphVersionCfg* version = m_rootVersion->findVersion(_version);
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


//! @file VersionGraphCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VersionGraphCfg.h"

ot::VersionGraphCfg::VersionGraphCfg() {

}

ot::VersionGraphCfg::VersionGraphCfg(const VersionGraphCfg& _other) {
	*this = _other;
}

ot::VersionGraphCfg::~VersionGraphCfg() {
}

ot::VersionGraphCfg& ot::VersionGraphCfg::operator=(const VersionGraphCfg& _other) {
	if (this == &_other) return *this;

	m_rootVersions = _other.m_rootVersions;
	m_activeVersionName = _other.m_activeVersionName;
	m_activeBranchVersionName = _other.m_activeBranchVersionName;

	return *this;
}

void ot::VersionGraphCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Active", JsonString(m_activeVersionName, _allocator), _allocator);
	_object.AddMember("ActiveBranch", JsonString(m_activeBranchVersionName, _allocator), _allocator);

	JsonArray rootArr;
	for (const VersionGraphVersionCfg& version : m_rootVersions) {
		JsonObject rootObj;
		version.addToJsonObject(rootObj, _allocator);
		rootArr.PushBack(rootObj, _allocator);
	}
	_object.AddMember("RootVersions", rootArr, _allocator);
}

void ot::VersionGraphCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_rootVersions.clear();

	m_activeVersionName = json::getString(_object, "Active");
	m_activeBranchVersionName = json::getString(_object, "ActiveBranch");
	
	std::list<ConstJsonObject> rootArr = json::getObjectList(_object, "RootVersions");
	for (const ConstJsonObject& rootObj : rootArr) {
		VersionGraphVersionCfg version;
		version.setFromJsonObject(rootObj);
		m_rootVersions.push_back(version);
	}
}

void ot::VersionGraphCfg::addRootVersion(const VersionGraphVersionCfg& _version) {
	m_rootVersions.push_back(_version);
}

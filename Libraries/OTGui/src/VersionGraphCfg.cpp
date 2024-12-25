//! @file VersionGraphCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/VersionGraphCfg.h"

ot::VersionGraphCfg::VersionGraphCfg() {

}

ot::VersionGraphCfg::VersionGraphCfg(VersionGraphCfg&& _other) noexcept :
	m_activeBranchVersionName(std::move(_other.m_activeBranchVersionName)), m_activeVersionName(std::move(_other.m_activeVersionName)),
	m_branches(std::move(_other.m_branches))
{

}

ot::VersionGraphCfg::~VersionGraphCfg() {
	this->clear();
}

ot::VersionGraphCfg& ot::VersionGraphCfg::operator=(VersionGraphCfg&& _other) noexcept {
	if (this != &_other) {
		m_branches = std::move(_other.m_branches);
		m_activeVersionName = std::move(_other.m_activeVersionName);
		m_activeBranchVersionName = std::move(_other.m_activeBranchVersionName);
	}
	return *this;
}

void ot::VersionGraphCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Active", JsonString(m_activeVersionName, _allocator), _allocator);
	_object.AddMember("ActiveBranch", JsonString(m_activeBranchVersionName, _allocator), _allocator);

	JsonArray branchesArr;
	for (const std::list<VersionGraphVersionCfg>& versions : m_branches) {
		JsonArray versionsArr;
		for (const VersionGraphVersionCfg& version : versions) {
			JsonObject versionObj;
			version.addToJsonObject(versionObj, _allocator);
			versionsArr.PushBack(versionObj, _allocator);
		}
		branchesArr.PushBack(versionsArr, _allocator);
	}
	_object.AddMember("Branches", branchesArr, _allocator);
}

void ot::VersionGraphCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();

	m_activeVersionName = json::getString(_object, "Active");
	m_activeBranchVersionName = json::getString(_object, "ActiveBranch");

	// Get versions
	ConstJsonArrayList branchesArr = json::getArrayList(_object, "Branches");
	for (const ConstJsonArray& branchVersions : branchesArr) {
		std::list<VersionGraphVersionCfg> versions;
		for (JsonSizeType i = 0; i < branchVersions.Size(); i++) {
			ConstJsonObject versionObj = json::getObject(branchVersions, i);
			ot::VersionGraphVersionCfg newVersion;
			newVersion.setFromJsonObject(versionObj);
			versions.push_back(std::move(newVersion));
		}
		if (!versions.empty()) {
			m_branches.push_back(std::move(versions));
		}
	}
}

ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) {
	for (std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		if (!branchVersions.empty()) {
			if (_version == branchVersions.back()) {
				return &branchVersions.back();
			}
		}
		for (VersionGraphVersionCfg& version : branchVersions) {
			if (_version == version) {
				return &version;
			}
		}
	}
	OT_LOG_E("Version \"" + _version + "\" not found");
	return nullptr;
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) const {
	for (const std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		if (!branchVersions.empty()) {
			if (_version == branchVersions.back()) {
				return &branchVersions.back();
			}
		}
		for (const VersionGraphVersionCfg& version : branchVersions) {
			if (_version == version) {
				return &version;
			}
		}
	}
	OT_LOG_E("Version \"" + _version + "\" not found");
	return nullptr;
}

bool ot::VersionGraphCfg::versionStartingWithNameExists(const std::string& _prefix) {
	for (const std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		for (const VersionGraphVersionCfg& version : branchVersions) {
			if (version.getName().find(_prefix) == 0) {
				return true;
			}
		}
	}
	return false;
}

void ot::VersionGraphCfg::removeVersion(const std::string& _version) {
	for (std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		auto it = std::find(branchVersions.begin(), branchVersions.end(), _version);
		if (it != branchVersions.end()) {
			branchVersions.erase(it);
		}
	}
	
	if (_version == m_activeVersionName) {
		m_activeVersionName.clear();
	}
	if (_version == m_activeBranchVersionName) {
		m_activeBranchVersionName.clear();
	}
}

void ot::VersionGraphCfg::clear(void) {
	m_branches.clear();
	m_activeBranchVersionName.clear();
	m_activeVersionName.clear();
}

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
	m_activeBranchName(std::move(_other.m_activeBranchName)), m_activeVersionName(std::move(_other.m_activeVersionName)),
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
		m_activeBranchName = std::move(_other.m_activeBranchName);
	}
	return *this;
}

void ot::VersionGraphCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Active", JsonString(m_activeVersionName, _allocator), _allocator);
	_object.AddMember("ActiveBranch", JsonString(m_activeBranchName, _allocator), _allocator);

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
	m_activeBranchName = json::getString(_object, "ActiveBranch");

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

void ot::VersionGraphCfg::addVersion(const std::string& _version, const std::string& _parentVersion, const std::string& _label, const std::string& _description) {
	VersionGraphVersionCfg newVersion(_version, _label, _description);
	newVersion.setParentVersion(_parentVersion);
	this->addVersion(std::move(newVersion));
}

void ot::VersionGraphCfg::addVersion(VersionGraphVersionCfg&& _version) {
	std::string branchName = _version.getBranchName();

	// Add version to existing branch
	for (std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchName() == branchName) {
			branchVersions.push_back(std::move(_version));
			return;
		}
	}

	// Create new branch
	std::list<VersionGraphVersionCfg> newBranch;
	newBranch.push_back(std::move(_version));
	m_branches.push_back(std::move(newBranch));
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

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findLastVersion(void) {
	return this->findLastVersion(m_activeBranchName);
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findLastVersion(const std::string& _branchName) {
	const VersionsList* versions = this->findBranch(_branchName);
	if (versions) {
		return &versions->back();
	}
	else {
		return nullptr;
	}
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

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findPreviousVersion(const std::string& _version) const {
	// Find current version
	const VersionsList* versionsList = nullptr;
	VersionsList::const_iterator versionIt;
	if (!this->findVersionIterator(_version, versionsList, versionIt)) {
		OT_LOG_W("Version \"" + _version + "\" not found");
		return nullptr;
	}

	if (versionIt == versionsList->begin()) {
		// Version is beginning of a branch, find in other branches.

		const std::string branchNode = versionIt->getBranchNodeName();
		if (branchNode.empty()) {
			// Either the specified version is the first version of the "main" branch, or the version is invalid
			return nullptr;
		}
		else {
			return this->findVersion(branchNode);
		}
	}
	else {
		// Version is in the middle/end of a branch
		versionIt--;
		return &(*versionIt);
	}
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findNextVersion(const std::string& _version) const {
	const VersionsList* tmp = nullptr;
	VersionsList::const_iterator tmpIt;
	return this->findNextVersion(_version, tmp, tmpIt);
}

std::list<const ot::VersionGraphVersionCfg*> ot::VersionGraphCfg::findNextVersions(const std::string& _version, const std::string& _lastVersion) {
	const VersionsList* lst = nullptr;
	VersionsList::const_iterator it;
	
	std::list<const ot::VersionGraphVersionCfg*> result;
	const ot::VersionGraphVersionCfg* nextVersion = this->findNextVersion(_version, lst, it);
	while (nextVersion) {
		result.push_back(nextVersion);
		if (nextVersion->getName() == _lastVersion) {
			break;
		}
		nextVersion = this->findNextVersion(nextVersion->getName(), lst, it);
	}
	return result;
}

std::list<const ot::VersionGraphVersionCfg*> ot::VersionGraphCfg::findAllNextVersions(const std::string& _version) {
	std::list<const ot::VersionGraphVersionCfg*> result;

	const VersionsList* lst = nullptr;
	VersionsList::const_iterator it;
	if (!this->findVersionIterator(_version, lst, it)) {
		OT_LOG_E("Version \"" + _version + "\" not found");
		return result;
	}

	// Current version branch
	it++;
	if (it != lst->end()) {
		this->findAllNextVersions(result, lst, it);
	}

	// Check branches
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchNodeName() == _version) {
			this->findAllNextVersions(result, &branchVersions, branchVersions.begin());
		}
	}

	return result;
}

bool ot::VersionGraphCfg::versionIsEndOfBranch(const std::string& _version) const {
	for (const std::list<VersionGraphVersionCfg>& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.back() == _version) {
			return true;
		}
	}

	return false;
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
	for (auto branchIt = m_branches.begin(); branchIt != m_branches.end(); branchIt++) {
		auto versionIt = std::find(branchIt->begin(), branchIt->end(), _version);
		if (versionIt != branchIt->end()) {
			branchIt->erase(versionIt);
		}
		if (branchIt->empty()) {
			m_branches.erase(branchIt);
			break;
		}
	}
	
	if (_version == m_activeVersionName) {
		m_activeVersionName.clear();
	}
	if (_version == m_activeBranchName) {
		m_activeBranchName.clear();
	}
}

void ot::VersionGraphCfg::clear(void) {
	m_branches.clear();
	m_activeBranchName.clear();
	m_activeVersionName.clear();
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findNextVersion(const std::string& _version, const VersionsList*& _versionList, VersionsList::const_iterator& _currentVersionListIterator) const {
	// First check if any branch starts as child of this version
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		const std::string branchNode = branchVersions.front().getBranchNodeName();

		// Check if the first version of the branch has _version as a branch node name and check if the active branch name starts with the branch node name aswell.
		if (branchVersions.front().getBranchNodeName() == _version && m_activeBranchName.find(branchNode) == 0) {
			_versionList = &branchVersions;
			_currentVersionListIterator = branchVersions.begin();
			return &branchVersions.front();
		}
	}

	// Here we know that no branch beginning is a child of the specified version.

	// Find the iterator
	if (_versionList == nullptr) {
		if (!this->findVersionIterator(_version, _versionList, _currentVersionListIterator)) {
			OT_LOG_W("Version not found \"" + _version + "\"");
			return nullptr;
		}
	}
	

	// Go to next version in the branch
	_currentVersionListIterator++;

	if (_currentVersionListIterator == _versionList->end()) {
		// Version is end of branch
		_versionList = nullptr;
		return nullptr;
	}
	else {
		// Return next
		return &(*_currentVersionListIterator);
	}
}

void ot::VersionGraphCfg::findAllNextVersions(std::list<const VersionGraphVersionCfg*>& _result, const VersionsList* _list, VersionsList::const_iterator _iterator) const {
	for (; _iterator != _list->end(); _iterator++) {
		OTAssertNullptr(*_iterator);
		_result.push_back(&(*_iterator));

		// Check branches
		for (const VersionsList& branchVersions : m_branches) {
			OTAssert(!branchVersions.empty(), "Empty branch stored");
			if (branchVersions.front().getBranchNodeName() == _iterator->getName()) {
				this->findAllNextVersions(_result, &branchVersions, branchVersions.begin());
			}
		}
	}
}

bool ot::VersionGraphCfg::findVersionIterator(const std::string& _versionName, const VersionsList*& _list, VersionsList::const_iterator& _iterator) const {
	for (const VersionsList& branchVersions : m_branches) {
		for (VersionsList::const_iterator it = branchVersions.begin(); it != branchVersions.end(); it++) {
			if (it->getName() == _versionName) {
				_list = &branchVersions;
				_iterator = it;
				return true;
			}
		}
	}

	return false;
}

const ot::VersionGraphCfg::VersionsList* ot::VersionGraphCfg::findBranch(const std::string& _branchName) const {
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchName() == _branchName) {
			return &branchVersions;
		}
	}

	OT_LOG_W("Branch \"" + _branchName + "\" not found");
	return nullptr;
}

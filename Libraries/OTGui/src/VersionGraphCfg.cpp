//! @file VersionGraphCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
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
		VersionsList versions;
		for (JsonSizeType i = 0; i < branchVersions.Size(); i++) {
			ConstJsonObject versionObj = json::getObject(branchVersions, i);
			VersionGraphVersionCfg newVersion;
			newVersion.setFromJsonObject(versionObj);
			versions.push_back(std::move(newVersion));
		}
		if (!versions.empty()) {
			this->insertBranch(std::move(versions));
		}
	}
}

void ot::VersionGraphCfg::incrementActiveVersion(void) {
	m_activeVersionName = this->incrementVersion(m_activeVersionName);
}

std::string ot::VersionGraphCfg::incrementVersion(const std::string& _version) {
	// If the current version is empty the incremented version is 1
	if (_version.empty()) {
		return "1";
	}

	// Find current version number
	size_t ix = _version.rfind('.');
	std::string prefix;
	VersionGraphVersionCfg::VersionNumberType versionNumber = 0;
	bool failed = false;
	
	if (ix == std::string::npos) {
		versionNumber = String::toNumber<VersionGraphVersionCfg::VersionNumberType>(_version, failed);
	}
	else {
		versionNumber = String::toNumber<VersionGraphVersionCfg::VersionNumberType>(_version.substr(ix + 1), failed);
		if (!failed) {
			prefix = _version.substr(0, ix + 1);
		}
	}

	if (failed) {
		OT_LOG_E("Version syntax error on version \"" + _version + "\"");
		return std::string();
	}
	else {
		// Increment version
		versionNumber++;

		return prefix + std::to_string(versionNumber);
	}
}

ot::VersionGraphVersionCfg& ot::VersionGraphCfg::insertVersion(const std::string& _version, const std::string& _parentVersion, const std::string& _label, const std::string& _description) {
	VersionGraphVersionCfg newVersion(_version, _label, _description);
	newVersion.setParentVersion(_parentVersion);
	return this->insertVersion(std::move(newVersion));
}

ot::VersionGraphVersionCfg& ot::VersionGraphCfg::insertVersion(VersionGraphVersionCfg&& _version) {
	std::string branchName = _version.getBranchName();

	VersionsList* branch = this->findBranch(_version.getBranchName());
	if (branch) {
		// Insert into existing branch

		VersionGraphVersionCfg::VersionNumberType versionNumber = _version.getVersionNumber();

		// Attempt to insert to back
		if (branch->empty() || (versionNumber > branch->back().getVersionNumber())) {
			branch->push_back(std::move(_version));
			return branch->back();
		}

		// Insert in sorted order
		for (auto it = branch->begin(); it != branch->end(); it++) {
			if (versionNumber < it->getVersionNumber()) {
				// Greater version found, insert before
				return *branch->insert(it, std::move(_version));
			}
			else if (versionNumber == it->getVersionNumber()) {
				// Duplicate version
				OT_LOG_W("Version \"" + _version.getName() + "\" already exists in branch \"" + _version.getBranchName() + "\"");
				return *it; // return existing version
			}
		}

		OTAssert(0, "Insert location not found, but version is not greater than back. This should never happen");
		branch->push_back(std::move(_version));
		return branch->back();
	}

	// Create new branch
	VersionsList newBranch;
	newBranch.push_back(std::move(_version));
	return this->insertBranch(std::move(newBranch)).back();
}

ot::VersionGraphCfg::VersionsList& ot::VersionGraphCfg::insertBranch(VersionsList&& _branch) {
	return VersionGraphCfg::insertBranch(std::move(_branch), m_branches);
}

ot::VersionGraphCfg::VersionsList& ot::VersionGraphCfg::insertBranch(VersionsList&& _branch, std::list<VersionsList>& _branchesList) {
	OTAssert(!_branch.empty(), "Empty branch provided");
	OTAssert(_branch.front().isValid(), "Invalid branch");

	std::vector<VersionGraphVersionCfg::VersionNumberType> newBranchNumbers = VersionGraphVersionCfg::getVersionNumbers(_branch.front().getBranchName());

	// "main" branch should always be first
	if (newBranchNumbers.empty()) {
		_branchesList.push_front(std::move(_branch));
		return _branchesList.front();
	}

	for (auto it = _branchesList.begin(); it != _branchesList.end(); it++) {
		if (it->front().getBranchName().empty()) {
			continue; // Main branch always first
		}
		else if (it->front().getBranchName() == _branch.front().getBranchName()) {
			throw std::exception("Branch already exists");
		}

		std::vector<VersionGraphVersionCfg::VersionNumberType> existingBranchNumbers = VersionGraphVersionCfg::getVersionNumbers(it->front().getBranchName());

		bool isVersion = true;
		size_t ix = 0;
		bool allEqual = true;
		while (ix < newBranchNumbers.size() && ix < existingBranchNumbers.size()) {
			if (isVersion && allEqual && (newBranchNumbers[ix] > existingBranchNumbers[ix])) {
				// Branch node version > than existing
				return *_branchesList.insert(it, std::move(_branch));
			}
			else if (isVersion && (newBranchNumbers[ix] < existingBranchNumbers[ix])) {
				allEqual = false;
			}
			else if (!isVersion && allEqual && (newBranchNumbers[ix] < existingBranchNumbers[ix])) {
				// Branch number < existing
				return *_branchesList.insert(it, std::move(_branch));
			}
			else if (!isVersion && (newBranchNumbers[ix] > existingBranchNumbers[ix])) {
				allEqual = false;
			}

			ix++;
			isVersion = !isVersion;
		}

		if (allEqual && isVersion && ix < existingBranchNumbers.size()) {
			// e.g. existing 3.1.3.1 inserting 3.1
			return *_branchesList.insert(it, std::move(_branch));
		}
	}

	// No insert location found, put "at bottom".
	_branchesList.push_back(std::move(_branch));
	return _branchesList.back();
}

ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) {
	VersionGraphVersionCfg versionInfo(_version);

	// Get branch
	VersionsList* branchVersions = this->findBranch(versionInfo.getBranchName());

	if (branchVersions) {
		// First check back since it it is the most common access.
		if (_version == branchVersions->back().getName()) {
			return &branchVersions->back();
		}

		// Check all branch versions.
		for (VersionGraphVersionCfg& version : *branchVersions) {
			if (_version == version.getName()) {
				return &version;
			}
		}
	}

	return nullptr;
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findVersion(const std::string& _version) const {
	VersionGraphVersionCfg versionInfo(_version);

	// Get branch
	const VersionsList* branchVersions = this->findBranch(versionInfo.getBranchName());

	if (branchVersions) {
		// First check back since it it is the most common access.
		if (_version == branchVersions->back().getName()) {
			return &branchVersions->back();
		}

		// Check all branch versions.
		for (const VersionGraphVersionCfg& version : *branchVersions) {
			if (_version == version.getName()) {
				return &version;
			}
		}
	}

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

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findPreviousVersion(const std::string& _version) const {
	// Find current version
	const VersionsList* versionsList = nullptr;
	VersionsList::const_iterator versionIt;
	if (!this->findVersionIterator(_version, versionsList, versionIt)) {
		OT_LOG_E("Version \"" + _version + "\" not found");
		return nullptr;
	}

	// Version is beginning of a branch, find in other branches.
	if (versionIt == versionsList->begin()) {

		// Example structure:
		// 
		// 1 --+----> 3
		//     |
		//     +--> 1.2.2.3.3.4.4
		//                      ^-- Searching from here.
		// 
		// Version / BranchNode  -  Branch / BranchNodeBranch
		// 1                     -  <empty>      
		// 1.2.2                 -  1.2           ^
		// 1.2.2.3.3             -  1.2.2.3       |
		// 1.2.2.3.3.4.4         -  1.2.2.3.3.4   |
		//                                        |

		VersionGraphVersionCfg branchNode(versionIt->getBranchNodeName());
		while (branchNode.isValid()) {
			const VersionsList* branch = this->findBranch(branchNode.getBranchName());
			if (branch) {
				const VersionGraphVersionCfg* parentVersion = nullptr;
				for (const VersionGraphVersionCfg& branchVersion : *branch) {
					if (branchVersion.getVersionNumber() > branchNode.getVersionNumber()) {
						break;
					}
					OTAssert((parentVersion == nullptr ? true : parentVersion->getVersionNumber() < branchVersion.getVersionNumber()), "Invalid version order");
					parentVersion = &branchVersion;
				}

				if (parentVersion) {
					return parentVersion;
				}
			}
			branchNode = VersionGraphVersionCfg(branchNode.getBranchNodeName());
		}

		// This should happen only for version "1" since this version should always exist.
		OTAssert(_version == "1", "Previous version could not be found");
		return nullptr;
	}
	else {
		// Version is in the middle/end of a branch:
		// 1 --+-- 2 ...
		//     |
		//     +-- 1.2.1 --- 1.2.2 --- 1.2.3  <-searching from 1.2.3, parent is 1.2.2
		versionIt--;
		return &(*versionIt);
	}
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findNextVersion(const std::string& _version) const {
	return this->findNextVersion(_version, m_activeBranchName);
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findNextVersion(const std::string& _version, const std::string& _activeBranch) const {
	const VersionsList* tmp = nullptr;
	VersionsList::const_iterator tmpIt;
	return this->findNextVersion(_version, _activeBranch, tmp, tmpIt);
}

std::list<const ot::VersionGraphVersionCfg*> ot::VersionGraphCfg::findNextVersions(const std::string& _version, const std::string& _lastVersion) {
	return this->findNextVersions(_version, m_activeBranchName, _lastVersion);
}

std::list<const ot::VersionGraphVersionCfg*> ot::VersionGraphCfg::findNextVersions(const std::string& _version, const std::string& _activeBranch, const std::string& _lastVersion) {
	const VersionsList* lst = nullptr;
	VersionsList::const_iterator it;

	std::list<const VersionGraphVersionCfg*> result;
	const VersionGraphVersionCfg* nextVersion = this->findNextVersion(_version, _activeBranch, lst, it);
	while (nextVersion) {
		result.push_back(nextVersion);
		if (nextVersion->getName() == _lastVersion) {
			break;
		}
		nextVersion = this->findNextVersion(nextVersion->getName(), _activeBranch, lst, it);
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

bool ot::VersionGraphCfg::getBranchExists(const std::string& _branchName) const {
	return this->findBranch(_branchName) != nullptr;
}

int ot::VersionGraphCfg::getBranchesCountFromNode(const std::string& _version) const {
	int result = 0;
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchNodeName() == _version) {
			result++;
		}
	}
	return result;
}

std::list<std::list<ot::VersionGraphVersionCfg>> ot::VersionGraphCfg::getBranchesFromNode(const std::string& _version) const {
	std::list<std::list<VersionGraphVersionCfg>> branches;
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchNodeName() == _version) {
			branches.push_back(branchVersions);
		}
	}

	return branches;
}

void ot::VersionGraphCfg::clear(void) {
	m_branches.clear();
	m_activeBranchName.clear();
	m_activeVersionName.clear();
}

const ot::VersionGraphVersionCfg* ot::VersionGraphCfg::findNextVersion(const std::string& _version, const std::string& _activeBranch, const VersionsList*& _versionList, VersionsList::const_iterator& _currentVersionListIterator) const {
	if (_version.empty()) {
		return nullptr;
	}
	
	// First check if any branch starts as child of this version
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		const std::string branchName = branchVersions.front().getBranchName();

		// Check if the first version of the branch has _version as a branch node name and check if the active branch name starts with the branch node name aswell.
		if (branchVersions.front().getBranchNodeName() == _version && _activeBranch.find(branchName) == 0) {
			_versionList = &branchVersions;
			_currentVersionListIterator = branchVersions.begin();
			return &branchVersions.front();
		}
	}

	// Here we know that no branch beginning is a child of the specified version.

	// Find the iterator
	if (_versionList == nullptr) {
		if (!this->findVersionIterator(_version, _versionList, _currentVersionListIterator)) {
			OT_LOG_E("Version not found \"" + _version + "\"");
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
		OTAssert(!branchVersions.empty(), "Empty branch stored");

		// First check back since it it is the most common access
		if (_versionName == branchVersions.back().getName()) {
			_list = &branchVersions;
			_iterator = --branchVersions.end();
			return true;
		}

		// Check all branch versions
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

ot::VersionGraphCfg::VersionsList* ot::VersionGraphCfg::findBranch(const std::string& _branchName) {
	for (VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchName() == _branchName) {
			return &branchVersions;
		}
	}
	return nullptr;
}

const ot::VersionGraphCfg::VersionsList* ot::VersionGraphCfg::findBranch(const std::string& _branchName) const {
	for (const VersionsList& branchVersions : m_branches) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchName() == _branchName) {
			return &branchVersions;
		}
	}
	return nullptr;
}

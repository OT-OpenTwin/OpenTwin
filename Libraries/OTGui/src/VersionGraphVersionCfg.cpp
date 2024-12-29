//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTGui/VersionGraphCfg.h"
#include "OTGui/VersionGraphVersionCfg.h"

ot::VersionGraphVersionCfg ot::VersionGraphVersionCfg::createBranchNodeFromBranch(const std::string& _branch) {
	VersionGraphVersionCfg version;
	if (!_branch.empty()) {
		size_t ix = _branch.rfind('.');
		if (ix != std::string::npos) {
			version.setName(_branch.substr(0, ix));
		}
	}
	return version;
}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg()
	: m_directParentIsHidden(false)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const std::string& _versionName, const std::string& _label, const std::string& _description)
	: m_name(_versionName), m_label(_label), m_description(_description), m_directParentIsHidden(false)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const VersionGraphVersionCfg& _other) :
	m_directParentIsHidden(false)
{
	*this = _other;
}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(VersionGraphVersionCfg&& _other) noexcept :
	m_directParentIsHidden(_other.m_directParentIsHidden), m_name(std::move(_other.m_name)), m_label(std::move(_other.m_label)),
	m_description(std::move(_other.m_description)), m_parentVersion(std::move(_other.m_parentVersion))
{

}

ot::VersionGraphVersionCfg::~VersionGraphVersionCfg() {

}

ot::VersionGraphVersionCfg& ot::VersionGraphVersionCfg::operator = (const VersionGraphVersionCfg& _other) {
	if (this != &_other) {
		m_name = _other.m_name;
		m_label = _other.m_label;
		m_description = _other.m_description;
		m_parentVersion = _other.m_parentVersion;
		m_directParentIsHidden = _other.m_directParentIsHidden;
	}
	return *this;
}

ot::VersionGraphVersionCfg& ot::VersionGraphVersionCfg::operator=(VersionGraphVersionCfg&& _other) noexcept {
	if (this != &_other) {
		m_name = std::move(_other.m_name);
		m_label = std::move(_other.m_label);
		m_description = std::move(_other.m_description);
		m_parentVersion = std::move(_other.m_parentVersion);
		m_directParentIsHidden = _other.m_directParentIsHidden;
	}
	return *this;
}

bool ot::VersionGraphVersionCfg::operator==(const std::string& _name) const {
	return m_name == _name;
}

bool ot::VersionGraphVersionCfg::operator!=(const std::string& _name) const {
	return m_name != _name;
}

void ot::VersionGraphVersionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	_object.AddMember("ParentVersion", JsonString(m_parentVersion, _allocator), _allocator);
}

void ot::VersionGraphVersionCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_label = json::getString(_object, "Label");
	m_description = json::getString(_object, "Description");
	m_parentVersion = json::getString(_object, "ParentVersion");
}

std::string ot::VersionGraphVersionCfg::getBranchName(void) const {
	size_t ix = this->getName().rfind('.');
	if (ix == std::string::npos) {
		return std::string();
	}
	else {
		return this->getName().substr(0, ix);
	}
}

std::string ot::VersionGraphVersionCfg::getBranchNodeName(void) const {
	size_t ix = this->getName().rfind('.');
	if (ix == std::string::npos) {
		return std::string(); // "Main" branch
	}
	size_t ix2 = this->getName().rfind('.', ix - 1);
	if (ix2 != std::string::npos) {
		return this->getName().substr(0, ix2); // Branch node
	}
	else {
		OT_LOG_E("Version name format error. ThisVersion \"" + this->getName() + "\"");
		return std::string(); // Error
	}
}

int ot::VersionGraphVersionCfg::getVersionNumber(const std::string& _version) {
	size_t ix = _version.rfind('.');
	int result = 0;
	bool failed = false;
	if (ix == std::string::npos) {
		result = String::toNumber<int>(_version, failed);
	}
	else {
		result = String::toNumber<int>(_version.substr(ix + 1), failed);
	}

	if (failed) {
		if (ix == std::string::npos) {
			OT_LOG_EAS("Failed to convert version \"" + _version + "\" to number");
		}
		else {
			OT_LOG_EAS("Failed to convert version \"" + _version.substr(ix + 1) + "\" to number");
		}
	}

	return result;
}

bool ot::VersionGraphVersionCfg::isValid(void) const {
	return VersionGraphVersionCfg::isValid(m_name);
}

bool ot::VersionGraphVersionCfg::isValid(const std::string& _versionName) {
	size_t fromIx = 0;
	const size_t len = _versionName.length();
	while (fromIx < len) {
		size_t ix = _versionName.find('.', fromIx);
		if (ix == std::string::npos) {
			// Last version number (e.g. (1 = 1) or (1.2.2 = 1.2.>2<, ...) 
			return String::isNumber<int>(_versionName.substr(fromIx));
		}
		else {
			// Mid version/branch number (e.g. (1.2.2 = >1<.2.2, or 1.>2<.2, ...)
			if (!String::isNumber<int>(_versionName.substr(fromIx, ix - fromIx))) {
				return false;
			}
			fromIx = ix + 1;
		}
	}

	// Empty version name is not valid.
	return false;
}

bool ot::VersionGraphVersionCfg::isOnActivePath(const std::string& _activeBranchName) const {
	OTAssert(this->isValid(), "Invalid version");

	VersionGraphVersionCfg branchNode;
	if (_activeBranchName.empty()) {
		// "main branch"
		branchNode.setName(std::to_string(std::numeric_limits<int>::max()));
	}
	else {
		// other branches
		branchNode.setName(_activeBranchName + "." + std::to_string(std::numeric_limits<int>::max()));
	}

	// Find branch
	while (branchNode.isValid()) {
		if (this->getBranchName() == branchNode.getBranchName()) {
			return this->getVersionNumber() <= branchNode.getVersionNumber();
		}
		branchNode = branchNode.getBranchNodeName();
	}

	return false;
}

bool ot::VersionGraphVersionCfg::isOnActivePath(const std::string& _versionName, const std::string& _activeBranchName) {
	VersionGraphVersionCfg version(_versionName);
	return version.isOnActivePath(_activeBranchName);
}

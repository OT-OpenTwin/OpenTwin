//! @file VersionGraphVersionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/VersionGraphCfg.h"
#include "OTGui/VersionGraphVersionCfg.h"

ot::VersionGraphVersionCfg::VersionGraphVersionCfg() 
	: m_directParentIsHidden(false)
{

}

ot::VersionGraphVersionCfg::VersionGraphVersionCfg(const std::string& _name, const std::string& _label, const std::string& _description)
	: m_name(_name), m_label(_label), m_description(_description), m_directParentIsHidden(false)
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
		OT_LOG_E("Version name format error");
		return std::string(); // Error
	}
}

std::string ot::VersionGraphVersionCfg::getBranchNodeBranchName(void) const {
	size_t ix = this->getName().rfind('.');
	std::string result;
	if (ix == std::string::npos) {
		return result; // "Main" branch
	}
	size_t ix2 = this->getName().rfind('.', ix - 1);
	if (ix2 == std::string::npos) {
		OT_LOG_E("Version name format error");
		return result;
	}

	size_t ix3 = this->getName().rfind('.', ix2 - 1);
	if (ix3 != std::string::npos) {
		result = this->getName().substr(0, ix3);
	}
	return result;
}

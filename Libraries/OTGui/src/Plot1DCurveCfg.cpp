//! @file Plot1DCurveCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Plot1DCurveCfg.h"

ot::Plot1DCurveCfg::Plot1DCurveCfg() 
	: m_id(0), m_version(0)
{

}

ot::Plot1DCurveCfg::Plot1DCurveCfg(UID _id, UID _version, const std::string& _name)
	: m_id(_id), m_version(_version), m_name(_name)
{}

ot::Plot1DCurveCfg::Plot1DCurveCfg(const Plot1DCurveCfg& _other) {
	*this = _other;
}

ot::Plot1DCurveCfg::~Plot1DCurveCfg() {}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const Plot1DCurveCfg& _other) {
	if (this == &_other) return *this;

	m_id = _other.m_id;
	m_version = _other.m_version;
	m_name = _other.m_name;

	return *this;
}

void ot::Plot1DCurveCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", m_id, _allocator);
	_object.AddMember("Version", m_version, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
}

void ot::Plot1DCurveCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_id = json::getUInt64(_object, "ID");
	m_version = json::getUInt64(_object, "Version");
	m_name = json::getString(_object, "Name");
}

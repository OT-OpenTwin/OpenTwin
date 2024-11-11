//! @file Plot1DCurveInfoCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Plot1DCurveInfoCfg.h"

ot::Plot1DCurveInfoCfg::Plot1DCurveInfoCfg() 
	: m_id(0), m_version(0), m_treeId(0), m_dimmed(false)
{

}

ot::Plot1DCurveInfoCfg::Plot1DCurveInfoCfg(UID _id, UID _version, const std::string& _name)
	: m_id(_id), m_version(_version), m_name(_name), m_treeId(0), m_dimmed(false)
{}

ot::Plot1DCurveInfoCfg::Plot1DCurveInfoCfg(const Plot1DCurveInfoCfg& _other) {
	*this = _other;
}

ot::Plot1DCurveInfoCfg::~Plot1DCurveInfoCfg() {}

ot::Plot1DCurveInfoCfg& ot::Plot1DCurveInfoCfg::operator=(const Plot1DCurveInfoCfg& _other) {
	if (this == &_other) return *this;

	m_id = _other.m_id;
	m_version = _other.m_version;
	m_name = _other.m_name;
	m_treeId = _other.m_treeId;
	m_dimmed = _other.m_dimmed;

	return *this;
}

void ot::Plot1DCurveInfoCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", m_id, _allocator);
	_object.AddMember("Version", m_version, _allocator);
	_object.AddMember("TreeId", m_treeId, _allocator);
	_object.AddMember("Dimmed", m_dimmed, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
}

void ot::Plot1DCurveInfoCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_id = json::getUInt64(_object, "ID");
	m_version = json::getUInt64(_object, "Version");
	m_name = json::getString(_object, "Name");
	m_treeId = json::getUInt64(_object, "TreeId");
	m_dimmed = json::getBool(_object, "Dimmed");
}

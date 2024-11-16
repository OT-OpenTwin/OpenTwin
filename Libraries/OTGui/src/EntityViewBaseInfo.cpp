//! @file EntityViewBaseInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/EntityViewBaseInfo.h"

ot::EntityViewBaseInfo::EntityViewBaseInfo() 
	: m_entityId(0), m_entityVersion(0)
{}

ot::EntityViewBaseInfo::EntityViewBaseInfo(const std::string& _name) 
	: m_entityId(0), m_entityVersion(0), m_name(_name)
{}

ot::EntityViewBaseInfo::EntityViewBaseInfo(const std::string& _name, UID _entityId, UID _entityVersion) 
	: m_entityId(_entityId), m_entityVersion(_entityVersion), m_name(_name)
{}

ot::EntityViewBaseInfo::EntityViewBaseInfo(const std::string& _name, const std::string& _title, UID _entityId, UID _entityVersion) 
	: m_entityId(_entityId), m_entityVersion(_entityVersion), m_name(_name), m_title(_title)
{}

ot::EntityViewBaseInfo::EntityViewBaseInfo(const EntityViewBaseInfo & _other) 
	: m_entityId(_other.m_entityId), m_entityVersion(_other.m_entityVersion), m_name(_other.m_name), m_title(_other.m_title)
{}

ot::EntityViewBaseInfo::~EntityViewBaseInfo() {}

ot::EntityViewBaseInfo& ot::EntityViewBaseInfo::operator=(const EntityViewBaseInfo& _other) {
	if (this != &_other) {
		m_entityId = _other.m_entityId;
		m_entityVersion = _other.m_entityVersion;
		m_name = _other.m_name;
		m_title = _other.m_title;
	}
	return *this;
}

void ot::EntityViewBaseInfo::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("EntityID", m_entityId, _allocator);
	_object.AddMember("EntityVersion", m_entityVersion, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
}

void ot::EntityViewBaseInfo::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entityId = json::getUInt64(_object, "EntityID");
	m_entityVersion = json::getUInt64(_object, "EntityVersion");
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
}

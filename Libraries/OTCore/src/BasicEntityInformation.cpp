// @otlicense

//! @file BasicBasicEntityInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/BasicEntityInformation.h"

ot::BasicEntityInformation::BasicEntityInformation()
	: m_entityId(0), m_entityVersion(0)
{}

ot::BasicEntityInformation::BasicEntityInformation(UID _entityId, UID _entityVersion)
	: m_entityId(_entityId), m_entityVersion(_entityVersion)
{}

ot::BasicEntityInformation::BasicEntityInformation(const std::string& _entityName, UID _entityId, UID _entityVersion)
	: m_entityName(_entityName), m_entityId(_entityId), m_entityVersion(_entityVersion)
{}

ot::BasicEntityInformation::BasicEntityInformation(BasicEntityInformation&& _other) noexcept :
	m_entityName(std::move(_other.m_entityName)), m_entityId(_other.m_entityId), m_entityVersion(_other.m_entityVersion)
{}

ot::BasicEntityInformation::BasicEntityInformation(const BasicEntityInformation& _other) 
	: m_entityName(_other.m_entityName), m_entityId(_other.m_entityId), m_entityVersion(_other.m_entityVersion)
{}

ot::BasicEntityInformation::~BasicEntityInformation() {}

ot::BasicEntityInformation& ot::BasicEntityInformation::operator=(BasicEntityInformation&& _other) noexcept {
	if (this != &_other) {
		m_entityName = std::move(_other.m_entityName);
		m_entityId = _other.m_entityId;
		m_entityVersion = _other.m_entityVersion;
	}

	return *this;
}

ot::BasicEntityInformation& ot::BasicEntityInformation::operator=(const BasicEntityInformation& _other) {
	if (this != &_other) {
		m_entityName = _other.m_entityName;
		m_entityId = _other.m_entityId;
		m_entityVersion = _other.m_entityVersion;
	}
	return *this;
}

bool ot::BasicEntityInformation::operator==(const BasicEntityInformation& _other) const {
	return (m_entityId == _other.m_entityId) &&
		(m_entityName == _other.m_entityName) &&
		(m_entityVersion == _other.m_entityVersion);
}

bool ot::BasicEntityInformation::operator!=(const BasicEntityInformation& _other) const {
	return !BasicEntityInformation::operator==(_other);
}

void ot::BasicEntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("EntityName", JsonString(m_entityName, _allocator), _allocator);
	_object.AddMember("EntityID", m_entityId, _allocator);
	_object.AddMember("EntityVersion", m_entityVersion, _allocator);
}

void ot::BasicEntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entityName = json::getString(_object, "EntityName");
	m_entityId = json::getUInt64(_object, "EntityID");
	m_entityVersion = json::getUInt64(_object, "EntityVersion");
}

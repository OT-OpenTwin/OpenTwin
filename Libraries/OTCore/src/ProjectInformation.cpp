//! @file ProjectInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/ProjectInformation.h"

ot::ProjectInformation::ProjectInformation() : m_version(0), m_creationTime(0), m_lastAccessTime(0) {

}

ot::ProjectInformation::ProjectInformation(const ConstJsonObject& _jsonObject) : ProjectInformation() {
	this->setFromJsonObject(_jsonObject);
}

void ot::ProjectInformation::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Version", m_version, _allocator);
	_jsonObject.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(m_type, _allocator), _allocator);
	_jsonObject.AddMember("UserName", JsonString(m_user, _allocator), _allocator);
	_jsonObject.AddMember("Collection", JsonString(m_collection, _allocator), _allocator);
	_jsonObject.AddMember("CreationTime", m_creationTime, _allocator);
	_jsonObject.AddMember("LastAccessTime", m_lastAccessTime, _allocator);
	_jsonObject.AddMember("Groups", JsonArray(m_groups, _allocator), _allocator);
	_jsonObject.AddMember("Tags", JsonArray(m_tags, _allocator), _allocator);
	_jsonObject.AddMember("Category", JsonString(m_category, _allocator), _allocator);
}

void ot::ProjectInformation::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_version = json::getInt(_jsonObject, "Version");
	m_name = json::getString(_jsonObject, "Name");
	m_type = json::getString(_jsonObject, "Type");
	m_user = json::getString(_jsonObject, "UserName");
	m_collection = json::getString(_jsonObject, "Collection");
	m_creationTime = json::getInt64(_jsonObject, "CreationTime");
	m_lastAccessTime = json::getInt64(_jsonObject, "LastAccessTime");
	m_groups = json::getStringList(_jsonObject, "Groups");
	m_tags = json::getStringList(_jsonObject, "Tags");
	m_category = json::getString(_jsonObject, "Category");
}

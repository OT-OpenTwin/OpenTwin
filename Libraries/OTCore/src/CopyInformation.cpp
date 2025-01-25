//! @file CopyInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/CopyInformation.h"

std::string ot::CopyInformation::getCopyTypeJsonKey(void) {
	return "CopyType";
}

std::string ot::CopyInformation::getCopyVersionJsonKey(void) {
	return "CopyVersion";
}

ot::CopyInformation::CopyInformation() 
	: m_scenePosSet(false)
{

}

void ot::CopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(CopyInformation::getCopyTypeJsonKey(), _allocator), JsonString(this->getCopyType(), _allocator), _allocator);
	_object.AddMember(JsonString(CopyInformation::getCopyVersionJsonKey(), _allocator), JsonNumber((long long)this->getCopyVersion()), _allocator);
	
	_object.AddMember("Project", JsonString(m_projectName, _allocator), _allocator);
	_object.AddMember("View", JsonString(m_viewName, _allocator), _allocator);

	JsonObject viewOwnerObj;
	m_serviceInfo.addToJsonObject(viewOwnerObj, _allocator);
	_object.AddMember("ViewOwner", viewOwnerObj, _allocator);

	_object.AddMember("ScenePosSet", m_scenePosSet, _allocator);
	JsonObject posObj;
	m_scenePos.addToJsonObject(posObj, _allocator);
	_object.AddMember("ScenePos", posObj, _allocator);

	JsonArray entities;
	for (const CopyEntityInformation& entity : m_entities) {
		JsonObject entityObj;
		entity.addToJsonObject(entityObj, _allocator);
		entities.PushBack(entityObj, _allocator);
	}
	_object.AddMember("Entities", entities, _allocator);
}

void ot::CopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_projectName = json::getString(_object, "Project");
	m_viewName = json::getString(_object, "View");

	m_serviceInfo.setFromJsonObject(json::getObject(_object, "ViewOwner"));

	m_scenePosSet = json::getBool(_object, "ScenePosSet");
	m_scenePos.setFromJsonObject(json::getObject(_object, "ScenePos"));

	m_entities.clear();
	for (const ConstJsonObject& entityObj : json::getObjectList(_object, "Entities")) {
		CopyEntityInformation info;
		info.setFromJsonObject(entityObj);
		m_entities.push_back(info);
	}
}

void ot::CopyInformation::addEntity(UID _uid, const std::string& _name, const std::string& _rawData) {
	this->addEntity(CopyEntityInformation(_uid, _name, _rawData));
}

bool ot::CopyInformation::isValid(void) const {
	return !m_viewName.empty() && !m_entities.empty();
}

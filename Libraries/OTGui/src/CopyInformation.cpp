//! @file CopyInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/CopyInformation.h"

std::string ot::CopyInformation::getCopyVersionJsonKey(void) {
	return "CopyVersion";
}

ot::CopyInformation::CopyInformation()
	: m_originViewInfo(WidgetViewBase::CustomView), m_destinationViewInfo(WidgetViewBase::CustomView), m_destinationScenePosSet(false) {

}

ot::CopyInformation::CopyInformation(const ConstJsonObject& _jsonObject) 
	: CopyInformation()
{
	this->setFromJsonObject(_jsonObject);
}

void ot::CopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(CopyInformation::getCopyVersionJsonKey(), _allocator), JsonNumber((long long)this->getCopyVersion()), _allocator);

	_object.AddMember("OriginProject", JsonString(m_originProjectName, _allocator), _allocator);

	JsonObject originViewObj;
	m_originViewInfo.addToJsonObject(originViewObj, _allocator);
	_object.AddMember("OriginView", originViewObj, _allocator);

	JsonObject destinationViewObj;
	m_destinationViewInfo.addToJsonObject(destinationViewObj, _allocator);
	_object.AddMember("DestinationView", destinationViewObj, _allocator);

	JsonObject posObj;
	m_destinationScenePos.addToJsonObject(posObj, _allocator);
	_object.AddMember("DestScenePos", posObj, _allocator);
	_object.AddMember("DestScenePosSet", m_destinationScenePosSet, _allocator);

	JsonArray entities;
	for (const CopyEntityInformation& entity : m_entities) {
		JsonObject entityObj;
		entity.addToJsonObject(entityObj, _allocator);
		entities.PushBack(entityObj, _allocator);
	}
	_object.AddMember("Entities", entities, _allocator);
}

void ot::CopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_originProjectName = json::getString(_object, "OriginProject");

	m_originViewInfo.setFromJsonObject(json::getObject(_object, "OriginView"));
	m_destinationViewInfo.setFromJsonObject(json::getObject(_object, "DestinationView"));

	m_destinationScenePosSet = json::getBool(_object, "ScenePosSet");
	m_destinationScenePos.setFromJsonObject(json::getObject(_object, "ScenePos"));

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

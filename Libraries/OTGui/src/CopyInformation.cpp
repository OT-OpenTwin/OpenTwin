// @otlicense

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/CopyInformation.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Static

ot::CopyInformation ot::CopyInformation::fromRawString(const std::string& _rawString) {
	ot::CopyInformation info;

	// Attempt to deserialize document, if fail provide raw string
	ot::JsonDocument importDoc;
	if (importDoc.fromJson(_rawString)) {
		if (importDoc.IsObject()) {
			if (!info.tryToSetFromJson(importDoc.getConstObject())) {
				info.setRawData(_rawString);
			}
		}
		else {
			info.setRawData(_rawString);
		}
	}
	else {
		info.setRawData(_rawString);
	}

	return info;
}

std::string ot::CopyInformation::getCopyVersionJsonKey(void) {
	return "CopyVersion";
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

ot::CopyInformation::CopyInformation()
	: m_originViewInfo(WidgetViewBase::CustomView), m_destinationViewInfo(WidgetViewBase::CustomView), m_destinationScenePosSet(false) {

}

ot::CopyInformation::CopyInformation(const ConstJsonObject& _jsonObject) 
	: CopyInformation()
{
	this->setFromJsonObject(_jsonObject);
}

ot::CopyInformation::CopyInformation(const CopyInformation& _other) :
	m_rawData(_other.m_rawData), m_originProjectName(_other.m_originProjectName), m_originViewInfo(_other.m_originViewInfo),
	m_destinationViewInfo(_other.m_destinationViewInfo), m_destinationScenePos(_other.m_destinationScenePos), 
	m_destinationScenePosSet(_other.m_destinationScenePosSet), m_entities(_other.m_entities)
{

}

ot::CopyInformation::CopyInformation(CopyInformation&& _other) noexcept :
	m_rawData(std::move(_other.m_rawData)), m_originProjectName(std::move(_other.m_originProjectName)),
	m_originViewInfo(std::move(_other.m_originViewInfo)), m_destinationViewInfo(std::move(_other.m_destinationViewInfo)),
	m_destinationScenePos(std::move(_other.m_destinationScenePos)), m_destinationScenePosSet(std::move(_other.m_destinationScenePosSet)),
	m_entities(std::move(_other.m_entities))
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

ot::CopyInformation& ot::CopyInformation::operator=(const CopyInformation& _other) {
	if (this != &_other) {
		m_rawData = _other.m_rawData;

		m_originProjectName = _other.m_originProjectName;

		m_originViewInfo = _other.m_originViewInfo;
		m_destinationViewInfo = _other.m_destinationViewInfo;

		m_destinationScenePos = _other.m_destinationScenePos;
		m_destinationScenePosSet = _other.m_destinationScenePosSet;

		m_entities = _other.m_entities;
	}
	return *this;
}

ot::CopyInformation& ot::CopyInformation::operator=(CopyInformation&& _other) noexcept {
	if (this != &_other) {
		m_rawData = std::move(_other.m_rawData);

		m_originProjectName = std::move(_other.m_originProjectName);

		m_originViewInfo = std::move(_other.m_originViewInfo);
		m_destinationViewInfo = std::move(_other.m_destinationViewInfo);

		m_destinationScenePos = std::move(_other.m_destinationScenePos);
		m_destinationScenePosSet = std::move(_other.m_destinationScenePosSet);

		m_entities = std::move(_other.m_entities);
	}
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::CopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(CopyInformation::getCopyVersionJsonKey(), _allocator), JsonNumber((long long)this->getCopyVersion()), _allocator);

	_object.AddMember("Raw", JsonString(m_rawData, _allocator), _allocator);

	_object.AddMember("OriginProject", JsonString(m_originProjectName, _allocator), _allocator);

	JsonObject originViewObj;
	m_originViewInfo.addToJsonObject(originViewObj, _allocator);
	_object.AddMember("OriginView", originViewObj, _allocator);

	JsonObject destinationViewObj;
	m_destinationViewInfo.addToJsonObject(destinationViewObj, _allocator);
	_object.AddMember("DestinationView", destinationViewObj, _allocator);

	JsonObject posObj;
	m_destinationScenePos.addToJsonObject(posObj, _allocator);
	_object.AddMember("DestionationScenePos", posObj, _allocator);
	_object.AddMember("DestionationScenePosSet", m_destinationScenePosSet, _allocator);

	JsonArray entities;
	for (const CopyEntityInformation& entity : m_entities) {
		JsonObject entityObj;
		entity.addToJsonObject(entityObj, _allocator);
		entities.PushBack(entityObj, _allocator);
	}
	_object.AddMember("Entities", entities, _allocator);
}

void ot::CopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	// Memers that are set here MUST be checked in the tryToSetFromJson.

	// So whenever ADDING NEW members, add the corresponding check to the try method!!

	m_rawData = json::getString(_object, "Raw");

	m_originProjectName = json::getString(_object, "OriginProject");

	m_originViewInfo.setFromJsonObject(json::getObject(_object, "OriginView"));
	m_destinationViewInfo.setFromJsonObject(json::getObject(_object, "DestinationView"));

	m_destinationScenePosSet = json::getBool(_object, "DestionationScenePosSet");
	m_destinationScenePos.setFromJsonObject(json::getObject(_object, "DestionationScenePos"));

	m_entities.clear();
	for (const ConstJsonObject& entityObj : json::getObjectList(_object, "Entities")) {
		CopyEntityInformation info;
		info.setFromJsonObject(entityObj);
		m_entities.push_back(info);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Helper

bool ot::CopyInformation::tryToSetFromJson(const ot::ConstJsonObject& _object) {
	// Ensure all members are present
	if (!json::isString(_object, "Raw")) { return false; }
	else if (!json::isString(_object, "OriginProject")) { return false; }
	else if (!json::isObject(_object, "OriginView")) { return false; }
	else if (!json::isObject(_object, "DestinationView")) { return false; }
	else if (!json::isBool(_object, "DestionationScenePosSet")) { return false; }
	else if (!json::isObject(_object, "DestionationScenePos")) { return false; }
	else if (!json::isArray(_object, "Entities")) { return false; }
	else {
		// Call set from json (since all members were checked, the method will finish successfully)
		this->setFromJsonObject(_object);
		return true;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::CopyInformation::addEntity(UID _uid, const std::string& _name, const std::string& _rawData) {
	this->addEntity(CopyEntityInformation(_uid, _name, _rawData));
}

// OpenTwin header
#include "OTBlockEditorAPI/BorderLayoutBlockConnectorManagerConfiguration.h"
#include "OTBlockEditorAPI/BlockConnectorConfiguration.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_TopItems "TopItems"
#define OT_JSON_MEMBER_LeftItems "LeftItems"
#define OT_JSON_MEMBER_RightItems "RightItems"
#define OT_JSON_MEMBER_BottomItems "BottomItems"

ot::BorderLayoutBlockConnectorManagerConfiguration::~BorderLayoutBlockConnectorManagerConfiguration() {
	for (auto c : m_top) delete c;
	for (auto c : m_left) delete c;
	for (auto c : m_right) delete c;
	for (auto c : m_bottom) delete c;
}

void ot::BorderLayoutBlockConnectorManagerConfiguration::addConnector(ot::BlockConnectorConfiguration* _connector, ConnectorLocation _insertLocation) {
	switch (_insertLocation)
	{
	case ot::BlockConnectorManagerConfiguration::TOP: m_top.push_back(_connector); break;
	case ot::BlockConnectorManagerConfiguration::LEFT: m_left.push_back(_connector); break;
	case ot::BlockConnectorManagerConfiguration::RIGHT: m_right.push_back(_connector); break;
	case ot::BlockConnectorManagerConfiguration::BOTTOM: m_bottom.push_back(_connector); break;
	case ot::BlockConnectorManagerConfiguration::AUTO:
	{
		ConnectorLocation l = TOP;
		size_t c = m_top.size();
		if (m_left.size() < c) {
			c = m_left.size();
			l = LEFT;
		}
		if (m_right.size() < c) {
			c = m_right.size();
			l = RIGHT;
		}
		if (m_bottom.size() < c) {
			c = m_bottom.size();
			l = BOTTOM;
		}

		switch (l)
		{
		case ot::BlockConnectorManagerConfiguration::TOP: m_top.push_back(_connector); break;
		case ot::BlockConnectorManagerConfiguration::LEFT: m_left.push_back(_connector); break;
		case ot::BlockConnectorManagerConfiguration::RIGHT: m_right.push_back(_connector); break;
		case ot::BlockConnectorManagerConfiguration::BOTTOM: m_bottom.push_back(_connector); break;
		case ot::BlockConnectorManagerConfiguration::AUTO:
		default:
			otAssert(0, "Invalid insert location");
			OT_LOG_W("Invalid insert location");
			break;
		}
	}
		break;
	default:
		otAssert(0, "Invalid insert location");
		OT_LOG_W("Invalid insert location");
		break;
	}
}

ot::BlockConnectorConfiguration* ot::BorderLayoutBlockConnectorManagerConfiguration::findConnector(const std::string& _connectorName, ConnectorLocation _searchLocation) {
	switch (_searchLocation)
	{
	case ot::BlockConnectorManagerConfiguration::TOP: for (auto c : m_top) { if (c->name() == _connectorName) return c; }; break;
	case ot::BlockConnectorManagerConfiguration::LEFT: for (auto c : m_left) { if (c->name() == _connectorName) return c; }; break;
	case ot::BlockConnectorManagerConfiguration::RIGHT: for (auto c : m_right) { if (c->name() == _connectorName) return c; }; break;
	case ot::BlockConnectorManagerConfiguration::BOTTOM: for (auto c : m_bottom) { if (c->name() == _connectorName) return c; }; break;
	case ot::BlockConnectorManagerConfiguration::AUTO:
	{
		ot::BlockConnectorConfiguration* connector = findConnector(_connectorName, TOP);
		if (connector) return connector;
		connector = findConnector(_connectorName, LEFT);
		if (connector) return connector;
		connector = findConnector(_connectorName, RIGHT);
		if (connector) return connector;
		connector = findConnector(_connectorName, BOTTOM);
		return connector;
	}
	default:
		otAssert(0, "Invalid search location");
		OT_LOG_W("Invalid search location");
	}
	return nullptr;
}

void ot::BorderLayoutBlockConnectorManagerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::BlockConnectorManagerConfiguration::addToJsonObject(_document, _object);

	OT_rJSON_createValueArray(topArr);
	OT_rJSON_createValueArray(leftArr);
	OT_rJSON_createValueArray(rightArr);
	OT_rJSON_createValueArray(bottomArr);

	for (auto c : m_top) {
		OT_rJSON_createValueObject(obj);
		c->addToJsonObject(_document, obj);
		topArr.PushBack(obj, _document.GetAllocator());
	}
	for (auto c : m_left) {
		OT_rJSON_createValueObject(obj);
		c->addToJsonObject(_document, obj);
		leftArr.PushBack(obj, _document.GetAllocator());
	}
	for (auto c : m_right) {
		OT_rJSON_createValueObject(obj);
		c->addToJsonObject(_document, obj);
		rightArr.PushBack(obj, _document.GetAllocator());
	}
	for (auto c : m_bottom) {
		OT_rJSON_createValueObject(obj);
		c->addToJsonObject(_document, obj);
		bottomArr.PushBack(obj, _document.GetAllocator());
	}

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TopItems, topArr);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_LeftItems, leftArr);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RightItems, rightArr);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BottomItems, bottomArr);
}

void ot::BorderLayoutBlockConnectorManagerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	ot::BlockConnectorManagerConfiguration::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TopItems, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_LeftItems, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RightItems, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BottomItems, Array);

	OT_rJSON_val topArr = _object[OT_JSON_MEMBER_TopItems].GetArray();
	OT_rJSON_val leftArr = _object[OT_JSON_MEMBER_LeftItems].GetArray();
	OT_rJSON_val rightArr = _object[OT_JSON_MEMBER_RightItems].GetArray();
	OT_rJSON_val bottomArr = _object[OT_JSON_MEMBER_BottomItems].GetArray();

	m_top.clear();
	m_left.clear();
	m_right.clear();
	m_bottom.clear();

	for (rapidjson::SizeType i = 0; i < topArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(topArr, i, Object);
		OT_rJSON_val obj = topArr[i].GetObject();
		ot::BlockConnectorConfiguration* newConnector = new ot::BlockConnectorConfiguration;
		newConnector->setFromJsonObject(obj);
		m_top.push_back(newConnector);
	}
	for (rapidjson::SizeType i = 0; i < leftArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(leftArr, i, Object);
		OT_rJSON_val obj = leftArr[i].GetObject();
		ot::BlockConnectorConfiguration* newConnector = new ot::BlockConnectorConfiguration;
		newConnector->setFromJsonObject(obj);
		m_left.push_back(newConnector);
	}
	for (rapidjson::SizeType i = 0; i < rightArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(rightArr, i, Object);
		OT_rJSON_val obj = rightArr[i].GetObject();
		ot::BlockConnectorConfiguration* newConnector = new ot::BlockConnectorConfiguration;
		newConnector->setFromJsonObject(obj);
		m_right.push_back(newConnector);
	}
	for (rapidjson::SizeType i = 0; i < bottomArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(bottomArr, i, Object);
		OT_rJSON_val obj = bottomArr[i].GetObject();
		ot::BlockConnectorConfiguration* newConnector = new ot::BlockConnectorConfiguration;
		newConnector->setFromJsonObject(obj);
		m_bottom.push_back(newConnector);
	}
}
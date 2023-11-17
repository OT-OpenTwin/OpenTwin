//! @file GraphicsConnectionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsConnectionCfg.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_Member_Style "Style"
#define OT_JSON_Member_SourceUid "Source.UID"
#define OT_JSON_Member_SourceName "Source.Name"
#define OT_JSON_Member_DestinationUid "Destination.UID"
#define OT_JSON_Member_DestinationName "Destination.Name"

std::string ot::GraphicsConnectionCfg::buildKey(const std::string& _originUid, const std::string& _originItemName, const std::string& _destUid, const std::string& _destItemName) {
	return _originUid + "|" + _originItemName + "|" + _destUid + "|" + _destItemName;
}

std::string ot::GraphicsConnectionCfg::styleToString(ConnectionStyle _style) {
	switch (_style)
	{
	case ot::GraphicsConnectionCfg::DirectLine: return "DirectLine";
	case ot::GraphicsConnectionCfg::SmoothLine: return "SmoothLine";
	default:
		OT_LOG_EA("Unknown connection style");
		return "DirectLine";
	}
}

ot::GraphicsConnectionCfg::ConnectionStyle ot::GraphicsConnectionCfg::stringToStyle(const std::string _style) {
	if (_style == styleToString(DirectLine)) return DirectLine;
	else if (_style == styleToString(SmoothLine)) return SmoothLine;
	else {
		OT_LOG_EAS("Unknown connection style \"" + _style + "\"");
		return DirectLine;
	}
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg()
	: m_style(DirectLine)
{}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const std::string& _originUid, const std::string& _originConnectableName, const std::string& _destinationUid, const std::string& _destinationName)
	: m_style(DirectLine),
	m_originUID(_originUid), m_originConnectable(_originConnectableName), m_destUID(_destinationUid), m_destConnectable(_destinationName)
{}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const GraphicsConnectionCfg& _other) 
	: m_style(_other.m_style), m_originUID(_other.m_originUID), m_originConnectable(_other.m_originConnectable), m_destUID(_other.m_destUID), m_destConnectable(_other.m_destConnectable)
{

}

ot::GraphicsConnectionCfg::~GraphicsConnectionCfg() {

}

ot::GraphicsConnectionCfg& ot::GraphicsConnectionCfg::operator = (const GraphicsConnectionCfg& _other) {
	m_originUID = _other.m_originUID;
	m_originConnectable = _other.m_originConnectable;
	
	m_destUID = _other.m_destUID;
	m_destConnectable = _other.m_destConnectable;

	m_style = _other.m_style;
	return *this;
}

void ot::GraphicsConnectionCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_Member_SourceUid, this->m_originUID);
	ot::rJSON::add(_document, _object, OT_JSON_Member_SourceName, this->m_originConnectable);
	ot::rJSON::add(_document, _object, OT_JSON_Member_DestinationUid, this->m_destUID);
	ot::rJSON::add(_document, _object, OT_JSON_Member_DestinationName, this->m_destConnectable);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Style, this->styleToString(this->m_style));
}

void ot::GraphicsConnectionCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_Member_SourceUid, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_SourceName, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_DestinationUid, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_DestinationName, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Style, String);

	this->m_originUID = _object[OT_JSON_Member_SourceUid].GetString();
	this->m_originConnectable = _object[OT_JSON_Member_SourceName].GetString();
	this->m_destUID = _object[OT_JSON_Member_DestinationUid].GetString();
	this->m_destConnectable = _object[OT_JSON_Member_DestinationName].GetString();
	this->m_style = this->stringToStyle(_object[OT_JSON_Member_Style].GetString());
}

ot::GraphicsConnectionCfg ot::GraphicsConnectionCfg::getReversedConnection(void) const {
	GraphicsConnectionCfg ret(m_destUID, m_destConnectable, m_originUID, m_originConnectable);
	return ret;
}

std::string ot::GraphicsConnectionCfg::buildKey(void) const {
	return this->buildKey(m_originUID, m_originConnectable, m_destUID, m_destConnectable);
}

std::string ot::GraphicsConnectionCfg::buildReversedKey(void) const {
	return this->buildKey(m_destUID, m_destConnectable, m_originUID, m_originConnectable);
}
//! @file GraphicsConnectionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsConnectionCfg.h"

#define OT_JSON_Member_Uid "UID"
#define OT_JSON_Member_Color "Color"
#define OT_JSON_Member_Style "Style"
#define OT_JSON_Member_Width "Width"
#define OT_JSON_Member_SourceUid "Source.UID"
#define OT_JSON_Member_SourceName "Source.Name"
#define OT_JSON_Member_DestinationUid "Destination.UID"
#define OT_JSON_Member_DestinationName "Destination.Name"


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
	: m_style(DirectLine), m_width(2)
{}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const ot::UID& _originUid, const std::string& _originConnectableName, const ot::UID& _destinationUid, const std::string& _destinationName)
	: m_style(DirectLine), m_width(2),
	m_originUID(_originUid), m_originConnectable(_originConnectableName), m_destUID(_destinationUid), m_destConnectable(_destinationName)
{}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const GraphicsConnectionCfg& _other) 
	: m_style(_other.m_style), m_width(_other.m_width), m_color(_other.m_color),
	m_originUID(_other.m_originUID), m_originConnectable(_other.m_originConnectable), 
	m_destUID(_other.m_destUID), m_destConnectable(_other.m_destConnectable), m_uid(_other.m_uid)
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
	m_width = _other.m_width;
	m_color = _other.m_color;

	m_uid = _other.m_uid;
	return *this;
}

void ot::GraphicsConnectionCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Uid, m_uid, _allocator);
	_object.AddMember(OT_JSON_Member_SourceUid, m_originUID, _allocator);
	_object.AddMember(OT_JSON_Member_SourceName, JsonString(m_originConnectable, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_DestinationUid, m_destUID, _allocator);
	_object.AddMember(OT_JSON_Member_DestinationName, JsonString(m_destConnectable, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Style, JsonString(this->styleToString(this->m_style), _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Width, m_width, _allocator);
	
	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember(OT_JSON_Member_Color, colorObj, _allocator);
}

void ot::GraphicsConnectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	m_uid = json::getUInt64(_object, OT_JSON_Member_Uid);
	m_originUID = json::getUInt64(_object, OT_JSON_Member_SourceUid);
	m_originConnectable = json::getString(_object, OT_JSON_Member_SourceName);
	m_destUID = json::getUInt64(_object, OT_JSON_Member_DestinationUid);
	m_destConnectable = json::getString(_object, OT_JSON_Member_DestinationName);
	m_width = json::getInt(_object, OT_JSON_Member_Width);
	m_style = this->stringToStyle(json::getString(_object, OT_JSON_Member_Style));
	m_color.setFromJsonObject(json::getObject(_object, OT_JSON_Member_Color));
}

ot::GraphicsConnectionCfg ot::GraphicsConnectionCfg::getReversedConnection(void) const {
	GraphicsConnectionCfg ret(m_destUID, m_destConnectable, m_originUID, m_originConnectable);
	return ret;
}

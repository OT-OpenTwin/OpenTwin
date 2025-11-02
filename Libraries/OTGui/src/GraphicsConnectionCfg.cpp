// @otlicense
// File: GraphicsConnectionCfg.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsConnectionCfg.h"

#define OT_JSON_Member_Uid "UID"
#define OT_JSON_Member_Shape "Shape"
#define OT_JSON_Member_Style "Style"
#define OT_JSON_Member_SourceUid "Source.UID"
#define OT_JSON_Member_SourcePos "Source.Pos"
#define OT_JSON_Member_SourceName "Source.Name"
#define OT_JSON_Member_HandleState "HandleState"
#define OT_JSON_Member_DestinationUid "Destination.UID"
#define OT_JSON_Member_DestinationPos "Destination.Pos"
#define OT_JSON_Member_DestinationName "Destination.Name"

std::string ot::GraphicsConnectionCfg::shapeToString(ConnectionShape _shape) {
	switch (_shape)
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine: return "DirectLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine: return "SmoothLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::XYLine: return "XYLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::YXLine: return "YXLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine: return "AutoXYLine";
	default:
		OT_LOG_EA("Unknown connection style");
		return "DirectLine";
	}
}

ot::GraphicsConnectionCfg::ConnectionShape ot::GraphicsConnectionCfg::stringToShape(const std::string _shape) {
	if (_shape == shapeToString(ConnectionShape::DirectLine)) return ConnectionShape::DirectLine;
	else if (_shape == shapeToString(ConnectionShape::SmoothLine)) return ConnectionShape::SmoothLine;
	else if (_shape == shapeToString(ConnectionShape::XYLine)) return ConnectionShape::XYLine;
	else if (_shape == shapeToString(ConnectionShape::YXLine)) return ConnectionShape::YXLine;
	else if (_shape == shapeToString(ConnectionShape::AutoXYLine)) return ConnectionShape::AutoXYLine;
	else {
		OT_LOG_EAS("Unknown connection style \"" + _shape + "\"");
		return ConnectionShape::DirectLine;
	}
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg()
	: m_lineShape(ConnectionShape::DirectLine), m_destUID(0), m_originUID(0), m_uid(0), m_handlesState(true)
{
	m_lineStyle.setWidth(2.);
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const ConstJsonObject& _jsonObject) : GraphicsConnectionCfg() {
	setFromJsonObject(_jsonObject);
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const ot::UID& _originUid, const std::string& _originConnectableName, const ot::UID& _destinationUid, const std::string& _destinationName)
	: m_lineShape(ConnectionShape::DirectLine), m_uid(0),
	m_originUID(_originUid), m_originConnectable(_originConnectableName), m_destUID(_destinationUid), m_destConnectable(_destinationName), m_handlesState(true)
{
	m_lineStyle.setWidth(2.);
}

ot::GraphicsConnectionCfg::~GraphicsConnectionCfg() {

}

bool ot::GraphicsConnectionCfg::operator==(const GraphicsConnectionCfg& _other) const
{
	return m_uid == _other.m_uid;
}

void ot::GraphicsConnectionCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Uid, m_uid, _allocator);
	_object.AddMember(OT_JSON_Member_HandleState, m_handlesState, _allocator);
	
	_object.AddMember(OT_JSON_Member_SourceUid, m_originUID, _allocator);
	_object.AddMember(OT_JSON_Member_SourcePos, JsonObject(m_originPos, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_SourceName, JsonString(m_originConnectable, _allocator), _allocator);
	
	_object.AddMember(OT_JSON_Member_DestinationUid, m_destUID, _allocator);
	_object.AddMember(OT_JSON_Member_DestinationPos, JsonObject(m_destPos, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_DestinationName, JsonString(m_destConnectable, _allocator), _allocator);

	_object.AddMember(OT_JSON_Member_Shape, JsonString(this->shapeToString(this->m_lineShape), _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Style, JsonObject(m_lineStyle, _allocator), _allocator);
}

void ot::GraphicsConnectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	m_uid = json::getUInt64(_object, OT_JSON_Member_Uid);
	m_handlesState = json::getBool(_object, OT_JSON_Member_HandleState);

	m_originUID = json::getUInt64(_object, OT_JSON_Member_SourceUid);
	m_originPos.setFromJsonObject(json::getObject(_object, OT_JSON_Member_SourcePos));
	m_originConnectable = json::getString(_object, OT_JSON_Member_SourceName);
	
	m_destUID = json::getUInt64(_object, OT_JSON_Member_DestinationUid);
	m_destPos.setFromJsonObject(json::getObject(_object, OT_JSON_Member_DestinationPos));
	m_destConnectable = json::getString(_object, OT_JSON_Member_DestinationName);
	
	m_lineShape = this->stringToShape(json::getString(_object, OT_JSON_Member_Shape));
	m_lineStyle.setFromJsonObject(json::getObject(_object, OT_JSON_Member_Style));
}

ot::GraphicsConnectionCfg ot::GraphicsConnectionCfg::getReversedConnection(void) const {
	GraphicsConnectionCfg ret(m_destUID, m_destConnectable, m_originUID, m_originConnectable);
	return ret;
}

std::string ot::GraphicsConnectionCfg::createConnectionKey() const
{
	return std::to_string(m_originUID) + m_originConnectable + std::to_string(m_destUID) + m_destConnectable;
}

std::string ot::GraphicsConnectionCfg::createConnectionKeyReverse() const
{
	return std::to_string(m_destUID) + m_destConnectable + std::to_string(m_originUID) + m_originConnectable;
}

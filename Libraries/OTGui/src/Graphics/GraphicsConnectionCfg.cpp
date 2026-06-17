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
#include "OTCore/Logging/Logger.h"
#include "OTGui/Graphics/GraphicsConnectionCfg.h"

std::string ot::GraphicsConnectionCfg::shapeToString(ConnectionShape _shape) {
	switch (_shape)
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine: return "DirectLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine: return "SmoothLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::XYLine: return "XYLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::YXLine: return "YXLine";
	case ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine: return "AutoXYLine";
	default:
		OT_LOG_E("Unknown connection style");
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
		OT_LOG_E("Unknown connection style \"" + _shape + "\"");
		return ConnectionShape::DirectLine;
	}
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg()
	: m_lineShape(ConnectionShape::DirectLine), m_uid(ot::invalidUID), m_handlesState(true)
{
	m_lineStyle.setWidth(2.);
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const ConstJsonObject& _jsonObject) : GraphicsConnectionCfg() {
	setFromJsonObject(_jsonObject);
}

ot::GraphicsConnectionCfg::GraphicsConnectionCfg(const ot::UID& _originUid, const std::string& _originConnectableName, const ot::UID& _destinationUid, const std::string& _destinationName)
	: GraphicsConnectionInfo(_originUid, _originConnectableName, _destinationUid, _destinationName), m_lineShape(ConnectionShape::DirectLine), m_uid(ot::invalidUID), m_handlesState(true)
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
	GraphicsConnectionInfo::addToJsonObject(_object, _allocator);

	_object.AddMember("UID", m_uid, _allocator);
	_object.AddMember("HandleState", m_handlesState, _allocator);
	
	_object.AddMember("OriginPos", JsonObject(m_originPos, _allocator), _allocator);
	_object.AddMember("DestinationPos", JsonObject(m_destPos, _allocator), _allocator);

	_object.AddMember("Shape", JsonString(this->shapeToString(this->m_lineShape), _allocator), _allocator);
	_object.AddMember("Style", JsonObject(m_lineStyle, _allocator), _allocator);
}

void ot::GraphicsConnectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsConnectionInfo::setFromJsonObject(_object);

	m_uid = json::getUInt64(_object, "UID");
	m_handlesState = json::getBool(_object, "HandleState");

	m_originPos.setFromJsonObject(json::getObject(_object, "OriginPos"));
	m_destPos.setFromJsonObject(json::getObject(_object, "DestinationPos"));

	m_lineShape = this->stringToShape(json::getString(_object, "Shape"));
	m_lineStyle.setFromJsonObject(json::getObject(_object, "Style"));
}

ot::GraphicsConnectionCfg ot::GraphicsConnectionCfg::getReversedConnectionCfg() const {
	GraphicsConnectionCfg ret(*this);

	ret.setOriginUid(this->getDestinationUid());
	ret.setOriginConnectable(this->getDestinationConnectable());
	ret.setOriginPos(this->getDestinationPos());

	ret.setDestinationUid(this->getOriginUid());
	ret.setDestinationConnectable(this->getOriginConnectable());
	ret.setDestinationPos(this->getOriginPos());

	return ret;
}

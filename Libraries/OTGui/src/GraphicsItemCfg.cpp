// @otlicense
// File: GraphicsItemCfg.cpp
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
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter2D.h"

// <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
//  <^^\\ '## u ##' \\ ^^^^^^O^                                                                                          ^O^^^^^^// '## w ##' //^^3
//   <^^\\ '## < ##' \\ ^^^^^^O^                                                                                       ^O^^^^^^// '## w ##' //^^3
//   <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
//    <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
//     <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
//      <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '          ^O^^^^^^ // '## o ##' //^^3
//        <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                            ^O^^^^^^  // '## > ##. //^^3
//         < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                              ^O^^^^^^  // .## > ##..//^^ 3
//          <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                      ^O^^^^^^   //.:## > ##:.//^^  3
//         <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                           ^O^^^^^^   //.:## > ##:.//^^   3

#define OT_JSON_MEMBER_Uid "UID"
#define OT_JSON_MEMBER_Name "Name"

#define OT_JSON_MEMBER_Key "Key"
#define OT_JSON_MEMBER_Value "Value"
#define OT_JSON_MEMBER_Title "Title"
#define OT_JSON_MEMBER_Flags "Flags"
#define OT_JSON_MEMBER_Margin "Margin"
#define OT_JSON_MEMBER_ZValue "ZValue"
#define OT_JSON_MEMBER_MinSize "Size.Min"
#define OT_JSON_MEMBER_MaxSize "Size.Max"
#define OT_JSON_MEMBER_ToolTip "ToolTip"
#define OT_JSON_MEMBER_Position "Position"
#define OT_JSON_MEMBER_Alignment "Alignment"
#define OT_JSON_MEMBER_StringMap "StringMap"
#define OT_JSON_MEMBER_Transform "Transform"
#define OT_JSON_MEMBER_SizePolicy "SizePolicy"
#define OT_JSON_MEMBER_ConnectionDirection "ConnectionDirection"
#define OT_JSON_MEMBER_AdditionalTriggerDistance "AddTrigDist"

#define OT_JSON_VALUE_Moveable "Moveable"
#define OT_JSON_VALUE_Connectable "Connectable"
#define OT_JSON_VALUE_Selectable "Selectable"
#define OT_JSON_VALUE_ForwardTooltip "ForwardTooltip"
#define OT_JSON_VALUE_SnapsToGridTopLeft "SnapsToGridTopLeft" 
#define OT_JSON_VALUE_SnapsToGridCenter "SnapsToGridCenter" 
#define OT_JSON_VALUE_TransformEnabled "TransformEnabled" 
#define OT_JSON_VALUE_IgnoreParentTransform "IgnoreParentTransform" 
#define OT_JSON_VALUE_HandleState "HandleState"
#define OT_JSON_VALUE_ForwardState "ForwardState"
#define OT_JSON_VALUE_SilencesNotifications "SilencesNotifications"

ot::GraphicsItemCfg::GraphicsItemCfg()
	: m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(Alignment::Center), m_uid(0),
	m_minSize(0., 0.), m_margins(0., 0., 0., 0.), m_zValue(ot::GraphicsZValues::Item),
	m_maxSize(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()),
	m_sizePolicy(SizePolicy::Preferred), m_connectionDirection(ConnectionDirection::Any)
{}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::GraphicsItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Uid, static_cast<int64_t>(m_uid), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Title, JsonString(m_title, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_ToolTip, JsonString(m_tooltip, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Transform, JsonObject(m_transform, _allocator), _allocator);
	_object.AddMember(JsonString(factoryTypeKey(), _allocator), JsonString(this->getFactoryKey(), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Alignment, JsonString(ot::toString(m_alignment), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_SizePolicy, JsonString(ot::toString(m_sizePolicy), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_ConnectionDirection, JsonString(ot::toString(m_connectionDirection), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Position, JsonObject(m_pos, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_MinSize, JsonObject(m_minSize, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_MaxSize, JsonObject(m_maxSize, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Margin, JsonObject(m_margins, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_AdditionalTriggerDistance, JsonObject(m_additionalTriggerDistance, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_ZValue, m_zValue, _allocator);

	JsonArray flagArr;
	if (m_flags & GraphicsItemCfg::ItemIsMoveable) flagArr.PushBack(JsonString(OT_JSON_VALUE_Moveable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(JsonString(OT_JSON_VALUE_Connectable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIsSelectable) flagArr.PushBack(JsonString(OT_JSON_VALUE_Selectable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemForwardsTooltip) flagArr.PushBack(JsonString(OT_JSON_VALUE_ForwardTooltip, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemSnapsToGridTopLeft) flagArr.PushBack(JsonString(OT_JSON_VALUE_SnapsToGridTopLeft, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemSnapsToGridCenter) flagArr.PushBack(JsonString(OT_JSON_VALUE_SnapsToGridCenter, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemUserTransformEnabled) flagArr.PushBack(JsonString(OT_JSON_VALUE_TransformEnabled, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIgnoresParentTransform) flagArr.PushBack(JsonString(OT_JSON_VALUE_IgnoreParentTransform, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemHandlesState) flagArr.PushBack(JsonString(OT_JSON_VALUE_HandleState, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemForwardsState) flagArr.PushBack(JsonString(OT_JSON_VALUE_ForwardState, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemSilencesNotifcations) flagArr.PushBack(JsonString(OT_JSON_VALUE_SilencesNotifications, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Flags, flagArr, _allocator);

	JsonArray stringMapArr;
	for (const auto& it : m_stringMap) {
		JsonObject stringMapObj;
		stringMapObj.AddMember(OT_JSON_MEMBER_Key, JsonString(it.first, _allocator), _allocator);
		stringMapObj.AddMember(OT_JSON_MEMBER_Value, JsonString(it.second, _allocator), _allocator);
		stringMapArr.PushBack(stringMapObj, _allocator);
	}
	_object.AddMember(OT_JSON_MEMBER_StringMap, stringMapArr, _allocator);
}

void ot::GraphicsItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	m_uid = static_cast<ot::UID>(json::getInt64(_object, OT_JSON_MEMBER_Uid));
	m_name = json::getString(_object, OT_JSON_MEMBER_Name);
	m_title = json::getString(_object, OT_JSON_MEMBER_Title);
	m_tooltip = json::getString(_object, OT_JSON_MEMBER_ToolTip);
	m_alignment = stringToAlignment(json::getString(_object, OT_JSON_MEMBER_Alignment));
	m_sizePolicy = stringToSizePolicy(json::getString(_object, OT_JSON_MEMBER_SizePolicy));
	m_connectionDirection = stringToConnectionDirection(json::getString(_object, OT_JSON_MEMBER_ConnectionDirection));
	m_zValue = json::getInt(_object, OT_JSON_MEMBER_ZValue, GraphicsZValues::Item);

	m_pos.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Position));
	m_margins.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Margin));
	m_minSize.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_MinSize));
	m_maxSize.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_MaxSize));
	m_additionalTriggerDistance.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_AdditionalTriggerDistance));

	m_flags = NoFlags;
	std::list<std::string> flagsArr = json::getStringList(_object, OT_JSON_MEMBER_Flags);
	for (auto f : flagsArr) {
		if (f == OT_JSON_VALUE_Moveable) m_flags |= ItemIsMoveable;
		else if (f == OT_JSON_VALUE_Connectable) m_flags |= ItemIsConnectable;
		else if (f == OT_JSON_VALUE_Selectable) m_flags |= ItemIsSelectable;
		else if (f == OT_JSON_VALUE_ForwardTooltip) m_flags |= ItemForwardsTooltip;
		else if (f == OT_JSON_VALUE_SnapsToGridTopLeft) m_flags |= ItemSnapsToGridTopLeft;
		else if (f == OT_JSON_VALUE_SnapsToGridCenter) m_flags |= ItemSnapsToGridCenter;
		else if (f == OT_JSON_VALUE_TransformEnabled) m_flags |= ItemUserTransformEnabled;
		else if (f == OT_JSON_VALUE_IgnoreParentTransform) m_flags |= ItemIgnoresParentTransform;
		else if (f == OT_JSON_VALUE_HandleState) m_flags |= ItemHandlesState;
		else if (f == OT_JSON_VALUE_ForwardState) m_flags |= ItemForwardsState;
		else if (f == OT_JSON_VALUE_SilencesNotifications) m_flags |= ItemSilencesNotifcations;
		else {
			OT_LOG_EAS("Unknown GraphicsItemFlag \"" + f + "\"");
		}
	}

	m_stringMap.clear();
	ConstJsonObjectList stringMapArr = json::getObjectList(_object, OT_JSON_MEMBER_StringMap);
	for (const ConstJsonObject& stringMapObj : stringMapArr) {
		std::string k = json::getString(stringMapObj, OT_JSON_MEMBER_Key);
		std::string v = json::getString(stringMapObj, OT_JSON_MEMBER_Value);
		m_stringMap.insert_or_assign(k, v);
	}

	m_transform.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Transform));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsItemCfg::setFixedSize(const Size2DD& _size) {
	this->setMinimumSize(_size);
	this->setMaximumSize(_size);
}

std::string ot::GraphicsItemCfg::getStringForKey(const std::string& _key) const {
	const auto& it = m_stringMap.find(_key);
	if (it == m_stringMap.end()) {
		return "#" + _key;
	}
	else {
		return it->second;
	}
}

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
#include "OTCore/Logging/Logger.h"
#include "OTGui/Graphics/GraphicsItemCfg.h"
#include "OTGui/Graphics/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter/Painter2D.h"

//    <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
//      <^^\\ '## u ##' \\ ^^^^^^O^                                                                                          ^O^^^^^^// '## w ##' //^^3
//        <^^\\ '## < ##' \\ ^^^^^^O^                                                                                       ^O^^^^^^// '## w ##' //^^3
//        <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
//       <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
//      <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
//     <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '          ^O^^^^^^ // '## o ##' //^^3
//    <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                            ^O^^^^^^  // '## > ##. //^^3
//   < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                              ^O^^^^^^  // .## > ##..//^^ 3
//  <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                      ^O^^^^^^   //.:## > ##:.//^^  3
// <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                           ^O^^^^^^   //.:## > ##:.//^^   3

ot::GraphicsItemCfg::GraphicsItemCfg()
	: m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(Alignment::Center), m_uid(ot::invalidUID),
	m_minSize(0., 0.), m_margins(0., 0., 0., 0.), m_zValue(ot::GraphicsZValues::Item),
	m_maxSize(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()),
	m_sizePolicy(SizePolicy::Preferred), m_connectionDirection(ConnectionDirection::Any)
{}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::GraphicsItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("UID", static_cast<int64_t>(m_uid), _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("ToolTip", JsonString(m_tooltip, _allocator), _allocator);
	_object.AddMember("Transform", JsonObject(m_transform, _allocator), _allocator);
	_object.AddMember(JsonString(factoryTypeKey(), _allocator), JsonString(this->getFactoryKey(), _allocator), _allocator);
	_object.AddMember("Align", JsonString(ot::toString(m_alignment), _allocator), _allocator);
	_object.AddMember("SizePolicy", JsonString(ot::toString(m_sizePolicy), _allocator), _allocator);
	_object.AddMember("ConnectionDir", JsonString(ot::toString(m_connectionDirection), _allocator), _allocator);

	_object.AddMember("Pos", JsonObject(m_pos, _allocator), _allocator);
	_object.AddMember("MinSize", JsonObject(m_minSize, _allocator), _allocator);
	_object.AddMember("MaxSize", JsonObject(m_maxSize, _allocator), _allocator);
	_object.AddMember("Margins", JsonObject(m_margins, _allocator), _allocator);
	_object.AddMember("AdditionalTriggerDist", JsonObject(m_additionalTriggerDistance, _allocator), _allocator);
	_object.AddMember("ZVal", m_zValue, _allocator);
	_object.AddMember("Flags", static_cast<uint64_t>(m_flags), _allocator);

	JsonArray stringMapArr;
	for (const auto& it : m_stringMap) {
		JsonObject stringMapObj;
		stringMapObj.AddMember("K", JsonString(it.first, _allocator), _allocator);
		stringMapObj.AddMember("V", JsonString(it.second, _allocator), _allocator);
		stringMapArr.PushBack(stringMapObj, _allocator);
	}
	_object.AddMember("StringMap", stringMapArr, _allocator);
}

void ot::GraphicsItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	m_uid = static_cast<ot::UID>(json::getInt64(_object, "UID"));
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_tooltip = json::getString(_object, "ToolTip");
	m_transform.setFromJsonObject(json::getObject(_object, "Transform"));
	m_alignment = stringToAlignment(json::getString(_object, "Align"));
	m_sizePolicy = stringToSizePolicy(json::getString(_object, "SizePolicy"));
	m_connectionDirection = stringToConnectionDirection(json::getString(_object, "ConnectionDir"));
	
	m_pos.setFromJsonObject(json::getObject(_object, "Pos"));
	m_minSize.setFromJsonObject(json::getObject(_object, "MinSize"));
	m_maxSize.setFromJsonObject(json::getObject(_object, "MaxSize"));
	m_margins.setFromJsonObject(json::getObject(_object, "Margins"));
	m_additionalTriggerDistance.setFromJsonObject(json::getObject(_object, "AdditionalTriggerDist"));
	m_zValue = json::getInt(_object, "ZVal", GraphicsZValues::Item);
	m_flags = GraphicsItemFlags(json::getUInt64(_object, "Flags"));

	m_stringMap.clear();
	ConstJsonObjectList stringMapArr = json::getObjectList(_object, "StringMap");
	for (const ConstJsonObject& stringMapObj : stringMapArr) {
		std::string k = json::getString(stringMapObj, "K");
		std::string v = json::getString(stringMapObj, "V");
		m_stringMap.insert_or_assign(k, v);
	}

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

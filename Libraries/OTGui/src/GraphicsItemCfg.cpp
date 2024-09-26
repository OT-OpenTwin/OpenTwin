//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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
#define OT_JSON_VALUE_SnapsToGrid "SnapsToGrid" 
#define OT_JSON_VALUE_TransformEnabled "TransformEnabled" 
#define OT_JSON_VALUE_IgnoreParentTransform "IgnoreParentTransform" 
#define OT_JSON_VALUE_HandleState "HandleState"
#define OT_JSON_VALUE_ForwardState "ForwardState"

ot::GraphicsItemCfg::GraphicsItemCfg()
	: m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(ot::AlignCenter), m_uid(0),
	m_minSize(0., 0.), m_margins(0., 0., 0., 0.), m_maxSize(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()), m_sizePolicy(ot::Preferred), m_connectionDirection(ot::ConnectAny)
{}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::GraphicsItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {

	// !!!!!     DON'T FORGET copyConfigDataToItem WHEN ADDING MEMBER ;-)     !!!!!

	JsonObject posObj;
	m_pos.addToJsonObject(posObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Position, posObj, _allocator);

	JsonObject minSizeObj;
	m_minSize.addToJsonObject(minSizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_MinSize, minSizeObj, _allocator);

	JsonObject maxSizeObj;
	m_maxSize.addToJsonObject(maxSizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_MaxSize, maxSizeObj, _allocator);

	JsonObject marginObj;
	m_margins.addToJsonObject(marginObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Margin, marginObj, _allocator);

	JsonObject addTrigDistObj;
	m_additionalTriggerDistance.addToJsonObject(addTrigDistObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_AdditionalTriggerDistance, addTrigDistObj, _allocator);

	JsonArray flagArr;
	if (m_flags & GraphicsItemCfg::ItemIsMoveable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Moveable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Connectable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIsSelectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Selectable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemForwardsTooltip) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_ForwardTooltip, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemSnapsToGrid) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_SnapsToGrid, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemUserTransformEnabled) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_TransformEnabled, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIgnoresParentTransform) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_IgnoreParentTransform, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemHandlesState) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_HandleState, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemForwardsState) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_ForwardState, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Flags, flagArr, _allocator);

	JsonArray stringMapArr;
	for (const auto& it : m_stringMap) {
		JsonObject stringMapObj;
		stringMapObj.AddMember(OT_JSON_MEMBER_Key, JsonString(it.first, _allocator), _allocator);
		stringMapObj.AddMember(OT_JSON_MEMBER_Value, JsonString(it.second, _allocator), _allocator);
		stringMapArr.PushBack(stringMapObj, _allocator);
	}
	_object.AddMember(OT_JSON_MEMBER_StringMap, stringMapArr, _allocator);

	JsonObject transformObj;
	m_transform.addToJsonObject(transformObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Transform, transformObj, _allocator);

	_object.AddMember(OT_JSON_MEMBER_Uid, static_cast<int64_t>(m_uid), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Title, JsonString(m_title, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_ToolTip, JsonString(m_tooltip, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_GraphicsItemCfgType, JsonString(this->getFactoryKey(), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Alignment, JsonString(ot::toString(m_alignment), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_SizePolicy, JsonString(ot::toString(m_sizePolicy), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_ConnectionDirection, JsonString(ot::toString(m_connectionDirection), _allocator), _allocator);
}

void ot::GraphicsItemCfg::setFromJsonObject(const ConstJsonObject& _object) {

	// !!!!!     DON'T FORGET copyConfigDataToItem WHEN ADDING MEMBER ;-)     !!!!!

	m_uid = static_cast<ot::UID>(json::getInt64(_object, OT_JSON_MEMBER_Uid));
	m_name = json::getString(_object, OT_JSON_MEMBER_Name);
	m_title = json::getString(_object, OT_JSON_MEMBER_Title);
	m_tooltip = json::getString(_object, OT_JSON_MEMBER_ToolTip);
	m_alignment = stringToAlignment(json::getString(_object, OT_JSON_MEMBER_Alignment));
	m_sizePolicy = stringToSizePolicy(json::getString(_object, OT_JSON_MEMBER_SizePolicy));
	m_connectionDirection = stringToConnectionDirection(json::getString(_object, OT_JSON_MEMBER_ConnectionDirection));

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
		else if (f == OT_JSON_VALUE_SnapsToGrid) m_flags |= ItemSnapsToGrid;
		else if (f == OT_JSON_VALUE_TransformEnabled) m_flags |= ItemUserTransformEnabled;
		else if (f == OT_JSON_VALUE_IgnoreParentTransform) m_flags |= ItemIgnoresParentTransform;
		else if (f == OT_JSON_VALUE_HandleState) m_flags |= ItemHandlesState;
		else if (f == OT_JSON_VALUE_ForwardState) m_flags |= ItemForwardsState;
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
	if (it == m_stringMap.end()) return "#" + _key;
	else return it->second;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

void ot::GraphicsItemCfg::copyConfigDataToItem(GraphicsItemCfg* _target) const {
	_target->m_name = m_name;
	_target->m_title = m_title;
	_target->m_uid = m_uid;
	_target->m_tooltip = m_tooltip;
	_target->m_pos = m_pos;
	_target->m_additionalTriggerDistance = m_additionalTriggerDistance;

	_target->m_minSize = m_minSize;
	_target->m_maxSize = m_maxSize;

	_target->m_margins = m_margins;
	_target->m_flags = m_flags;
	_target->m_alignment = m_alignment;
	_target->m_sizePolicy = m_sizePolicy;
	_target->m_connectionDirection = m_connectionDirection;

	_target->m_stringMap = m_stringMap;
	_target->m_transform = m_transform;
}

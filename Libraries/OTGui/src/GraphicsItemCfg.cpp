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

#define OT_JSON_MEMBER_Title "Title"
#define OT_JSON_MEMBER_Flags "Flags"
#define OT_JSON_MEMBER_Margin "Margin"
#define OT_JSON_MEMBER_MinSize "Size.Min"
#define OT_JSON_MEMBER_MaxSize "Size.Max"
#define OT_JSON_MEMBER_ToolTip "ToolTip"
#define OT_JSON_MEMBER_Position "Position"
#define OT_JSON_MEMBER_Alignment "Alignment"
#define OT_JSON_MEMBER_SizePolicy "SizePolicy"
#define OT_JSON_MEMBER_ConnectionDirection "ConnectionDirection"

#define OT_JSON_VALUE_Moveable "Moveable"
#define OT_JSON_VALUE_Connectable "Connectable"
#define OT_JSON_VALUE_ForwardTooltip "ForwardTooltip"
#define OT_JSON_VALUE_SnapsToGrid "SnapsToGrid" 
#define OT_JSON_VALUE_NoFeedback "NoFeedback"

ot::GraphicsItemCfg::GraphicsItemCfg()
	: m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(ot::AlignCenter), m_uid(0),
	m_minSize(0., 0.), m_margins(0., 0., 0., 0.), m_maxSize(DBL_MAX, DBL_MAX), m_sizePolicy(ot::Preferred), m_connectionDirection(ot::ConnectAny)
{}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::GraphicsItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
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

	JsonArray flagArr;
	if (m_flags & GraphicsItemCfg::ItemIsMoveable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Moveable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Connectable, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemForwardsTooltip) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_ForwardTooltip, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemSnapsToGrid) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_SnapsToGrid, _allocator), _allocator);
	if (m_flags & GraphicsItemCfg::ItemHasNoFeedback) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_NoFeedback, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Flags, flagArr, _allocator);

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

	m_flags = NoFlags;
	std::list<std::string> flagsArr = json::getStringList(_object, OT_JSON_MEMBER_Flags);
	for (auto f : flagsArr) {
		if (f == OT_JSON_VALUE_Moveable) m_flags |= ItemIsMoveable;
		else if (f == OT_JSON_VALUE_Connectable) m_flags |= ItemIsConnectable;
		else if (f == OT_JSON_VALUE_ForwardTooltip) m_flags |= ItemForwardsTooltip;
		else if (f == OT_JSON_VALUE_SnapsToGrid) m_flags |= ItemSnapsToGrid;
		else if (f == OT_JSON_VALUE_NoFeedback) m_flags |= ItemHasNoFeedback;
		else {
			OT_LOG_EAS("Unknown GraphicsItemFlag \"" + f + "\"");
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

void ot::GraphicsItemCfg::setupData(GraphicsItemCfg* _config) const {
	_config->m_name = m_name;
	_config->m_title = m_title;
	_config->m_uid = m_uid;
	_config->m_tooltip = m_tooltip;
	_config->m_pos = m_pos;
	_config->m_minSize = m_minSize;
	_config->m_maxSize = m_maxSize;
	_config->m_margins = m_margins;
	_config->m_flags = m_flags;
	_config->m_alignment = m_alignment;
	_config->m_sizePolicy = m_sizePolicy;
	_config->m_connectionDirection = m_connectionDirection;
}

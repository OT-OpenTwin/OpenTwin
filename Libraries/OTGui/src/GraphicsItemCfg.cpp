//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTCore/rJSON.h"
#include "OTCore/rJSONHelper.h"

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

ot::GraphicsItemCfg::GraphicsItemCfg()
	: m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(ot::AlignCenter), 
	m_minSize(0., 0.), m_maxSize(DBL_MAX, DBL_MAX), m_sizePolicy(ot::Preferred), m_connectionDirection(ot::ConnectAny)
{}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

void ot::GraphicsItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueObject(posObj);
	m_pos.addToJsonObject(_document, posObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Position, posObj);

	OT_rJSON_createValueObject(minSizeObj);
	m_minSize.addToJsonObject(_document, minSizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_MinSize, minSizeObj);

	OT_rJSON_createValueObject(maxSizeObj);
	m_maxSize.addToJsonObject(_document, maxSizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_MaxSize, maxSizeObj);

	OT_rJSON_createValueObject(marginObj);
	m_margins.addToJsonObject(_document, marginObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Margin, marginObj);

	OT_rJSON_createValueArray(flagArr);
	if (m_flags & GraphicsItemCfg::ItemIsMoveable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Moveable, _document.GetAllocator()), _document.GetAllocator());
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Connectable, _document.GetAllocator()), _document.GetAllocator());
	if (m_flags & GraphicsItemCfg::ItemForwardsTooltip) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_ForwardTooltip, _document.GetAllocator()), _document.GetAllocator());
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Flags, flagArr);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Uid, m_uid);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Title, m_tile);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ToolTip, m_tooltip);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Alignment, ot::toString(m_alignment));
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_SizePolicy, ot::toString(m_sizePolicy));
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ConnectionDirection, ot::toString(m_connectionDirection));
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, this->simpleFactoryObjectKey());
}

void ot::GraphicsItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Uid, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ToolTip, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Position, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_MinSize, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_MaxSize, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Margin, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Flags, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Alignment, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_SizePolicy, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ConnectionDirection, String);

	m_uid = _object[OT_JSON_MEMBER_Uid].GetString();
	m_name = _object[OT_JSON_MEMBER_Name].GetString();
	m_tile = _object[OT_JSON_MEMBER_Title].GetString();
	m_tooltip = _object[OT_JSON_MEMBER_ToolTip].GetString();
	m_alignment = ot::stringToAlignment(_object[OT_JSON_MEMBER_Alignment].GetString());
	m_sizePolicy = ot::stringToSizePolicy(_object[OT_JSON_MEMBER_SizePolicy].GetString());
	m_connectionDirection = ot::stringToConnectionDirection(_object[OT_JSON_MEMBER_ConnectionDirection].GetString());

	OT_rJSON_val posObj = _object[OT_JSON_MEMBER_Position].GetObject();
	OT_rJSON_val marginObj = _object[OT_JSON_MEMBER_Margin].GetObject();
	OT_rJSON_val minSizeObj = _object[OT_JSON_MEMBER_MinSize].GetObject();
	OT_rJSON_val maxSizeObj = _object[OT_JSON_MEMBER_MaxSize].GetObject();

	m_pos.setFromJsonObject(posObj);
	m_margins.setFromJsonObject(marginObj);
	m_minSize.setFromJsonObject(minSizeObj);
	m_maxSize.setFromJsonObject(maxSizeObj);

	m_flags = NoFlags;
	OT_rJSON_val flagArr = _object[OT_JSON_MEMBER_Flags].GetArray();
	for (rapidjson::SizeType i = 0; i < flagArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(flagArr, i, String);
		std::string f = flagArr[i].GetString();
		if (f == OT_JSON_VALUE_Moveable) m_flags |= ItemIsMoveable;
		else if (f == OT_JSON_VALUE_Connectable) m_flags |= ItemIsConnectable;
		else if (f == OT_JSON_VALUE_ForwardTooltip) m_flags |= ItemForwardsTooltip;
		else {
			OT_LOG_EA("Unknown GraphicsItem flag");
		}
	}
}

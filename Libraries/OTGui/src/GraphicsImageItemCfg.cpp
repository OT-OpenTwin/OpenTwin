//! @file GraphicsImageItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"

OT_RegisterSimpleFactoryObject(ot::GraphicsImageItemCfg, "GraphicsImageItemCfg");

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const std::string& _imageSubPath) : m_imageSubPath(_imageSubPath) {}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, "GraphicsTextItemCfg");

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ImagePath, m_imageSubPath);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ImagePath, String);
	m_imageSubPath = _object[OT_JSON_MEMBER_ImagePath].GetString();
}
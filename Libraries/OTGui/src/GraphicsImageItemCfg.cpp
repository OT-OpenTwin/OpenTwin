//! @file GraphicsImageItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"
#define OT_JSON_MEMBER_ColorMask "ColorMask"
#define OT_JSON_MEMBER_MaintainAspectRatio "MaintainAspectRatio"

static ot::SimpleFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg);

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const std::string& _imageSubPath)
	: m_imageSubPath(_imageSubPath), m_maintainAspectRatio(false), m_colorMask(-1.f, -1.f, -1.f, -1.f)
{}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ImagePath, m_imageSubPath);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_MaintainAspectRatio, m_maintainAspectRatio);

	OT_rJSON_createValueObject(maskObj);
	m_colorMask.addToJsonObject(_document, maskObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ColorMask, maskObj);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ImagePath, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ColorMask, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_MaintainAspectRatio, Bool);

	m_imageSubPath = _object[OT_JSON_MEMBER_ImagePath].GetString();
	m_maintainAspectRatio = _object[OT_JSON_MEMBER_MaintainAspectRatio].GetBool();

	OT_rJSON_val maskObj = _object[OT_JSON_MEMBER_ColorMask].GetObject();
	m_colorMask.setFromJsonObject(maskObj);
}

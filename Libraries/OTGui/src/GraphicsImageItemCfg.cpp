//! @file GraphicsImageItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"
#define OT_JSON_MEMBER_ColorMask "ColorMask"
#define OT_JSON_MEMBER_MaintainAspectRatio "MaintainAspectRatio"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_FactoryKey_GraphicsImageItem);

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const std::string& _imageSubPath)
	: m_imageSubPath(_imageSubPath), m_maintainAspectRatio(false), m_colorMask(-255, -255, -255, -255)
{}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_ImagePath, JsonString(m_imageSubPath, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_MaintainAspectRatio, m_maintainAspectRatio, _allocator);

	JsonObject maskObj;
	m_colorMask.addToJsonObject(maskObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_ColorMask, maskObj, _allocator);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	m_imageSubPath = json::getString(_object, OT_JSON_MEMBER_ImagePath);
	m_maintainAspectRatio = json::getBool(_object, OT_JSON_MEMBER_MaintainAspectRatio);
	m_colorMask.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_ColorMask));
}

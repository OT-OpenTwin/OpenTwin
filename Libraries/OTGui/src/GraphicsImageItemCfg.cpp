//! @file GraphicsImageItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"
#define OT_JSON_MEMBER_MaintainAspectRatio "MaintainAspectRatio"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_FactoryKey_GraphicsImageItem);

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const std::string& _imageSubPath)
	: m_imageSubPath(_imageSubPath), m_maintainAspectRatio(false)
{}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

ot::GraphicsItemCfg* ot::GraphicsImageItemCfg::createCopy(void) const {
	ot::GraphicsImageItemCfg* copy = new GraphicsImageItemCfg;
	this->copyConfigDataToItem(copy);

	copy->m_imageSubPath = m_imageSubPath;
	copy->m_maintainAspectRatio = m_maintainAspectRatio;
	
	return copy;
}

void ot::GraphicsImageItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_ImagePath, JsonString(m_imageSubPath, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_MaintainAspectRatio, m_maintainAspectRatio, _allocator);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	m_imageSubPath = json::getString(_object, OT_JSON_MEMBER_ImagePath);
	m_maintainAspectRatio = json::getBool(_object, OT_JSON_MEMBER_MaintainAspectRatio);
}

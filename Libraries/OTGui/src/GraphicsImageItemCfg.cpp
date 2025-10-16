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

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(ot::GraphicsImageItemCfg::className());

ot::GraphicsImageItemCfg::GraphicsImageItemCfg()
	: m_maintainAspectRatio(false), m_imageDataFileType(ImageFileFormat::PNG)
{}

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const GraphicsImageItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_imageData(_other.m_imageData), m_imageSubPath(_other.m_imageSubPath),
	m_maintainAspectRatio(_other.m_maintainAspectRatio), m_imageDataFileType(_other.m_imageDataFileType)
{}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Data", JsonString(m_imageData, _allocator), _allocator);
	_object.AddMember("DataFileFormat", JsonString(ot::toString(m_imageDataFileType), _allocator), _allocator);
	_object.AddMember("Path", JsonString(m_imageSubPath, _allocator), _allocator);
	_object.AddMember("MaintainRatio", m_maintainAspectRatio, _allocator);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	m_imageData = json::getString(_object, "Data");
	m_imageDataFileType = ot::stringToImageFileFormat(json::getString(_object, "DataFileFormat"));
	m_imageSubPath = json::getString(_object, "Path");
	m_maintainAspectRatio = json::getBool(_object, "MaintainRatio");
}

// @otlicense

//! @file GraphicsImageItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"
#define OT_JSON_MEMBER_MaintainAspectRatio "MaintainAspectRatio"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(ot::GraphicsImageItemCfg::className());

ot::GraphicsImageItemCfg::GraphicsImageItemCfg()
	: m_maintainAspectRatio(false), m_imageDataFileType(ImageFileFormat::PNG), m_borderPen(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder))
{
	m_borderPen.setStyle(LineStyle::NoLine);
}

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const GraphicsImageItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_imageData(_other.m_imageData), m_imageSubPath(_other.m_imageSubPath),
	m_maintainAspectRatio(_other.m_maintainAspectRatio), m_imageDataFileType(_other.m_imageDataFileType),
	m_borderPen(_other.m_borderPen)
{}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Data", JsonString(String::compressedVectorBase64(m_imageData), _allocator), _allocator);
	_object.AddMember("DataLen", m_imageData.size(), _allocator);
	_object.AddMember("DataFileFormat", JsonString(ot::toString(m_imageDataFileType), _allocator), _allocator);
	_object.AddMember("Path", JsonString(m_imageSubPath, _allocator), _allocator);
	_object.AddMember("MaintainRatio", m_maintainAspectRatio, _allocator);
	_object.AddMember("BorderPen", JsonObject(m_borderPen, _allocator), _allocator);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	uint64_t dataLen = json::getUInt64(_object, "DataLen");
	m_imageData = String::decompressedVectorBase64(json::getString(_object, "Data"), dataLen);
	m_imageDataFileType = ot::stringToImageFileFormat(json::getString(_object, "DataFileFormat"));
	m_imageSubPath = json::getString(_object, "Path");
	m_maintainAspectRatio = json::getBool(_object, "MaintainRatio");
	m_borderPen.setFromJsonObject(json::getObject(_object, "BorderPen"));
}

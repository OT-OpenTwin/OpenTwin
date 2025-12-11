// @otlicense
// File: GraphicsImageItemCfg.cpp
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

	std::unique_ptr<char> compressed(String::compressBase64(reinterpret_cast<const uint8_t*>(m_imageData.data()), static_cast<uint64_t>(m_imageData.size())));
	_object.AddMember("Data", JsonString(std::string(compressed.get()), _allocator), _allocator);
	_object.AddMember("DataLen", m_imageData.size(), _allocator);
	_object.AddMember("DataFileFormat", JsonString(ot::toString(m_imageDataFileType), _allocator), _allocator);
	_object.AddMember("Path", JsonString(m_imageSubPath, _allocator), _allocator);
	_object.AddMember("MaintainRatio", m_maintainAspectRatio, _allocator);
	_object.AddMember("BorderPen", JsonObject(m_borderPen, _allocator), _allocator);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	
	std::string data = json::getString(_object, "Data");
	uint64_t decompressedSize = json::getUInt64(_object, "DataLen");
	std::unique_ptr<uint8_t> decompressed(String::decompressBase64(data.c_str(), decompressedSize));
	m_imageData = std::vector<char>(reinterpret_cast<char*>(decompressed.get()), reinterpret_cast<char*>(decompressed.get()) + decompressedSize);
	m_imageDataFileType = ot::stringToImageFileFormat(json::getString(_object, "DataFileFormat"));
	m_imageSubPath = json::getString(_object, "Path");
	m_maintainAspectRatio = json::getBool(_object, "MaintainRatio");
	m_borderPen.setFromJsonObject(json::getObject(_object, "BorderPen"));
}

// @otlicense
// File: ExtendedProjectInformation.cpp
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
#include "OTGui/ExtendedProjectInformation.h"

ot::ExtendedProjectInformation::ExtendedProjectInformation() 
	: m_imageFormat(ImageFileFormat::PNG), m_descriptionSyntax(DocumentSyntax::PlainText)
{

}

ot::ExtendedProjectInformation::ExtendedProjectInformation(const ProjectInformation& _basicInfo) 
	: ProjectInformation(_basicInfo), m_imageFormat(ImageFileFormat::PNG), m_descriptionSyntax(DocumentSyntax::PlainText)
{}

void ot::ExtendedProjectInformation::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	ProjectInformation::addToJsonObject(_jsonObject, _allocator);

	_jsonObject.AddMember("ImageData", JsonString(String::compressedVectorBase64(m_imageData), _allocator), _allocator);
	_jsonObject.AddMember("ImageDataLength", static_cast<uint64_t>(m_imageData.size()), _allocator);
	_jsonObject.AddMember("ImageFormat", JsonString(toString(m_imageFormat), _allocator), _allocator);
	
	_jsonObject.AddMember("Description", JsonString(m_description, _allocator), _allocator);
	_jsonObject.AddMember("DescriptionSyntax", JsonString(toString(m_descriptionSyntax), _allocator), _allocator);
}

void ot::ExtendedProjectInformation::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	ProjectInformation::setFromJsonObject(_jsonObject);

	std::string data = json::getString(_jsonObject, "ImageData");
	m_imageData = String::decompressedVectorBase64(data, json::getUInt64(_jsonObject, "ImageDataLength"));
	m_imageFormat = stringToImageFileFormat(json::getString(_jsonObject, "ImageFormat"));
	
	m_description = json::getString(_jsonObject, "Description");
	m_descriptionSyntax = stringToDocumentSyntax(json::getString(_jsonObject, "DescriptionSyntax"));
}

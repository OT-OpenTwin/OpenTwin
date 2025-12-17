// @otlicense
// File: GridFSFileInfo.cpp
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
#include "GridFSFileInfo.h"

ot::GridFSFileInfo::GridFSFileInfo(const ConstJsonObject& _jsonObject) : GridFSFileInfo()
{
	setFromJsonObject(_jsonObject);
}

void ot::GridFSFileInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Collection", JsonString(m_collectionName, _allocator), _allocator);
	_jsonObject.AddMember("ID", JsonString(m_documentId, _allocator), _allocator);
	_jsonObject.AddMember("IsCompressed", m_isCompressed, _allocator);
	_jsonObject.AddMember("UncompressedSize", static_cast<uint64_t>(m_uncompressedSize), _allocator);
}

void ot::GridFSFileInfo::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_collectionName = json::getString(_jsonObject, "Collection");
	m_documentId = json::getString(_jsonObject, "ID");
	m_isCompressed = json::getBool(_jsonObject, "IsCompressed", false);
	m_uncompressedSize = static_cast<size_t>(json::getUInt64(_jsonObject, "UncompressedSize", 0));
}

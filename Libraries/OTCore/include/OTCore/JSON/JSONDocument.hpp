// @otlicense
// File: JSONDocument.hpp
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

#pragma once

// OpenTwin header
#include "OTCore/JSON/JSONDocument.h"

inline constexpr rapidjson::Type ot::JsonDocument::getRapidJsonType(JsonDocumentType _type) {
	switch (_type) {
	case JsonDocumentType::ObjectType: return rapidjson::kObjectType;
	case JsonDocumentType::ArrayType:  return rapidjson::kArrayType;
	case JsonDocumentType::NullType:   OT_FALLTHROUGH
	default:                           return rapidjson::kNullType;
	}
}

inline ot::JsonDocument::JsonDocument() : rapidjson::Document(rapidjson::kObjectType) {};

inline ot::JsonDocument::JsonDocument(rapidjson::Type _type) : rapidjson::Document(_type) {};

inline ot::JsonDocument::JsonDocument(JsonDocumentType _type) : rapidjson::Document(getRapidJsonType(_type)) {};

inline bool ot::JsonDocument::fromJson(const char* _fromJson) {
	this->SetNull();
	this->Parse(_fromJson);
	return (this->GetType() == rapidjson::kObjectType) || (this->GetType() == rapidjson::kArrayType);
}

inline std::string ot::JsonDocument::toJson() const {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	this->Accept(writer);

	// Return string
	return std::string(buffer.GetString());
}
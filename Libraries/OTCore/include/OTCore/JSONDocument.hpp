// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/JSONDocument.h"

inline ot::JsonDocument::JsonDocument() : rapidjson::Document(rapidjson::kObjectType) {};

inline ot::JsonDocument::JsonDocument(rapidjson::Type _type) : rapidjson::Document(_type) {};

inline bool ot::JsonDocument::fromJson(const char* _fromJson) {
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
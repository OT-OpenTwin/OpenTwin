//! @file JSONDocument.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSONDocument.h"

ot::JsonDocument::JsonDocument() : rapidjson::Document(rapidjson::kObjectType) {};

ot::JsonDocument::JsonDocument(rapidjson::Type _type) : rapidjson::Document(_type) {};

bool ot::JsonDocument::fromJson(const char* _fromJson) {
	this->Parse(_fromJson);
	return (this->GetType() == rapidjson::kObjectType) || (this->GetType() == rapidjson::kArrayType);
}

std::string ot::JsonDocument::toJson(void) const {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	this->Accept(writer);

	// Return string
	return std::string(buffer.GetString());
}
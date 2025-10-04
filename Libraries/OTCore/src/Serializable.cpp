//! @file Serializable.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Serializable.h"

std::string ot::Serializable::toJson() const {
	ot::JsonDocument doc;
	this->addToJsonObject(doc, doc.GetAllocator());
	return doc.toJson();
}
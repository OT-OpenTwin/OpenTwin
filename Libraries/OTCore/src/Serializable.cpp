// @otlicense

// OpenTwin header
#include "OTCore/Serializable.h"

std::string ot::Serializable::toJson() const {
	ot::JsonDocument doc;
	this->addToJsonObject(doc, doc.GetAllocator());
	return doc.toJson();
}
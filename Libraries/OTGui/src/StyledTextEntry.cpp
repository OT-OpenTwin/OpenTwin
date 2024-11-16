//! @file StyledTextEntry.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/StyledTextEntry.h"

ot::StyledTextEntry::StyledTextEntry(const StyledTextStyle& _style)
	: m_style(_style) 
{

}

ot::StyledTextEntry::StyledTextEntry(const ot::ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

void ot::StyledTextEntry::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject styleObj;
	m_style.addToJsonObject(styleObj, _allocator);
	_object.AddMember("Style", styleObj, _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::StyledTextEntry::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_style.setFromJsonObject(json::getObject(_object, "Style"));
	m_text = json::getString(_object, "Text");
}

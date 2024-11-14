//! @file StyledTextBuilder.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/StyledTextBuilder.h"

ot::StyledTextBuilder::StyledTextBuilder() {
	m_entries.push_back(StyledTextEntry());
}

ot::StyledTextBuilder::StyledTextBuilder(const ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

void ot::StyledTextBuilder::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray entriesArray;
	for (const StyledTextEntry& entry : m_entries) {
		JsonObject entryObject;
		entry.addToJsonObject(entryObject, _allocator);
		entriesArray.PushBack(entryObject, _allocator);
	}
	_object.AddMember("Entries", entriesArray, _allocator);
}

void ot::StyledTextBuilder::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entries.clear();
	ConstJsonObjectList entriesArray = json::getObjectList(_object, "Entries");
	for (const ConstJsonObject& entryObject : entriesArray) {
		m_entries.push_back(StyledTextEntry(entryObject));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Builder

ot::StyledTextBuilder& ot::StyledTextBuilder::operator<<(const char* _text) {
	return this->operator<<(std::string(_text));
}

ot::StyledTextBuilder& ot::StyledTextBuilder::operator<<(const std::string& _text) {
	m_entries.back().getText().append(_text);
	return *this;
}

ot::StyledTextBuilder& ot::StyledTextBuilder::operator<<(StyledText::ColorReference _colorReference) {
	// Ignore applying same color
	if (m_entries.back().getStyle().getColorReference() != _colorReference) {
		// Setup new style
		StyledTextStyle newStyle = m_entries.back().getStyle();
		newStyle.setColorReference(_colorReference);

		// Apply new style
		this->applyNextStyle(newStyle);
	}

	return *this;
}

ot::StyledTextBuilder& ot::StyledTextBuilder::operator<<(StyledText::TextControl _control) {
	// Check for changes and apply new style
	switch (_control) {
	case ot::StyledText::ClearStyle:
		if (m_entries.back().getStyle().hasStyleSet()) {
			this->applyNextStyle(StyledTextStyle());
		}
		break;

	case ot::StyledText::Bold:
		if (!m_entries.back().getStyle().getBold()) {
			StyledTextStyle newStyle = m_entries.back().getStyle();
			newStyle.setBold(true);
			this->applyNextStyle(newStyle);
		}
		break;

	case ot::StyledText::Italic:
		if (!m_entries.back().getStyle().getItalic()) {
			StyledTextStyle newStyle = m_entries.back().getStyle();
			newStyle.setItalic(true);
			this->applyNextStyle(newStyle);
		}
		break;

	case ot::StyledText::NotBold:
		if (m_entries.back().getStyle().getBold()) {
			StyledTextStyle newStyle = m_entries.back().getStyle();
			newStyle.setBold(false);
			this->applyNextStyle(newStyle);
		}
		break;

	case ot::StyledText::NotItalic:
		if (m_entries.back().getStyle().getItalic()) {
			StyledTextStyle newStyle = m_entries.back().getStyle();
			newStyle.setItalic(false);
			this->applyNextStyle(newStyle);
		}
		break;

	default:
		OT_LOG_EAS("Unknown TextControl (" + std::to_string((int)_control) + ")");
		break;
	}

	return *this;
}

void ot::StyledTextBuilder::applyNextStyle(const StyledTextStyle& _style) {
	// If the text is empty setup new style.
	if (m_entries.back().getText().empty()) {
		m_entries.back().setStyle(_style);
	}
	// If any text is set the new style is applied for the new section.
	else {
		m_entries.push_back(StyledTextEntry(_style));
	}
}

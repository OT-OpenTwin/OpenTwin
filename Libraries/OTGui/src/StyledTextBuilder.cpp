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

bool ot::StyledTextBuilder::isEmpty(void) const {
	for (const StyledTextEntry& entry : m_entries) {
		if (!entry.getText().empty()) {
			return false;
		}
	}
	return true;
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
	StyledTextStyle newStyle = m_entries.back().getStyle();

	switch (_control) {
	case ot::StyledText::ClearStyle: newStyle = StyledTextStyle(); break;
	case ot::StyledText::Bold: newStyle.setBold(true); break;
	case ot::StyledText::NotBold: newStyle.setBold(false); break;
	case ot::StyledText::Italic: newStyle.setItalic(true); break;
	case ot::StyledText::NotItalic: newStyle.setItalic(false); break;
	case ot::StyledText::Underline: newStyle.setUnderline(true); break;
	case ot::StyledText::NotUnderline: newStyle.setUnderline(false); break;
	case ot::StyledText::LineTrough: newStyle.setLineTrough(true); break;
	case ot::StyledText::NotLineTrough: newStyle.setLineTrough(false); break;
	case ot::StyledText::Text: newStyle.setTextSize(StyledTextStyle::TextSize::Regular); break;
	case ot::StyledText::Header1: newStyle.setTextSize(StyledTextStyle::TextSize::Header1); break;
	case ot::StyledText::Header2: newStyle.setTextSize(StyledTextStyle::TextSize::Header2); break;
	case ot::StyledText::Header3: newStyle.setTextSize(StyledTextStyle::TextSize::Header3); break;
	default:
		OT_LOG_EAS("Unknown TextControl (" + std::to_string((int)_control) + ")");
		break;
	}

	if (m_entries.back().getStyle() != newStyle) {
		this->applyNextStyle(newStyle);
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

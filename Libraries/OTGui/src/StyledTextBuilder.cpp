// @otlicense
// File: StyledTextBuilder.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/StyledTextBuilder.h"

ot::StyledTextBuilder::StyledTextBuilder(const BuilderFlags& _flags) :
	m_flags(_flags)
{
	m_entries.push_back(StyledTextEntry());
}

ot::StyledTextBuilder::StyledTextBuilder(const ConstJsonObject& _jsonObject) :
	m_flags(NoFlags)
{
	this->setFromJsonObject(_jsonObject);
}

ot::StyledTextBuilder::StyledTextBuilder(const StyledTextBuilder& _other) {
	*this = _other;
}

ot::StyledTextBuilder::StyledTextBuilder(StyledTextBuilder&& _other) noexcept {
	*this = std::move(_other);
}

ot::StyledTextBuilder& ot::StyledTextBuilder::operator=(const StyledTextBuilder& _other) {
	if (this != &_other) {
		m_flags = _other.m_flags;
		m_entries = _other.m_entries;
	}
	return *this;
}

ot::StyledTextBuilder& ot::StyledTextBuilder::operator=(StyledTextBuilder&& _other) noexcept {
	if (this != &_other) {
		m_flags = std::move(_other.m_flags);
		m_entries = std::move(_other.m_entries);
	}
	
	return *this;
}

void ot::StyledTextBuilder::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray entriesArray;
	for (const StyledTextEntry& entry : m_entries) {
		JsonObject entryObject;
		entry.addToJsonObject(entryObject, _allocator);
		entriesArray.PushBack(entryObject, _allocator);
	}
	_object.AddMember("Entries", entriesArray, _allocator);

	JsonArray flagsArray;
	if (m_flags.has(BuilderFlag::EvaluateSubstitutionTokens)) {
		flagsArray.PushBack("EvaluateSubstitutionTokens", _allocator);
	}
	_object.AddMember("Flags", flagsArray, _allocator);
}

void ot::StyledTextBuilder::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entries.clear();
	ConstJsonObjectList entriesArray = json::getObjectList(_object, "Entries");
	for (const ConstJsonObject& entryObject : entriesArray) {
		m_entries.push_back(StyledTextEntry(entryObject));
	}

	m_flags = StyledTextBuilder::NoFlags;
	for (const std::string& flag : json::getStringList(_object, "Flags")) {
		if (flag == "EvaluateSubstitutionTokens") {
			m_flags.set(StyledTextBuilder::EvaluateSubstitutionTokens, true);
		}
		else {
			OT_LOG_EAS("Unknown StyledTextBuilder flag \"" + flag + "\"");
		}
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

ot::StyledTextBuilder& ot::StyledTextBuilder::operator<<(StyledText::SubstitutionToken _token) {
	m_entries.back().getText().append("$(" + StyledText::toString(_token) + ")");
	m_flags.set(BuilderFlag::EvaluateSubstitutionTokens, true);
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

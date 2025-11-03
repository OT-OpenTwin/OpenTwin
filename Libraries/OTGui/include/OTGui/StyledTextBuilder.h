// @otlicense
// File: StyledTextBuilder.h
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
#include "OTSystem/Flags.h"
#include "OTGui/StyledTextEntry.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT StyledTextBuilder : public Serializable {
	public:
		enum BuilderFlag {
			NoFlags                    = 0 << 0, //! @brief No flags set.
			EvaluateSubstitutionTokens = 1 << 0  //! @brief Evaluate substitution tokens when converting text.
		};
		typedef Flags<BuilderFlag> BuilderFlags;

		StyledTextBuilder(const BuilderFlags& _flags = BuilderFlag::NoFlags);
		StyledTextBuilder(const ConstJsonObject& _jsonObject);
		StyledTextBuilder(const StyledTextBuilder& _other);
		StyledTextBuilder(StyledTextBuilder&& _other) noexcept;
		virtual ~StyledTextBuilder() {};

		StyledTextBuilder& operator = (const StyledTextBuilder& _other);
		StyledTextBuilder& operator = (StyledTextBuilder&& _other) noexcept;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Returns true if the builder has no entries that should be displayed (no text provided).
		bool isEmpty(void) const;

		void setFlag(BuilderFlag _flag, bool _active = true) { m_flags.set(_flag, _active); };
		void setFlags(const BuilderFlags& _flags) { m_flags = _flags; };
		const BuilderFlags& getFlags(void) const { return m_flags; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Builder

		//! @brief Return all entries that should be displayed.
		const std::list<StyledTextEntry> getEntries(void) const { return m_entries; };

		//! @brief Append text to the current section.
		StyledTextBuilder& operator << (const char* _text);

		//! @brief Append text to the current section.
		StyledTextBuilder& operator << (const std::string& _text);

		//! @brief Append the provided SubstitutionToken as a text to the current section.
		//! The token will be replaced when visualizing the text.
		//! @note Using a StyledText::SubstitutionToken will automatically enable the BuilderFlag::EvaluateSubstitutionTokens flag.
		StyledTextBuilder& operator << (StyledText::SubstitutionToken _token);

		//! @brief Set the StyledText::ColorReference for the next/current section.
		//! If the current section contains text, a new section will be created by copying the style of the current section and applying the new ColorReference to the new section.
		StyledTextBuilder& operator << (StyledText::ColorReference _colorReference);

		//! @brief Apply the StyledText::TextControl for the next/current section.
		//! If the current section contains text, a new section will be created by copying the style of the current section and applying the StyledText::TextControl.
		StyledTextBuilder& operator << (StyledText::TextControl _control);

	private:
		//! @brief Apply the provided StyledTextStyle to the current/next section.
		//! If the current section contains text, a new section will be created and the style will be applied to the new section, otherwise the style will be applied to the current section.
		void applyNextStyle(const StyledTextStyle& _style);

		BuilderFlags m_flags;
		std::list<StyledTextEntry> m_entries;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::StyledTextBuilder::BuilderFlag, ot::StyledTextBuilder::BuilderFlags)
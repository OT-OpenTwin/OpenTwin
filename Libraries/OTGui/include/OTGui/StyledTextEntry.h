// @otlicense
// File: StyledTextEntry.h
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
#include "OTGui/StyledTextStyle.h"

namespace ot {

	class OT_GUI_API_EXPORT StyledTextEntry : public Serializable {
	public:
		StyledTextEntry() = default;
		StyledTextEntry(const StyledTextStyle& _style);
		StyledTextEntry(const ConstJsonObject& _jsonObject);
		StyledTextEntry(const StyledTextEntry&) = default;
		virtual ~StyledTextEntry() = default;
		StyledTextEntry& operator = (const StyledTextEntry&) = default;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setStyle(const StyledTextStyle& _style) { m_style = _style; };
		StyledTextStyle& getStyle(void) { return m_style; };
		const StyledTextStyle& getStyle(void) const { return m_style; };

		void setText(const std::string& _text) { m_text = _text; };
		std::string& getText(void) { return m_text; };
		const std::string& getText(void) const { return m_text; };

	private:
		StyledTextStyle m_style;
		std::string m_text;
	};

}
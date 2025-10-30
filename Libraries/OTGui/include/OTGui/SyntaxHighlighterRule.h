// @otlicense
// File: SyntaxHighlighterRule.h
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
#include "OTCore/Serializable.h"
#include "OTGui/Font.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT SyntaxHighlighterRule : public Serializable {
	public:
		SyntaxHighlighterRule();

		//! @brief Constructor.
		//! The SyntaxHighlighterRule takes ownership of the painter.
		SyntaxHighlighterRule(const std::string& _regularExpression, Painter2D* _painter);
		
		SyntaxHighlighterRule(const SyntaxHighlighterRule& _other);
		SyntaxHighlighterRule(SyntaxHighlighterRule&& _other) noexcept;
		
		virtual ~SyntaxHighlighterRule();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		SyntaxHighlighterRule& operator = (const SyntaxHighlighterRule& _other);
		SyntaxHighlighterRule& operator = (SyntaxHighlighterRule&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################
		
		// Serialization

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the painter.
		//! The SyntaxHighlighterRule takes ownership of the painter.
		//! An existing painter will be destroyed before replacing.
		void setPainter(Painter2D* _painter);

		//! @brief Returns the current painter.
		//! The SyntaxHighlighterRule keeps ownership of the painter.
		Painter2D* getPainter(void) { return m_painter; };

		//! @brief Returns the current painter.
		//! The SyntaxHighlighterRule keeps ownership of the painter.
		const Painter2D* getPainter(void) const { return m_painter; };

		void setRegularExpression(const std::string& _regularExpression) { m_regex = _regularExpression; };
		const std::string& getRegularExpression(void) const { return m_regex; };

		void setFont(const Font& _f) { m_font = _f; };
		const Font& getFont(void) const { return m_font; };

	private:
		Painter2D* m_painter;
		std::string m_regex;
		Font m_font;
	};

}
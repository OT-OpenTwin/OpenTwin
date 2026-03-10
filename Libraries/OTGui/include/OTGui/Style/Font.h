// @otlicense
// File: Font.h
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
#include "OTGui/GuiTypes.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT Font : public Serializable {
		OT_DECL_DEFCOPY(Font)
		OT_DECL_DEFMOVE(Font)
	public:
		Font();
		Font(FontFamily _fontFamily, int _sizePx = 12, bool _isBold = false, bool _isItalic = false);
		Font(const std::string& _fontFamily, int _sizePx = 12, bool _isBold = false, bool _isItalic = false);
		virtual ~Font();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		bool operator == (const Font& _other) const;
		bool operator != (const Font& _other) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setFamily(const std::string& _fontFamily) { m_family = _fontFamily; };
		void setFamily(FontFamily _fontFamily);
		const std::string& family(void) const { return m_family; };

		void setSize(int _px) { m_sizePx = _px; };
		int size(void) const { return m_sizePx; };

		void setBold(bool _isBold = true) { m_isBold = _isBold; };
		bool isBold(void) const { return m_isBold; };

		void setItalic(bool _isItalic = true) { m_isItalic = _isItalic; };
		bool isItalic(void) const { return m_isItalic; };

	private:
		std::string m_family;
		int m_sizePx;
		bool m_isBold;
		bool m_isItalic;
	};

}
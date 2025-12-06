// @otlicense
// File: PropertyString.h
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
#include "OTGui/Property.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT PropertyString : public Property {
		OT_DECL_NOCOPY(PropertyString)
		OT_DECL_NOMOVE(PropertyString)
	public:
		using PropertyValueType = std::string;

		explicit PropertyString(const PropertyString* _other);
		explicit PropertyString(const PropertyBase& _base);
		explicit PropertyString(PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyString(const std::string& _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyString(const std::string& _name, const std::string& _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyString() {};

		static std::string propertyTypeString(void) { return "String"; };
		virtual std::string getPropertyType(void) const override { return PropertyString::propertyTypeString(); };

		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode) override;

		virtual Property* createCopy(void) const override;

		void setValue(const std::string& _value) { m_value = _value; };
		const std::string& getValue(void) const { return m_value; };

		void setPlaceholderText(const std::string& _text) { m_placeholderText = _text; };
		const std::string& getPlaceholderText(void) const { return m_placeholderText; };

		void setMaxLength(int _length) { m_maxLength = _length; };
		int getMaxLength(void) const { return m_maxLength; };

		void setMultiline(bool _multiline) { m_multiline = _multiline; };
		bool isMultiline(void) const { return m_multiline; };

	protected:
		//! @brief Add the property data to the provided JSON object
		//! The property type is already added
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the property data from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setPropertyData(const ot::ConstJsonObject& _object) override;

	private:
		std::string m_value;
		std::string m_placeholderText;
		unsigned int m_maxLength;
		bool m_multiline;
	};

}
// @otlicense
// File: PropertyColor.h
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
#include "OTCore/Color.h"
#include "OTGui/Property.h"

namespace ot {

	class OT_GUI_API_EXPORT PropertyColor : public Property {
		OT_DECL_NOCOPY(PropertyColor)
		OT_DECL_NOMOVE(PropertyColor)
	public:
		using PropertyValueType = Color;

		explicit PropertyColor(const PropertyColor* _other);
		explicit PropertyColor(const PropertyBase& _base);
		explicit PropertyColor(PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyColor(ot::Color _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyColor(const std::string& _name, ot::Color _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyColor() {};

		static std::string propertyTypeString(void) { return "Color"; };
		virtual std::string getPropertyType(void) const override { return PropertyColor::propertyTypeString(); };

		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode) override;

		virtual Property* createCopy(void) const override;

		void setValue(const ot::Color& _value) { m_value = _value; };
		const ot::Color& getValue(void) const { return m_value; };

		void setIncludeAlpha(bool _include = true) { m_includeAlpha = _include; };
		bool getIncludeAlpha(void) const { return m_includeAlpha; };

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
		ot::Color m_value;
		bool m_includeAlpha;
	};

}
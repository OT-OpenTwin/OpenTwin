//! @file PropertyColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/Property.h"

namespace ot {

	class OT_GUI_API_EXPORT PropertyColor : public Property {
		OT_DECL_NOCOPY(PropertyColor)
	public:
		using PropertyValueType = Color;

		PropertyColor(const PropertyColor* _other);
		PropertyColor(const PropertyBase& _base);
		PropertyColor(PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyColor(ot::Color _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyColor(const std::string& _name, ot::Color _value, PropertyFlags _flags = PropertyFlags(NoFlags));
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
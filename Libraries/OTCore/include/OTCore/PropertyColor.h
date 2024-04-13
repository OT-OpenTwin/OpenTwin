//! @file PropertyColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Property.h"

#define OT_PROPERTY_TYPE_Color "Color"

namespace ot {

	class OT_CORE_API_EXPORT PropertyColor : public Property {
		OT_DECL_NOCOPY(PropertyColor)
	public:
		PropertyColor(const PropertyColor* _other);
		PropertyColor(PropertyFlags _flags = PropertyFlags::NoFlags) : Property(_flags) {};
		PropertyColor(ot::Color _value, PropertyFlags _flags = PropertyFlags::NoFlags) : Property(_flags), m_value(_value) {};
		PropertyColor(const std::string& _name, ot::Color _value, PropertyFlags _flags = PropertyFlags::NoFlags) : Property(_name, _flags), m_value(_value) {};
		virtual ~PropertyColor() {};

		virtual std::string getPropertyType(void) const override { return OT_PROPERTY_TYPE_Color; };

		virtual Property* createCopy(void) const override;

		void setValue(ot::Color _value) { m_value = _value; };
		ot::Color value(void) const { return m_value; };

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
	};

}
//! @file PropertyInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

namespace ot {

	class OT_GUI_API_EXPORT PropertyInt : public Property {
		OT_DECL_NOCOPY(PropertyInt)
	public:
		PropertyInt(PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyInt(int _value, PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyInt(const std::string& _name, int _value, PropertyFlags _flags = PropertyFlags::NoFlags);
		virtual ~PropertyInt() {};

		virtual PropertyType getPropertyType(void) const override { return Property::IntType; };

		virtual Property* createCopy(void) const override;

		void setValue(int _value) { m_value = _value; };
		int value(void) const { return m_value; };

		void setRange(int _min, int _max) { m_min = _min; m_max = _max; };

		void setMin(int _value) { m_min = _value; };
		int min(void) const { return m_min; };

		void setMax(int _value) { m_max = _value; };
		int max(void) const { return m_max; };

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
		int m_value;
		int m_min;
		int m_max;
	};

}
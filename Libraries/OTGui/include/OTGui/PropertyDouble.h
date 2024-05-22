//! @file PropertyDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

#define OT_PROPERTY_TYPE_Double "Double"

namespace ot {

	class OT_GUI_API_EXPORT PropertyDouble : public Property {
		OT_DECL_NOCOPY(PropertyDouble)
	public:
		PropertyDouble(const PropertyDouble* _other);
		PropertyDouble(const PropertyBase& _base);
		PropertyDouble(PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyDouble(double _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyDouble(const std::string& _name, double _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyDouble() {};

		virtual std::string getPropertyType(void) const override { return OT_PROPERTY_TYPE_Double; };

		virtual Property* createCopy(void) const override;

		void setValue(double _value) { m_value = _value; };
		double value(void) const { return m_value; };

		void setRange(double _min, double _max) { m_min = _min; m_max = _max; };

		void setMin(double _value) { m_min = _value; };
		double min(void) const { return m_min; };

		void setMax(double _value) { m_max = _value; };
		double max(void) const { return m_max; };

		void setPrecision(int _p) { m_precision = _p; };
		int precision(void) const { return m_precision; };

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
		double m_value;
		double m_min;
		double m_max;
		int m_precision;
	};

}
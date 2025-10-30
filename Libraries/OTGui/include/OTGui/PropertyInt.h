// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

namespace ot {

	class OT_GUI_API_EXPORT PropertyInt : public Property {
		OT_DECL_NOCOPY(PropertyInt)
		OT_DECL_NOMOVE(PropertyInt)
	public:
		using PropertyValueType = int;

		explicit PropertyInt(const PropertyInt* _other);
		explicit PropertyInt(const PropertyBase& _base);
		explicit PropertyInt(PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyInt(int _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyInt(const std::string& _name, int _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyInt(const std::string& _name, int _value, int _min, int _max, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyInt() {};

		static std::string propertyTypeString(void) { return "Int"; };
		virtual std::string getPropertyType(void) const override { return PropertyInt::propertyTypeString(); };

		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode) override;

		virtual Property* createCopy(void) const override;

		void setValue(int _value) { m_value = _value; };
		int getValue(void) const { return m_value; };

		void setRange(int _min, int _max) { m_min = _min; m_max = _max; };

		void setMin(int _value) { m_min = _value; };
		int getMin(void) const { return m_min; };

		void setMax(int _value) { m_max = _value; };
		int getMax(void) const { return m_max; };

		//! @brief Sets the suffix that is appended to the value when displayed in the frontend.
		//! The suffix is only displayed when custom values are not enabled.
		//! @param _suffix Suffix string.
		void setSuffix(const std::string& _suffix) { m_suffix = _suffix; };
		std::string getSuffix() const { return m_suffix; };

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
		std::string m_suffix;
	};

}
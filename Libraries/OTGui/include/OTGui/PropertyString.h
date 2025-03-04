//! @file PropertyString.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT PropertyString : public Property {
		OT_DECL_NOCOPY(PropertyString)
	public:
		using PropertyValueType = std::string;

		PropertyString(const PropertyString* _other);
		PropertyString(const PropertyBase& _base);
		PropertyString(PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyString(const std::string& _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyString(const std::string& _name, const std::string& _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyString() {};

		static std::string propertyTypeString(void) { return "String"; };
		virtual std::string getPropertyType(void) const override { return PropertyString::propertyTypeString(); };

		virtual void setValueFromOther(const Property* _other) override;

		virtual Property* createCopy(void) const override;

		void setValue(const std::string& _value) { m_value = _value; };
		const std::string& getValue(void) const { return m_value; };

		void setPlaceholderText(const std::string& _text) { m_placeholderText = _text; };
		const std::string& getPlaceholderText(void) const { return m_placeholderText; };

		void setMaxLength(int _length) { m_maxLength = _length; };
		int getMaxLength(void) const { return m_maxLength; };

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
	};

}
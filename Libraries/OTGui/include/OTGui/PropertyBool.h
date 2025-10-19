//! @file PropertyBool.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

namespace ot {

	class OT_GUI_API_EXPORT PropertyBool : public Property {
		OT_DECL_NOCOPY(PropertyBool)
		OT_DECL_NOMOVE(PropertyBool)
	public:
		using PropertyValueType = bool;

		explicit PropertyBool(const PropertyBool* _other);
		explicit PropertyBool(const PropertyBase& _base);
		explicit PropertyBool(PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyBool(bool _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		explicit PropertyBool(const std::string& _name, bool _value, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyBool() {};

		static std::string propertyTypeString(void) { return "Bool"; };
		virtual std::string getPropertyType(void) const override { return PropertyBool::propertyTypeString(); };

		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode) override;

		virtual Property* createCopy(void) const override;

		void setValue(bool _value) { m_value = _value; };
		bool getValue(void) const { return m_value; };

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
		bool m_value;
	};

}
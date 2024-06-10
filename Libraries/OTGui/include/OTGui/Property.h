//! @file Property.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyBase.h"

#pragma warning(disable:4251)

#define OT_JSON_MEMBER_Property_Type "Type"

namespace ot {

	class PropertyGroup;

	//! \class Property
	//! \brief The Property class is used as a base class for all Properties that can be displayed and modified in the frontend.
	class OT_GUI_API_EXPORT Property : public PropertyBase {
		OT_DECL_NOCOPY(Property)
	public:
		Property(const Property* _other);
		Property(const PropertyBase& _base);
		Property(PropertyFlags _flags = PropertyFlags(NoFlags));
		Property(const std::string& _name, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~Property() {};

		virtual std::string getPropertyType(void) const = 0;

		virtual Property* createCopy(void) const = 0;

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setParentGroup(PropertyGroup* _group) { m_parentGroup = _group; };
		PropertyGroup* getParentGroup(void) const { return m_parentGroup; };

		//! \brief Returns the property path.
		//! The property path is a string containing all parent groups and the property name (e.g. "RootGroup/ChildGroup/ThisItem").
		std::string getPropertyPath(char _delimiter = '/') const;

	protected:
		//! @brief Add the property data to the provided JSON object
		//! The property type is already added
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const = 0;

		//! @brief Set the property data from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setPropertyData(const ot::ConstJsonObject& _object) = 0;

	private:
		PropertyGroup* m_parentGroup;
	};

}

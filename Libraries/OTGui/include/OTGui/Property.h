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
		using PropertyValueType = void;

		Property(const Property* _other);
		Property(const PropertyBase& _base);
		Property(PropertyFlags _flags = PropertyFlags(NoFlags));
		Property(const std::string& _name, PropertyFlags _flags = PropertyFlags(NoFlags));

		//! \brief Destructor.
		//! Destroys the parent group if set.
		virtual ~Property();

		virtual std::string getPropertyType(void) const = 0;

		//! \brief Creates a copy of this property.
		virtual Property* createCopy(void) const = 0;

		//! brief Creates a copy of this property and all parent groups excluding other properties and group paths.
		//! The created property will have all the parent groups set up to the root group.
		//!
		//! \note Other child groups and properties are ignored.
		//!
		//! Assume you have the following structure:<br>
		//! <pre>
		//!   Root
		//!   +--> ChildGroup1
		//!        +--> Property1
		//!   +--> ChildGroup2
		//!        +--> <This Property>
		//!   +--> ChildGroup3
		//!        +--> Property3
		//! 
		//! Then the new structure will be:
		//!   Root
		//!   +--> ChildGroup2
		//!        +--> <This Property>
		//! </pre>
		Property* createCopyWithParents(void) const;

		//! @brief Set the property values from the other property.
		//! The base date won't be changed (base class method does nothing).
		//! @param _other Other property.
		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode = MergeFlag::MergeValues);

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

		//! \brief Returns the root group.
		//! If this item has no parent group returns 0.
		PropertyGroup* getRootGroup(void) const;

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

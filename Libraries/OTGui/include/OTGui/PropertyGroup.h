//! @file PropertyGroup.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	class Property;

	class OT_GUI_API_EXPORT PropertyGroup : public Serializable {
	public:
		//! @brief Create empty group
		PropertyGroup();

		PropertyGroup(const PropertyGroup& _other);

		//! @brief Create empty group with name and title
		//! @param _name Will be set as name and title
		PropertyGroup(const std::string& _name);

		//! @brief Create empty group with name and title
		//! @param _name Group name
		//! @param _title Group title
		PropertyGroup(const std::string& _name, const std::string& _title);
		virtual ~PropertyGroup();

		PropertyGroup& operator = (const PropertyGroup& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \brief Adds the contents of the other configuration to this configuration.
		//! If an improved merge is required just subclass and implement 
		//! \param _other Other group to merge into this.
		//! \param _replaceExistingProperties If enabled existing properties will be replaced with their corresponding property in the other group. Note that the property type may change if the other group contains a different property type.
		virtual void mergeWith(const PropertyGroup& _other, bool _replaceExistingProperties);

		void setParentGroup(PropertyGroup* _group) { m_parentGroup = _group; };
		PropertyGroup* getParentGroup(void) const { return m_parentGroup; };

		void setName(const std::string& _name) { m_name = _name; };
		std::string& getName(void) { return m_name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		std::string& getTitle(void) { return m_title; };
		const std::string& getTitle(void) const { return m_title; };

		//! @brief Set the properties.
		//! This group takes ownership of the properties.
		void setProperties(const std::list<Property*>& _properties);

		//! @brief Add the property.
		//! This group takes ownership of the property.
		void addProperty(Property* _property);

		//! \brief Removes the property with the given name.
		void removeProperty(const std::string& _propertyName);

		//! \brief Group properties.
		const std::list<Property*>& getProperties(void) const { return m_properties; };

		//! \brief Returns all properties of all groups and nested groups.
		std::list<Property*> getAllProperties(void) const;

		//! @brief Set the child groups.
		//! This group takes ownership of the groups.
		void setChildGroups(const std::list<PropertyGroup*>& _groups);

		const std::list<PropertyGroup*>& getChildGroups(void) const { return m_childGroups; };

		//! @brief Add the provided group as a child.
		//! This group takes ownership of the child.
		void addChildGroup(PropertyGroup* _group);

		PropertyGroup* findGroup(const std::string& _name) const;

		void findPropertiesBySpecialType(const std::string& _specialType, std::list<Property*>& _list) const;

		void setBackgroundColor(DefaultColor _color) { this->setBackgroundColor(Color(_color)); };
		void setBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setBackgroundColor(Color(_r, _g, _b, _a)); };
		void setBackgroundColor(const Color& _color) { m_backgroundColor = _color; };
		const Color& getBackgroundColor(void) const { return m_backgroundColor; };

		void setAlternateBackgroundColor(DefaultColor _color) { this->setAlternateBackgroundColor(Color(_color)); };
		void setAlternateBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setAlternateBackgroundColor(Color(_r, _g, _b, _a)); };
		void setAlternateBackgroundColor(const Color& _color) { m_alternateBackgroundColor = _color; };
		const Color& getAlternateBackgroundColor(void) const { return m_alternateBackgroundColor; };

		void clear(bool _keepGroups = false);

		//! \brief Returns false if at least one property exists in this group or any of the child groups.
		bool isEmpty(void) const;

		//! \brief Returns the group path.
		//! The group path is a string containing all parent groups and this group name (e.g. "Root/Child/ThisGroup").
		std::string getGroupPath(char _delimiter = '/') const;

	private:
		PropertyGroup* m_parentGroup;

		std::string m_name;
		std::string m_title;
		Color m_backgroundColor;
		Color m_alternateBackgroundColor;
		std::list<Property*> m_properties;
		std::list<PropertyGroup*> m_childGroups;
	};

}

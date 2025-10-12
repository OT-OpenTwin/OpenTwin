//! @file PropertyGridCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyBase.h"

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	class Property;
	class PropertyGroup;

	class OT_GUI_API_EXPORT PropertyGridCfg : public ot::Serializable {
	public:
		PropertyGridCfg();
		PropertyGridCfg(const PropertyGridCfg& _other);
		PropertyGridCfg(PropertyGridCfg&& _other) noexcept;
		virtual ~PropertyGridCfg();

		PropertyGridCfg& operator = (const PropertyGridCfg& _other);
		PropertyGridCfg& operator = (PropertyGridCfg&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		//! @brief Adds the contents of the other configuration to this configuration.
		//! @param _other Other configuration to merge into this.
		//! @param _mergeMode Merge mode to use.
		void mergeWith(const PropertyGridCfg& _other, const PropertyBase::MergeMode& _mergeMode);

		void setRootGroups(const std::list<PropertyGroup*>& _groups);
		void addRootGroup(PropertyGroup* _group);
		std::list<PropertyGroup*>& getRootGroups(void) { return m_rootGroups; };
		const std::list<PropertyGroup*>& getRootGroups(void) const { return m_rootGroups; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Sets the modal option.
		//! If a proeprty grid is filled with the modal option all further fill requests for the property grid will be ignored until the modal option is explicitly unset.
		//! @param _isModal Modal option.
		void setIsModal(bool _isModal = true) { m_isModal = _isModal; };

		//! @ref setIsModal
		bool getIsModal() const { return m_isModal; };

		PropertyGroup* findGroup(const std::string& _name, bool _searchChildGroups = false);
		const PropertyGroup* findGroup(const std::string& _name, bool _searchChildGroups = false) const;
		PropertyGroup* findOrCreateGroup(const std::string& _name, bool _searchChildGroups = false);

		//! @brief Returns all properties of all groups and nested groups.
		std::list<Property*> getAllProperties(void) const;
		std::list<Property*> findPropertiesBySpecialType(const std::string& _specialType) const;

		//! @brief Returns the property at the given path.
		//! @ref Property* findPropertyByPath(const std::list<std::string>& _path)
		Property* findPropertyByPath(const std::string& _path, char _delimiter = '/');

		//! @brief Returns the property at the given path.
		//! @ref Property* findPropertyByPath(const std::list<std::string>& _path)
		const Property* findPropertyByPath(const std::string& _path, char _delimiter = '/') const;

		//! @brief Returns the property at the given path.
		//! The path contains the group names from root to the item.
		//! The last path entry must be the property name.
		Property* findPropertyByPath(std::list<std::string> _path);

		//! @brief Returns the property at the given path.
		//! The path contains the group names from root to the item.
		//! The last path entry must be the property name.
		const Property* findPropertyByPath(std::list<std::string> _path) const;

		//! @brief Returns false if at least one property exists in any of the groups and its child groups.
		bool isEmpty(void) const;

	private:
		void clear(void);

		bool m_isModal;
		std::list<PropertyGroup*> m_rootGroups;
	};

}
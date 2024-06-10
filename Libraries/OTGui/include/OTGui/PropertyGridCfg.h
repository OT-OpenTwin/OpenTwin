//! @file PropertyGridCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

#pragma warning(disable:4251)

namespace ot {

	class Property;
	class PropertyGroup;

	class OT_GUI_API_EXPORT PropertyGridCfg : public ot::Serializable {
	public:
		PropertyGridCfg();
		PropertyGridCfg(const PropertyGridCfg& _other);
		virtual ~PropertyGridCfg();

		PropertyGridCfg& operator = (const PropertyGridCfg& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \brief Adds the contents of the other configuration to this configuration.
		//! \param _other Other configuration to merge into this.
		//! \param _replaceExistingProperties If enabled existing properties will be replaced with their corresponding property in the other group. Note that the property type may change if the other group contains a different property type.
		void mergeWith(const PropertyGridCfg& _other, bool _replaceExistingProperties);

		PropertyGroup* getDefaultGroup(void) const { return m_defaultGroup; };

		void setRootGroups(const std::list<PropertyGroup*>& _groups);
		void addRootGroup(PropertyGroup* _group);
		const std::list<PropertyGroup*>& getRootGroups(void) const { return m_rootGroups; };
		PropertyGroup* findGroup(const std::string& _name, bool _searchChildGroups = false) const;
		PropertyGroup* findOrCreateGroup(const std::string& _name, bool _searchChildGroups = false);

		std::list<Property*> findPropertiesBySpecialType(const std::string& _specialType) const;

		//! \brief Returns false if at least one property exists in any of the groups and its child groups.
		bool isEmpty(void) const;

	private:
		void clear(void);

		PropertyGroup* m_defaultGroup;
		std::list<PropertyGroup*> m_rootGroups;
	};

}
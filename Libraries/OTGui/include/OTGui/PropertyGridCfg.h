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

namespace ot {

	class PropertyGroup;

	class OT_GUI_API_EXPORT PropertyGridCfg : public ot::Serializable {
		OT_DECL_NOCOPY(PropertyGridCfg)
	public:
		PropertyGridCfg();
		virtual ~PropertyGridCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		PropertyGroup* defaultGroup(void) const { return m_defaultGroup; };

		void setRootGroups(const std::list<PropertyGroup*>& _groups);
		void addRootGroup(PropertyGroup* _group);
		const std::list<PropertyGroup*>& rootGroups(void) const { return m_rootGroups; };

	private:
		void clear(void);

		PropertyGroup* m_defaultGroup;
		std::list<PropertyGroup*> m_rootGroups;
	};

}
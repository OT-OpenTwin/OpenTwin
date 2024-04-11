//! @file PropertyDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/PropertyGridCfg.h"
#include "OTGui/DialogCfg.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT PropertyDialogCfg : public DialogCfg {
		OT_DECL_NOCOPY(PropertyDialogCfg)
	public:
		PropertyDialogCfg();
		virtual ~PropertyDialogCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		const PropertyGridCfg& gridConfig(void) const { return m_gridConfig; };

		PropertyGroup* defaultGroup(void) const { return m_gridConfig.defaultGroup(); };

		void setRootGroups(const std::list<PropertyGroup*>& _groups) { m_gridConfig.setRootGroups(_groups); };
		void addRootGroup(PropertyGroup* _group) { m_gridConfig.addRootGroup(_group); };
		const std::list<PropertyGroup*>& rootGroups(void) const { return m_gridConfig.rootGroups(); };

	private:
		PropertyGridCfg m_gridConfig;
	};

}
//! @file BlockEditorAPI.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditorAPI/BlockEditorConfigurationPackage.h"
#include "OpenTwinCore/Owner.h"

// std header
#include <list>

namespace ot {

	class BlockPickerWidget;
	class BlockNetworkEditor;

	namespace BlockEditorAPI {

		//! @brief Create a new empty block editor
		BLOCK_EDITOR_API_EXPORT BlockNetworkEditor* createEmptyBlockEditor(const ot::ServiceOwner_t& _owner, const ot::BlockEditorConfigurationPackage& _config);

		//! @brief Set the global block picker widget that will be used by this API
		BLOCK_EDITOR_API_EXPORT void setGlobalBlockPickerWidget(ot::BlockPickerWidget* _picker);

		//! @brief Clean up all editors and related data for the given owner
		BLOCK_EDITOR_API_EXPORT void clean(const ot::ServiceOwner_t& _owner);

		//! @brief Return all block editors for the given owner
		BLOCK_EDITOR_API_EXPORT std::list<ot::BlockNetworkEditor*> blockEditors(const ot::ServiceOwner_t& _owner);

	}

}
//! @file BlockConfigurationAPI.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockEditorConfigurationPackage.h"

// std header
#include <string>
#include <list>

namespace ot {

	class BlockCategoryConfiguration;

	namespace BlockConfigurationAPI {

		class __declspec(dllexport) BlockEditorNotifier {
		public:
			BlockEditorNotifier() {};
			virtual ~BlockEditorNotifier() {};

			//! @brief Will be called when the specified editor was closed in the UI Frontend
			virtual void editorClosed(const std::string& _editorName) {};

		private:

		};

		//! @brief Will create a Block Editor view in the ui frontend application with the given settings
		//! @param _callbackNotifier The notifier that will receive the block editor callbacks
		//! @param _uiUrl The url where the ui frontend application can be reached at
		//! @param _config The block editor configuration
		BLOCKEDITORAPI_API_EXPORT bool createEmptyBlockEditor(BlockEditorNotifier* _callbackNotifier, const std::string& _uiUrl, const ot::BlockEditorConfigurationPackage& _config);

	}

}
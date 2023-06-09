// OpenTwin header
#include "OTBlockEditorAPI/BlockConfigurationAPI.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OpenTwinCore/ObjectManagerTemplate.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Singleton.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"

namespace ot {
	namespace intern { 
		class BlockEditorAPIManager : 
			public ot::ObjectManagerTemplate<std::string, ot::BlockConfigurationAPI::BlockEditorNotifier>, 
			public ot::Singleton<ot::intern::BlockEditorAPIManager>
		{
			OT_SINGLETON(ot::intern::BlockEditorAPIManager)
		};
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

bool ot::BlockConfigurationAPI::createEmptyBlockEditor(BlockEditorNotifier* _callbackNotifier, const std::string& _uiUrl, const ot::BlockEditorConfigurationPackage& _config) {
	// Check params (debug only)
	otAssert(_callbackNotifier, "No callback notifier provided");
	otAssert(!_uiUrl.empty(), "UI URL is empty");

	// Check for duplicates
	if (intern::BlockEditorAPIManager::instance().contains(_config.editorName())) {
		otAssert(0, "Duplicate Block Editor name");
		OT_LOG_E("A Block Editor (with: name = \"" + _config.editorName() + "\") already exists");
		return false;
	}

	// Construct UI message
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_BLOCKEDITOR_CreateEmptyBlockEditor);
	OT_rJSON_createValueObject(configurationObj);
	_config.addToJsonObject(doc, configurationObj);
	ot::rJSON::add(doc, OT_ACTION_PARAM_BLOCKEDITOR_ConfigurationPackage, configurationObj);
	ot::GlobalOwner::instance().addToJsonObject(doc, doc);

	// Send request to UI
	std::string response;
	if (!ot::msg::send("", _uiUrl, ot::QUEUE, ot::rJSON::toJSON(doc), response)) {
		otAssert(0, "[ERROR]: Failed to request Block Editor");
		OT_LOG_E("Failed to request block editor (with: name = \"" + _config.editorName() + "\") at UI Frontend (with: url = \"" + _uiUrl + "\"). Reason: Failed to deliver ping");
		return false;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		otAssert(0, "[ERROR]: Warning response for Block Editor request");
		OT_LOG_E("Failed to request block editor (with: name = \"" + _config.editorName() + "\") at UI Frontend (with: url = \"" + _uiUrl + "\"). Reason: Invalid response (expected = \"" OT_ACTION_RETURN_VALUE_OK "\"; received = \"" + response + "\")");
		return false;
	}
	OT_ACTION_IF_RESPONSE_WARNING(response) {
		otAssert(0, "[ERROR]: Warning response for Block Editor request");
		OT_LOG_E("Failed to request block editor (with: name = \"" + _config.editorName() + "\") at UI Frontend (with: url = \"" + _uiUrl + "\"). Reason: Invalid response (expected = \"" OT_ACTION_RETURN_VALUE_OK "\"; received = \"" + response + "\")");
		return false;
	}

	// Store notifier for editor
	intern::BlockEditorAPIManager::instance().insert(_config.editorName(), _callbackNotifier);

	return true;
}
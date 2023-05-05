// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPI.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OpenTwinCore/ObjectManagerTemplate.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"

namespace ot { namespace intern { typedef ot::ObjectManagerTemplate<std::string, ot::BlockEditorAPI::BlockEditorNotifier> BlockEditorAPIManager; } }

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

bool ot::BlockEditorAPI::createEmptyBlockEditor(BlockEditorNotifier* _callbackNotifier, const std::string& _uiUrl, const std::string& _editorName, const std::string& _editorTitle, const std::list<ot::BlockCategoryConfiguration*>& _topLevelBlockCategories) {
	// Check params (debug only)
	otAssert(_callbackNotifier, "No callback notifier provided");
	otAssert(!_uiUrl.empty(), "UI URL is empty");

	// Check for duplicates
	if (intern::BlockEditorAPIManager::instance().contains(_editorName)) {
		otAssert(0, "Duplicate Block Editor name");
		OT_LOG_E("A Block Editor (with: name = \"" + _editorName + "\") already exists");
		return;
	}

	// Construct UI message
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_BLOCKEDITOR_CreateEmptyBlockEditor);

	OT_rJSON_createValueArray(categoriesArr);
	for (auto c : _topLevelBlockCategories) {
		OT_rJSON_createValueObject(categoryObj);
		c->addToJsonObject(doc, categoryObj);
		categoriesArr.PushBack(categoryObj, doc.GetAllocator());
	}
	ot::rJSON::add(doc, OT_ACTION_PARAM_BLOCKEDITOR_EditorName, _editorName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle, _editorTitle);
	ot::rJSON::add(doc, OT_ACTION_PARAM_BLOCKEDITOR_Categories, categoriesArr);

	// Send request to UI
	std::string response;
	if (!ot::msg::send("", _uiUrl, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
		otAssert(0, "[ERROR]: Failed to request Block Editor");
		OT_LOG_E("Failed to request block editor (with: name = \"" + _editorName + "\") at UI Frontend (with: url = \"" + _uiUrl + "\"). Reason: Failed to deliver ping");
		return false;
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		otAssert(0, "[ERROR]: Warning response for Block Editor request");
		OT_LOG_E("Failed to request block editor (with: name = \"" + _editorName + "\") at UI Frontend (with: url = \"" + _uiUrl + "\"). Reason: Invalid response (expected = \"" OT_ACTION_RETURN_VALUE_OK "\"; received = \"" + response + "\")");
		return false;
	}

	// Store notifier for editor
	intern::BlockEditorAPIManager::instance().insert(_editorName, _callbackNotifier);

	return true;
}
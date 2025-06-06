#include "CircuitModelHandler.h"


#include "stdafx.h"
#include "Model.h"
#include "Application.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/Encryption.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTCore/ReturnMessage.h"


#include <assert.h>

void CircuitModelHandler::addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName) {
	const std::string groupName = ("Libraries");
	_uiComponent->addMenuGroup(_pageName, groupName);
	m_buttonAddCircuitModel.SetDescription(_pageName, groupName, "Add Circuit Model");
	_uiComponent->addMenuButton(m_buttonAddCircuitModel, ot::LockModelWrite, "Add","Default");
}

bool CircuitModelHandler::handleAction(const std::string& _action, ot::JsonDocument& _doc) {
	bool actionIsHandled = false;

	//if (_action == m_buttonAddCircuitModel.GetFullDescription()) {
	//	createModelDialog();
	//	actionIsHandled = true;
	//}
	//else if (_action == OT_ACTION_PARAM_MODEL_AddCircuitModelToProject) {
	//	std::string model = ot::json::getString(_doc, OT_ACTION_PARAM_Value);

	//	// Get modelString from LMS 


	//	// Add Model as FileEntity
	//


	//	actionIsHandled = true;
	//}
		

	// Just for pushing, remove this later
	actionIsHandled = true;
	return actionIsHandled;

}

void CircuitModelHandler::createModelDialog() {
	// First get list of all circuit models from LMS
	std::list<std::string> circuitModels = m_libraryMangementWrapper.getCircuitModels();

	if (circuitModels.empty()) {
		OT_LOG_E("No CircuitModels found");
		return;
	}
	
	// Now create OnePropertyDialog
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_OnePropertyDialog, doc.GetAllocator()), doc.GetAllocator());

	// Create property to edit
	ot::PropertyStringList* models = new ot::PropertyStringList;
	models->setPropertyName("CircuitModels");
	models->setPropertyTitle("Circuit Models");
	models->setList(circuitModels);

	// Create dialog
	ot::OnePropertyDialogCfg modelDialog(models);
	modelDialog.setName("CircuitModelDialog");
	modelDialog.setTitle("Circuit Model Dialog");
	modelDialog.setFlags(ot::DialogCfg::CancelOnNoChange);

	// Serialize
	ot::JsonObject cfgObj;
	modelDialog.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_PARAM_MODEL_AddCircuitModelToProject, doc.GetAllocator()), doc.GetAllocator());
	// Add service information
	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	//Send
	std::string response;
	Application::instance()->uiComponent()->sendMessage(true, doc, response);
}

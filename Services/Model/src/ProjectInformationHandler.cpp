// @otlicense

#include <stdafx.h>

// OpenTwin header
#include "Model.h"
#include "Application.h"
#include "ProjectInformationHandler.h"
#include "OTGui/ExtendedProjectInformation.h"

ProjectInformationHandler::ProjectInformationHandler(ot::ActionDispatcher* _dispatcher) :
	ot::ActionHandler(_dispatcher), ot::ButtonHandler(_dispatcher)
{
	m_editButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Project", "Edit Information", "ToolBar/EditProjectInformation");
	m_editButton.setButtonLockFlags(ot::LockType::ModelRead | ot::LockType::ModelWrite);
	connectToolBarButton(m_editButton, this, &ProjectInformationHandler::requestUploadProjectPreviewImage);

	connectAction(OT_ACTION_CMD_EditProjectInformation, this, &ProjectInformationHandler::applyProjectInformation);
}

ProjectInformationHandler::~ProjectInformationHandler() {

}

void ProjectInformationHandler::addButtons(ot::components::UiComponent* _ui) {
	const std::string pageName = Application::getToolBarPageName();
	
	_ui->addMenuGroup(pageName, m_editButton.getGroup());
	_ui->addMenuButton(m_editButton);
}

// ##################################################################################################################################################################################################################

// Action and Button handler

ot::ReturnMessage ProjectInformationHandler::applyProjectInformation(ot::JsonDocument& _document) {
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No model created yet");
	}

	ModelState* stateManager = model->getStateManager();
	if (!stateManager) {
		OT_LOG_E("No model state manager available");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No model state manager available");
	}

	ot::ExtendedProjectInformation info;
	info.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	std::vector<char> imageData = info.getImageData();
	if (imageData.empty()) {
		stateManager->removePreviewImage();
	}
	else if (!stateManager->addPreviewImage(std::move(imageData), info.getImageFormat())) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Setting project image failed");
	}
	
	if (info.getDescription().empty()) {
		stateManager->removeProjectDescription();
	}
	else if (!stateManager->addProjectDescription(info.getDescription(), info.getDescriptionSyntax())) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Setting project description failed");
	}

	app->getUiComponent()->displayMessage("Project information updated successfully\n");
	return ot::ReturnMessage::Ok;
}

void ProjectInformationHandler::requestUploadProjectPreviewImage() {
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model) {
		OT_LOG_E("No model created yet");
		return;
	}
	ModelState* state = model->getStateManager();
	if (!state) {
		OT_LOG_E("No model state manager available");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_EditProjectInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(OT_ACTION_CMD_EditProjectInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(app->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	app->getUiComponent()->sendMessage(true, doc, response);
}

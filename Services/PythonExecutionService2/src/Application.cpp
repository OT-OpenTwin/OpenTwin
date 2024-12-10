/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "UiNotifier.h"
#include "Application.h"
#include "ModelNotifier.h"
#include "SubprocessManager.h"

// Open twin header
#include "OTCore/Variable.h"
#include "OTCore/TypeNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, new UiNotifier(), new ModelNotifier()),
	m_subprocessManager(nullptr)
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions



// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (m_subprocessManager) {
		OT_LOG_EA("Application already in run mode");
		return;
	}

	// Create new subprocess manager
	m_subprocessManager = new SubprocessManager(this);

	if (EnsureDataBaseConnection()) {
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	DataBaseInfo info;
	info.setSiteId(this->siteID());
	info.setDataBaseUrl(DataBase::GetDataBase()->getDataBaseServerURL());
	info.setCollectionName(this->m_collectionName);
	info.setUserName(DataBase::GetDataBase()->getUserName());
	info.setUserPassword(DataBase::GetDataBase()->getUserPassword());

	m_subprocessManager->setDataBaseInfo(info);

	if (this->uiComponent()) {
		m_subprocessManager->setFrontendUrl(this->uiComponent()->getServiceURL());
	}
	if (this->modelComponent()) {
		m_subprocessManager->setModelUrl(this->modelComponent()->getServiceURL());
	}
}


std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc) {
	return ot::ReturnMessage(ot::ReturnMessage::Failed, "Not supported action").toJson();
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc) {
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui) {
	if (m_subprocessManager) {
		m_subprocessManager->setFrontendUrl(_ui->getServiceURL());
	}
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui) {
	if (m_subprocessManager) {
		m_subprocessManager->setFrontendUrl("");
	}
}

void Application::modelConnected(ot::components::ModelComponent * _model) {
	if (m_subprocessManager) {
		m_subprocessManager->setModelUrl(_model->getServiceURL());
	}
}

void Application::modelDisconnected(const ot::components::ModelComponent * _model) {
	if (m_subprocessManager) {
		m_subprocessManager->setModelUrl("");
	}
}

void Application::serviceConnected(ot::ServiceBase * _service) {

}

void Application::serviceDisconnected(const ot::ServiceBase * _service) {

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void) {

}

bool Application::startAsRelayService(void) const {
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}


// ##################################################################################################################################################################################################################

std::string Application::handleExecuteAction(ot::JsonDocument& _doc) {
	std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
	OT_LOG_D("Executing action: " + action);
	
	std::string returnMessage;
	if (!m_subprocessManager->sendRequest(_doc, returnMessage)) {
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to send request").toJson();
	}
	
	return returnMessage;
}
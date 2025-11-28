// @otlicense
// File: Application.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
	connectAction({ OT_ACTION_CMD_PYTHON_EXECUTE_Command, OT_ACTION_CMD_PYTHON_EXECUTE_Scripts }, this, &Application::handleForwardToSubprocess);
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

	DataBaseInfo info;
	info.setSiteID(this->getSiteID());
	info.setDataBaseUrl(DataBase::instance().getDataBaseServerURL());
	info.setCollectionName(this->getCollectionName());
	info.setUserName(DataBase::instance().getUserName());
	info.setUserPassword(DataBase::instance().getUserPassword());

	m_subprocessManager->setDataBaseInfo(info);

	if (this->getUiComponent()) {
		m_subprocessManager->setFrontendUrl(this->getUiComponent()->getServiceURL());
	}
	if (this->getModelComponent()) {
		m_subprocessManager->setModelUrl(this->getModelComponent()->getServiceURL());
	}
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

void Application::logFlagsChanged(const ot::LogFlags& _flags) {
	if (!m_subprocessManager) {
		return;
	}

	if (!m_subprocessManager->isConnected()) {
		return;
	}

	OT_LOG_D("Updating log flags");

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetLogFlags, doc.GetAllocator()), doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(_flags, flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LogFlags, flagsArr, doc.GetAllocator());

	std::string response;
	m_subprocessManager->sendRequest(doc, response);
}

std::string Application::handleForwardToSubprocess(ot::JsonDocument& _doc) 
{
	
	if (ot::json::exists(_doc, OT_ACTION_PARAM_Python_Environment))
	{
		ot::UID manifestUID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_Python_Environment);
		m_subprocessManager->setManifestUID(manifestUID);
	}
	std::string returnMessage;

	if (!m_subprocessManager->sendRequest(_doc, returnMessage)) {
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to send request").toJson();
	}

	return returnMessage;
}


// ##################################################################################################################################################################################################################

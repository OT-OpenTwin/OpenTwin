// @otlicense
// File: Application.h
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

#pragma once
// Service header
#include "MongoWrapper.h"

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTGui/ModelLibraryDialogCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"


// std header
#include <list>
#include <string>

class Application : public ot::ServiceBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public functions
	int initialize(const char* _siteID, const char* _ownURL, const char* _globalSessionServiceURL, const char* _databasePWD);
	

	
	

	

private:

	std::string getModelInformation(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value,
		const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	std::string getModelMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value,
		const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	std::optional<ot::ModelLibraryDialogCfg> createModelLibraryDialogCfg(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value,
		const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	std::string sendConfigToUI(const ot::JsonDocument& _doc, const std::string& _uiUrl);
	std::string sendMessageToModel(const ot::JsonDocument& _doc, const std::string& _modelUrl);

	void packMetaData(const bsoncxx::document::view& _doc, ot::LibraryModel& _model, ot::ModelLibraryDialogCfg& _dialogCfg);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler
	OT_HANDLER(handleGetDocument, Application, OT_ACTION_CMD_LMS_GetDocument, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleGetListOfDocuments, Application, OT_ACTION_CMD_LMS_GetDocumentList, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleCreateDialogConfig, Application, OT_ACTION_CMD_LMS_CreateConfig, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleModelDialogConfirmed, Application, OT_ACTION_CMD_UI_ModelDialogConfirmed, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleModelDialogCanceled, Application, OT_ACTION_CMD_UI_ModelDialogCanceled, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################
	
	MongoWrapper* db = nullptr;
	// Constructor/Destructor

	Application();
	virtual ~Application();

	std::string circuitMetaDataCollection = "CircuitMetaData";
	std::string circuitMetaParameters = "Parameters";
	
};

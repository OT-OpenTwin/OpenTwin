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

// LDS header
#include "Application.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/DebugHelper.h"
#include "OTCore/Logging/LogDispatcher.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCore/ReturnMessage.h"






//std header
#include <chrono>
#include <thread>
#include <optional>

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public functions

int Application::initialize(const char* _siteID,const char* _ownURL, const char* _globalSessionServiceURL, const char * _databasePWD) {
	try {
		OT_LOG_I("Library Management Service initialization");
		setSiteID(_siteID);
		setServiceURL(_ownURL);
		// Now store the command line arguments and perform the initialization
		if (_ownURL == nullptr) {
			exit(ot::AppExitCode::ServiceUrlInvalid);
		}
		if (_globalSessionServiceURL == nullptr) {
			exit(ot::AppExitCode::GSSUrlMissing);
		}

		ot::DebugHelper::serviceSetupCompleted(*this);

		// Register at GSS
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLibraryManagementService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL, ot::JsonString(getServiceURL(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

		// Send request to GSS
			std::string gssResponse;

		// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
		const int maxCt = 30;
		int ct = 1;
		bool ok = false;
	
		do {
			gssResponse.clear();
			if (!(ok = ot::msg::send(getServiceURL(), _globalSessionServiceURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
				OT_LOG_E("Register at Global Session Service failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(500ms);
				
			}
		} while (!ok && ct++ <= maxCt);

		if (!ok) {
			OT_LOG_E("Registration at Global Session service failed after " + std::to_string(maxCt) + " attemts. Exiting...");
			exit(ot::AppExitCode::GSSRegistrationFailed);
		}

		ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(gssResponse);
		if (rMsg != ot::ReturnMessage::Ok) {
			OT_LOG_E("Registration failed: " + rMsg.getWhat());
			exit(ot::AppExitCode::GSSRegistrationFailed);
		}

		// Get DBUrl and AuthUrl from gss response

		ot::JsonDocument gssRespoonseUrls;
		gssRespoonseUrls.fromJson(rMsg.getWhat());

		std::string dbUrl = ot::json::getString(gssRespoonseUrls, OT_ACTION_PARAM_SERVICE_DBURL);
		std::string authUrl = ot::json::getString(gssRespoonseUrls, OT_ACTION_PARAM_SERVICE_AUTHURL);

		

				



		// Initialzation of MongoDB connection
		db = new MongoWrapper(_siteID);



	}
	catch (std::exception& e) {
		OT_LOG_EAS(std::string{"Uncaught exception: "}.append(e.what()));
		exit(ot::AppExitCode::GeneralError);
	}
	catch (...) {
		assert(0);
		exit(ot::AppExitCode::UnknownError);
	}

	
	
	
	OT_LOG_D("Initialization finished");
	return ot::AppExitCode::Success;
}

std::string Application::getModelInformation(const ot::LibraryElementSelectionCfg& _selectionCfg, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	auto result = db->getDocumentList(_selectionCfg, _dbUserName, _dbUserPassword, _dbServerUrl);
	return result;
}

std::string Application::getModelMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	auto result = db->getMetaData(_collectionName, _fieldType, _value, _dbUserName, _dbUserPassword, _dbServerUrl);
	return result;
}

 std::optional<ot::ModelLibraryDialogCfg> Application::createModelLibraryDialogCfg(const ot::LibraryElementSelectionCfg _selectionCfg, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	
	// First get model info from database
	std::string modelInfos = getModelInformation( _selectionCfg, _dbUserName, _dbUserPassword, _dbServerUrl);

	ot::JsonDocument modelInfosDoc;
	modelInfosDoc.fromJson(modelInfos);

	ot::ModelLibraryDialogCfg dialogCfg;

	if (modelInfosDoc.IsObject()) {
		ot::ConstJsonObject obj = modelInfosDoc.getConstObject();

		if (!obj.HasMember("Documents") || !obj["Documents"].IsArray()) {
			OT_LOG_E("Documents array not found in response");
			return std::nullopt;
		}

		ot::ConstJsonArray docs = obj["Documents"].GetArray();

		for (const ot::JsonValue& val : docs) {
			if (!val.IsObject()) {
				continue;
			}

			ot::ConstJsonObject doc = val.GetObject();

			// Get name (required field)
			if (!doc.HasMember("Name") || !doc["Name"].IsString()) {
				OT_LOG_W("Document missing Name field, skipping");
				continue;
			}
			std::string name = doc["Name"].GetString();

			ot::LibraryModel model(name, "", "");

			if (doc.HasMember("additionalInfos") && doc["additionalInfos"].IsObject()) 	{
				ot::ConstJsonObject additionalInfos = doc["additionalInfos"].GetObject();

				for (auto it = additionalInfos.MemberBegin(); it != additionalInfos.MemberEnd(); ++it) {
					std::string key = it->name.GetString();
					std::string value;

					if (it->value.IsString()) {
						value = it->value.GetString();
					}

					model.addMetaData(key, value);
					dialogCfg.addFilter(key);
				}
			}

			if (doc.HasMember("metaData") && doc["metaData"].IsObject()) {
				ot::ConstJsonObject metaDataObj = doc["metaData"].GetObject();

				for (auto it = metaDataObj.MemberBegin(); it != metaDataObj.MemberEnd(); ++it) {
					std::string key = it->name.GetString();
					std::string value;

					if (it->value.IsString()) {
						value = it->value.GetString();
					}

					model.addMetaData(key, value);
					dialogCfg.addFilter(key);
					
				}
			}

			dialogCfg.addModel(model);
		}
		dialogCfg.setName("Library Selection");
		dialogCfg.setTitle("Select Library Element");

		return dialogCfg;
	}

	OT_LOG_E("ModelInfoDoc is not an object: Failed to get model infos");
	return std::nullopt;
}

 std::string Application::sendConfigToUI(const ot::JsonDocument& _doc, const std::string& _uiUrl) {
	 std::string uiResponse;


	 // In case of error:
		 // Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		 // Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	 const int maxCt = 30;
	 int ct = 1;
	 bool ok = false;

	 do {
		 uiResponse.clear();
		 if (!(ok = ot::msg::send(this->getServiceURL(), _uiUrl, ot::QUEUE, _doc.toJson(), uiResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			 OT_LOG_E("Request create dialog failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			 using namespace std::chrono_literals;
			 std::this_thread::sleep_for(500ms);

		 }
	 } while (!ok && ct++ <= maxCt);

	 if (!ok) {
		 OT_LOG_E("Request create dialog failed");
		 return {};
	 }

	 return uiResponse;
 }

 std::string Application::sendMessageToModel(const ot::JsonDocument& _doc, const std::string& _modelUrl) {
	 std::string modelResponse;


	 // In case of error:
		 // Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		 // Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	 const int maxCt = 30;
	 int ct = 1;
	 bool ok = false;

	 do {
		 modelResponse.clear();
		 if (!(ok = ot::msg::send(this->getServiceURL(), _modelUrl, ot::EXECUTE, _doc.toJson(), modelResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			 OT_LOG_E("Request create dialog failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			 using namespace std::chrono_literals;
			 std::this_thread::sleep_for(500ms);

		 }
	 } while (!ok && ct++ <= maxCt);

	 if (!ok) {
		 OT_LOG_E("Request create dialog failed");
		 return {};
	 }

	 return modelResponse;
 }

void Application::packMetaData(const bsoncxx::document::view& _doc, ot::LibraryModel& _model, ot::ModelLibraryDialogCfg& _dialogCfg) {
	if (_doc["Parameters"] && _doc["Parameters"].type() == bsoncxx::type::k_document) {
		auto paramsDoc = _doc["Parameters"].get_document().value;

		for (auto&& element : paramsDoc) {
			if (element.type() == bsoncxx::type::k_double) {
				std::string key = std::string(element.key());
				std::string value = std::to_string(element.get_double().value);

				// Here we add the meta data to the model object
				_model.addMetaData(key, value);

				// Here we add the filter options to the cfg
				_dialogCfg.addFilter(key);
			}
		}

		// Now we add the full model object to the dialogCfg
		_dialogCfg.addModel(_model);
	}
}


// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

std::string Application::handleGetDocument(ot::JsonDocument& _document) {

	std::string collectionName= ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string fieldType = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	auto result = db->getDocument(collectionName, fieldType, value,dbUserName,dbUserPassword,dbServerUrl);
	if (!result.empty()) {	
		return ot::ReturnMessage(ot::ReturnMessage::Ok, result).toJson();
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
	}


}


std::string Application::handleGetListOfDocuments(ot::JsonDocument& _document) {

	//std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	//std::string fieldType = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	//std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	//std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	//std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	//std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	//auto result = db->getDocumentList(collectionName, fieldType, value, dbUserName, dbUserPassword, dbServerUrl);
	//if (!result.empty()) {
	//	return ot::ReturnMessage(ot::ReturnMessage::Ok, result).toJson();
	//}
	//else {
	//	return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
	//}
	return ot::ReturnMessage(ot::ReturnMessage::Ok).toJson();
}

std::string Application::handleCreateDialogConfig(ot::JsonDocument& _document) {
	ot::ConstJsonObject configObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::LibraryElementSelectionCfg selectionCfg;
	selectionCfg.setFromJsonObject(configObj);

	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	ot::ModelLibraryDialogCfg modelDialogCfg;

	auto result = createModelLibraryDialogCfg(selectionCfg, dbUserName, dbUserPassword, dbServerUrl);
	if (result) {
		modelDialogCfg = result.value();
	}	
	

	ot::JsonDocument modelCfgDoc;
	ot::JsonObject modelCfg;
	modelDialogCfg.addToJsonObject(modelCfg, modelCfgDoc.GetAllocator());

	modelCfgDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ModelDialog, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(this->getServiceURL(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, selectionCfg.getRequestingEntityID(), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(selectionCfg.getCollectionName(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(selectionCfg.getNewEntityFolder(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_ElementType, ot::JsonString("Diode", modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(selectionCfg.getCallBackService(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(dbUserName, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(dbUserPassword, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(dbServerUrl, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());

	modelCfgDoc.AddMember(OT_ACTION_PARAM_Config, modelCfg,modelCfgDoc.GetAllocator());
	
	//Send config to UI
	std::string uiResponse = sendConfigToUI(modelCfgDoc, selectionCfg.getUIServiceUrl());
	
	return uiResponse;

	return "";
}

std::string Application::handleModelDialogConfirmed(ot::JsonDocument& _document) {
	//// Adding fieldtype value for GetDocument
	//_document.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("Name", _document.GetAllocator()), _document.GetAllocator());

	//// Get model description
	//std::string modelInfo = handleGetDocument(_document);
	//ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(modelInfo);
	//if (rMsg != ot::ReturnMessage::Ok) {
	//	OT_LOG_E("Get Models failed: " + rMsg.getWhat());
	//	return "Failed";
	//}

	//// Getting relevant infos

	//std::string selectedModel = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	//ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	//std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	//std::string targetFolder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	//std::string elementType = ot::json::getString(_document, OT_ACTION_PARAM_ElementType);
	//std::string modelUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);



	//


	//
	//// Creating dialog confirmed doc and add the model description 
	//ot::JsonDocument dialogConfirmed;
	//dialogConfirmed.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ModelDialogConfirmed, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityID, dialogConfirmed.GetAllocator());
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(targetFolder, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_ElementType, ot::JsonString(elementType, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(selectedModel, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//
	//// Add the model info got from database
	//dialogConfirmed.AddMember(OT_ACTION_PARAM_ModelInfo, ot::JsonString(rMsg.getWhat(), dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	//std::string response = sendMessageToModel(dialogConfirmed, modelUrl);

	return "";

}

std::string Application::handleModelDialogCanceled(ot::JsonDocument& _document) {
	
	/*ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	std::string modelUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);

	ot::JsonDocument dialogCanceled;
	dialogCanceled.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ModelDialogCanceled, dialogCanceled.GetAllocator()), dialogCanceled.GetAllocator());
	dialogCanceled.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityID, dialogCanceled.GetAllocator());

	std::string response = sendMessageToModel(dialogCanceled, modelUrl);*/
	return "";
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LibraryManagementService, OT_INFO_SERVICE_TYPE_LibraryManagementService) {

}

Application::~Application() {
	delete db;
	db = nullptr;
}

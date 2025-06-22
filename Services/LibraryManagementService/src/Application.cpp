//! @file Application.cpp
//! @author Sebastian Urmann
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Application.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCore/ReturnMessage.h"






//std header
#include <chrono>
#include <thread>

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public functions

int Application::initialize(const char* _siteID,const char* _ownURL, const char* _globalSessionServiceURL, const char * _databasePWD) {
	try {
		OT_LOG_I("Library Management Service initialization");
		setSiteId(_siteID);
		setServiceURL(_ownURL);
		// Now store the command line arguments and perform the initialization
		if (_ownURL == nullptr) {
			exit(ot::AppExitCode::ServiceUrlInvalid);
		}
		if (_globalSessionServiceURL == nullptr) {
			exit(ot::AppExitCode::GSSUrlMissing);
		}

		std::string ownUrl(_ownURL);

		// Register at GSS
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLibraryManagementService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL, ot::JsonString(ownUrl, gssDoc.GetAllocator()), gssDoc.GetAllocator());

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
			if (!(ok = ot::msg::send(ownUrl, _globalSessionServiceURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
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

std::string Application::getModelInformation(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	auto result = db->getDocumentList(_collectionName, _fieldType, _value, _dbUserName, _dbUserPassword, _dbServerUrl);
	return result;
}

std::string Application::getModelMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	auto result = db->getMetaData(_collectionName, _fieldType, _value, _dbUserName, _dbUserPassword, _dbServerUrl);
	return result;
}

void Application::createModelDialogCfg(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	
	// First get model info from database
	std::string modelInfos = getModelInformation(_collectionName, _fieldType, _value, _dbUserName, _dbUserPassword, _dbServerUrl);

	ot::JsonDocument modelInfosDoc;
	modelInfosDoc.fromJson(modelInfos);


	if (modelInfosDoc.IsObject()) {
		ot::ConstJsonObject obj = modelInfosDoc.GetConstObject();
		ot::ConstJsonArray docs = obj["Documents"].GetArray();

		for (const ot::JsonValue& val : docs) {
			ot::ConstJsonObject doc = val.GetObject();
			std::string name = doc["Name"].GetString();
			std::string fileName = doc["Filename"].GetString();
			std::string modelType = doc["ModelType"].GetString();
			std::string elementType = doc["ElementType"].GetString();
			std::string metaDataId;

			// Check if meta data exists
			if (doc.HasMember("MetaDataID")) {
				ot::ConstJsonObject metaDataIdObj = doc["MetaDataID"].GetObject();
				if (metaDataIdObj.HasMember("$oid")) {
					metaDataId = metaDataIdObj["$oid"].GetString();
				}
			}

			// Create library model object
			ot::LibraryModel model(name, fileName, modelType, elementType);

			// Get the meta data
			std::string metaDataInfos = getModelMetaData(circuitMetaDataCollection, "_id", metaDataId, _dbUserName, _dbUserPassword, _dbServerUrl);

			auto metaDoc = bsoncxx::from_json(metaDataInfos);
	

			// Fill object with meta data
			extractMetaToMap(metaDoc.view(), model);

			// Library model is filled now fill the model dialog cfg

			

		}
	}

}

void Application::extractMetaToMap(const bsoncxx::document::view& _doc, ot::LibraryModel& _model) {
	if (_doc["Parameters"] && _doc["Parameters"].type() == bsoncxx::type::k_document) {
		auto paramsDoc = _doc["Parameters"].get_document().value;

		for (auto&& element : paramsDoc) {
			if (element.type() == bsoncxx::type::k_double) {
				std::string key = std::string(element.key());
				std::string value = std::to_string(element.get_double().value);
				_model.addData(key, value);
			}
		}
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

	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string fieldType = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	auto result = db->getDocumentList(collectionName, fieldType, value, dbUserName, dbUserPassword, dbServerUrl);
	if (!result.empty()) {
		return ot::ReturnMessage(ot::ReturnMessage::Ok, result).toJson();
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
	}


}

std::string Application::handleCreateDialogConfig(ot::JsonDocument& _document) {

	std::string entityID = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityID);
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string targetFolder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string elementType = ot::json::getString(_document, OT_ACTION_PARAM_ElementType);
	std::string uiUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	//First check if model request is circuit model
	if (!elementType.empty()) {
		createModelDialogCfg(collectionName, "ElementType", elementType, dbUserName, dbUserPassword, dbServerUrl);
	}


	// get the metaData from db
	
	

	//Create model dialog config with metadata
	

	// send Config to UI




	return ot::ReturnMessage(ot::ReturnMessage::Ok).toJson();


}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LibraryManagementService, OT_INFO_SERVICE_TYPE_LibraryManagementService) {

}

Application::~Application() {}

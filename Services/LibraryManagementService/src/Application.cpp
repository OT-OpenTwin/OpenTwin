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
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Dispatch/ActionDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTModelEntities/Lms/LibraryElement.h"
#include "OTModelEntities/Lms/LibraryElementRequest.h"
#include "OTModelEntities/Lms/UserLibraryElement.h"
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/FileSystem/DirectoryIterator.h"
#include "OTSystem/FileSystem/AdvancedDirectoryIterator.h"
#include "OTGuiAPI/Frontend.h"

//std header
#include <chrono>
#include <thread>
#include <optional>

// Third party header
#include <QtCore/qcryptographichash.h>

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public functions

int Application::initialize(const char* _siteID,const char* _ownURL, const char* _globalSessionServiceURL, const char * _databasePWD) {

	//! @brief Initialization of the service. This includes:
	//! - Setting up the db url by requesting it from the GSS and setting it in the db wrapper
	//! - Getting and setting the build information from the GSS
	//! - Updating the model library if necessary by comparing the build information from the GSS with the one stored in the database. If they differ, the model library will be updated and the new build information will be stored in the database.
	//! - Registration in the GSS 


	// In case of error:
	// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
	// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;


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
		db.setSiteID(_siteID);

		// Admin credentials for database operations
		std::string adminUserName = db.getAdminUserName();
		std::string adminPassword = _databasePWD;
		if (!adminPassword.empty()) {
			adminPassword = std::string(_databasePWD);
		}
		else {
			adminPassword = ot::UserCredentials::encryptString(db.getAdminUserName());
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Get and set the database URL

		std::string dbUrl;
		std::string gssResponseDbUrl;
		ot::JsonDocument dbUrlReqDoc;
		dbUrlReqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetGlobalServicesUrl, dbUrlReqDoc.GetAllocator()), dbUrlReqDoc.GetAllocator());

		do {
			gssResponseDbUrl.clear();
			if (!(ok = ot::msg::send(getServiceURL(), _globalSessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, dbUrlReqDoc.toJson(), gssResponseDbUrl, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
				OT_LOG_E("Failed to get database URL from GSS [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(500ms);
			}
		} while ((ot::ReturnMessage::fromJson(gssResponseDbUrl) != ot::ReturnMessage::Ok || !ok) && ct++ <= maxCt);

		if (!ok) {
				OT_LOG_E("Failed to get database URL from GSS after " + std::to_string(maxCt) + " attempts. Exiting...");
				exit(ot::AppExitCode::GeneralError);
		}

		ot::ReturnMessage dbUrlResponseMsg = ot::ReturnMessage::fromJson(gssResponseDbUrl);
		if (dbUrlResponseMsg != ot::ReturnMessage::Ok) {
			OT_LOG_E("Failed to get database URL from GSS: " + dbUrlResponseMsg.getWhat());
			exit(ot::AppExitCode::GeneralError);
		}

		ot::JsonDocument dbUrlResponseDoc;
		dbUrlResponseDoc.fromJson(dbUrlResponseMsg.getWhat());
		dbUrl = ot::json::getString(dbUrlResponseDoc, OT_ACTION_PARAM_SERVICE_DBURL);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		// Reset attempt counter for further requests to GSS and reset ok for further checks
		ct = 1;
		ok = false;

		// Get and set build information
		std::string gssResponseBuildInformation;
		ot::JsonDocument buildReqDoc;
		buildReqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetBuildInformation, buildReqDoc.GetAllocator()), buildReqDoc.GetAllocator());
		
		do {
			gssResponseBuildInformation.clear();
			if (!(ok = ot::msg::send(getServiceURL(), _globalSessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, buildReqDoc.toJson(), gssResponseBuildInformation, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
				OT_LOG_E("Failed to get build information from GSS [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(500ms);
			}
		} while (!ok && ct++ <= maxCt);

		if (!ok) {
			OT_LOG_E("Failed to get build information from GSS after " + std::to_string(maxCt) + " attempts. Exiting...");
			exit(ot::AppExitCode::GeneralError);
		}

		if (gssResponseBuildInformation.empty()) {
			OT_LOG_E("Received empty build information from GSS");
		}
		std::string buildInfoResponseStrGss = gssResponseBuildInformation;
	

		// Get the stored build information from the database
		std::string storedBuildInformation = db.getBuildInformation(db.getAdminUserName(),ot::UserCredentials::decryptString(adminPassword), dbUrl);
		if(storedBuildInformation != buildInfoResponseStrGss) {
			OT_LOG_I("Build information has changed. Updating database...");
			if(db.setBuildInformation(db.getAdminUserName(), ot::UserCredentials::decryptString(adminPassword), dbUrl, buildInfoResponseStrGss)) {
				bool updateSuccess = launchModelLibraryUpdate(getServiceURL(), adminPassword);
				if(updateSuccess) {
					OT_LOG_I("Model library update completed successfully.");
				}
				else {
					OT_LOG_E("Model library update failed. Please check the logs for more details.");
				}
			}
			else {
				OT_LOG_E("Failed to update build information in the database.");
			}
		}
		else {
			OT_LOG_I("Build information is up to date. No update needed.");
		}
	}
	catch (std::exception& e) {
		OT_LOG_E(std::string{"Uncaught exception: "}.append(e.what()));
		exit(ot::AppExitCode::GeneralError);
	}
	catch (...) {
		assert(0);
		exit(ot::AppExitCode::UnknownError);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Register at GSS
	std::string gssResponseRegister;
	ot::JsonDocument gssDocRegister;
	gssDocRegister.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLibraryManagementService, gssDocRegister.GetAllocator()), gssDocRegister.GetAllocator());
	gssDocRegister.AddMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL, ot::JsonString(getServiceURL(), gssDocRegister.GetAllocator()), gssDocRegister.GetAllocator());


	do {
		gssResponseRegister.clear();
		if (!(ok = ot::msg::send(getServiceURL(), _globalSessionServiceURL, ot::EXECUTE, gssDocRegister.toJson(), gssResponseRegister, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Register at Global Session Service failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while ((ot::ReturnMessage::fromJson(gssResponseRegister) != ot::ReturnMessage::Ok || !ok) && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Registration at Global Session service failed after " + std::to_string(maxCt) + " attempts. Exiting...");
		exit(ot::AppExitCode::GSSRegistrationFailed);
	}

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(gssResponseRegister);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Registration failed: " + rMsg.getWhat());
		exit(ot::AppExitCode::GSSRegistrationFailed);
	}

	ot::DebugHelper::serviceSetupCompleted(*this);
	return ot::AppExitCode::Success;
}

void Application::promptUserForLibraryElementOverwrite(const ot::UserLibraryElement& _element, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _uiServiceUrl) {
	// Serialize the element to JSON and add database credentials
	ot::JsonDocument configDoc;
	ot::JsonObject elementObj;
	_element.addToJsonObject(elementObj, configDoc.GetAllocator());

	// Add database credentials to the prompt data
	configDoc.AddMember("element", elementObj, configDoc.GetAllocator());
	configDoc.AddMember("dbUserName", ot::JsonString(_dbUserName, configDoc.GetAllocator()), configDoc.GetAllocator());
	configDoc.AddMember("dbUserPassword", ot::JsonString(_dbUserPassword, configDoc.GetAllocator()), configDoc.GetAllocator());
	configDoc.AddMember("dbServerUrl", ot::JsonString(_dbServerUrl, configDoc.GetAllocator()), configDoc.GetAllocator());

	std::string promptJson = configDoc.toJson();

	ot::JsonDocument promptDoc;
	promptDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptInformation, promptDoc.GetAllocator()), promptDoc.GetAllocator());

	ot::MessageDialogCfg config;
	config.setText("The library element \"" + _element.getName() + "\" already exists.\n\nDo you want to overwrite it?");
	config.setIcon(ot::MessageDialogCfg::Question);
	config.setButtons(ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);
	config.setTitle("OpenTwin");

	ot::JsonObject promptConfigObj;
	config.addToJsonObject(promptConfigObj, promptDoc.GetAllocator());
	promptDoc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(this->getServiceURL(), promptDoc.GetAllocator()), promptDoc.GetAllocator());
	promptDoc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(c_promptActionOverwriteUserLibraryElement, promptDoc.GetAllocator()), promptDoc.GetAllocator());
	promptDoc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(config, promptDoc.GetAllocator()), promptDoc.GetAllocator());
	promptDoc.AddMember(OT_ACTION_PARAM_Info, ot::JsonString(promptJson, promptDoc.GetAllocator()), promptDoc.GetAllocator());
	
	// Send the prompt to the UI service
	sendConfigToUI(promptDoc, _uiServiceUrl);
}

std::string Application::generateUniqueElementName(const std::string& _baseName, const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	std::string uniqueName = _baseName;
	int counter = 1;

	// Ensure db / collection exists (once per collection)
	if (!db.ensureDatabaseAndCollection(_collectionName, _dbUserName, _dbUserPassword, _dbServerUrl)) {
		OT_LOG_E("Failed to ensure database and collection '" + _collectionName + "'");
	}
	OT_LOG_I("Database and collection '" + _collectionName + "' are ready");

	// Keep trying names with incrementing suffix until we find one that doesn't exist
	while (true) {
		std::string candidateName = _baseName + "_" + std::to_string(counter);

		// Check if this name already exists in the collection
		std::string existingDocJson = db.getCompleteDocument(_collectionName, _dbUserName, _dbUserPassword, _dbServerUrl, candidateName);

		if (existingDocJson.empty()) {
			// Name doesn't exist, we can use it
			return candidateName;
		}

		counter++;
	}
}

bool Application::launchModelLibraryUpdate(const std::string& _ownURL, const std::string& _databasePWD) {

	OT_LOG_I("Launching Model Library Updater");

	std::string lmsUrl = _ownURL;
	std::string databasePassword = _databasePWD;
	std::string libraryDataBasePath;


#ifdef _DEBUG
	libraryDataBasePath = ot::OperatingSystem::getEnvironmentVariableString("OPENTWIN_DEV_ROOT") + "\\LibraryData";
	OT_LOG_D("Using development LibraryData path: " + libraryDataBasePath);
#else
	libraryDataBasePath = ot::OperatingSystem::getCurrentExecutableDirectory() + "\\LibraryData";
#endif

	OT_LOG_D("Determined library data path: " + libraryDataBasePath);
	OT_LOG_D("Using LMS URL: " + lmsUrl);

	if (databasePassword.empty()) {
		OT_LOG_W("No database password provided. Attempting to use empty password for database operations.");
	}
	else {
		OT_LOG_D("Database password provided via command line argument.");
	}

	if (libraryDataBasePath.empty()) {
		OT_LOG_E("Failed to determine library data path. Aborting Model Library Updater launch.");
		return false;
	}

	if (lmsUrl.empty()) {
		OT_LOG_E("Service URL is empty. Aborting Model Library Updater launch.");
		return false;
	}

	// Collections to update - can be extended if needed
	std::list<std::string> collections = { "PythonEnvironments", "PythonScripts", "CircuitData"};

	// Resolve DB address and decrypted admin password once
	const std::string dbAddress = ot::OperatingSystem::getEnvironmentVariableString("OPEN_TWIN_MONGODB_ADDRESS");
	const std::string adminUserName = db.getAdminUserName();
	const std::string adminPasswordPlain = ot::UserCredentials::decryptString(databasePassword);

	for (const auto& collectionName : collections) {
		const std::string libraryDataPath = libraryDataBasePath + "\\" + collectionName;

		std::list<ot::LibraryElement> localModels = getLocalModels(libraryDataPath, collectionName);
		if (localModels.empty()) {
			OT_LOG_DS("No local models found in the specified folder: " << libraryDataPath);
			OT_LOG_DS("Skipping LMS update and creation process for " << collectionName);
			continue;
		}

		// Ensure db / collection exists (once per collection)
		if (!db.ensureDatabaseAndCollection(collectionName, adminUserName, adminPasswordPlain, dbAddress)) {
			OT_LOG_E("Failed to ensure database and collection '" + collectionName + "'");
			continue;
		}
		OT_LOG_I("Database and collection '" + collectionName + "' are ready");

		std::list<std::shared_ptr<ot::LibraryElement>> localPtrModels;
		for (auto& model : localModels) {
			localPtrModels.push_back(std::make_shared<ot::LibraryElement>(std::move(model)));
		}

		// Filter out models that are already up-to-date and get their existence status
		LibraryElementExistenceStatus updateStatus = updateOrCreateLibraryElement(localPtrModels, adminUserName, adminPasswordPlain, dbAddress);

		// If no models remain after the check, nothing to do
		if (localPtrModels.empty()) {
			OT_LOG_I("No models needed to be updated or created for collection: " + collectionName);
			OT_LOG_D("Last element status: " + std::to_string(static_cast<int>(updateStatus)));
			continue;
		}

		// Attach binary content from disk
		addDataToLibraryElements(localPtrModels, libraryDataPath);

		// Add or update remaining library elements in DB
		addLibraryElement(localPtrModels, adminUserName, adminPasswordPlain, dbAddress);
		OT_LOG_D("Completed add/update for collection: " + collectionName);
	}

	return true;
}

std::list<ot::LibraryElement> Application::getLocalModels(const std::string& _modelFolderPath, const std::string& _collectionName) {
	std::list<ot::LibraryElement> localModels;
	try {

		if (!std::filesystem::exists(_modelFolderPath)) {
			return {};
		}

		ot::IgnoreRules ignoreRules;
		ignoreRules.addRule("*");
		ignoreRules.addRule("!*.otmeta.json");

		ot::AdvancedDirectoryIterator directoryIterator(_modelFolderPath, ot::DirectoryIterator::Files, ignoreRules);

		// Iterate through all
		while (directoryIterator.hasNext()) {
			ot::FileInformation fileInfo = directoryIterator.next();
			const std::string filePath = fileInfo.getPath().string();

			try {
				std::ifstream file(filePath);
				if (!file) {
					OT_LOG_E("Cannot open file: " + filePath);
					continue;
				}

				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string content = buffer.str();
				file.close();

				// Deserialize the content into a LibraryElement
				ot::LibraryElement element = ot::LibraryElement::fromJson(content);
				element.setCollectionName(_collectionName);
				// Fill library element with hash value calculated from content file
				fillLibraryElementWithHash(element, _modelFolderPath);

				// Add the element to the list of local models
				localModels.push_back(element);

			}
			catch (const std::exception& e) {
				OT_LOG_E("Error processing .otmeta.json file '" + filePath + "': " + std::string(e.what()));
				continue;
			}
		}
	}
	catch (const std::exception& e) {
		OT_LOG_E("Error while reading local models: " + std::string(e.what()));
		return {};
	}
	return localModels;	
}

void Application::fillLibraryElementWithHash(ot::LibraryElement& _element, const std::string& _modelFolderPath) {
	// Get content file to calculate hash and set it to LibraryElement
	std::string contentFileName = _element.getFileName();
	if (_modelFolderPath.empty()) {
		std::string data = _element.getData();
		QCryptographicHash hashCalculator(QCryptographicHash::Algorithm::Md5);
		hashCalculator.addData(QByteArrayView(data.data(), data.size()));
		std::string hashValue = hashCalculator.result().toHex().toStdString();
		_element.setHash(hashValue);
	}
	else {
		std::filesystem::path contentFilePath = std::filesystem::path(_modelFolderPath) / contentFileName;
		if (std::filesystem::exists(contentFilePath)) {
			try {
				std::ifstream contentFile(contentFilePath, std::ios::binary);
				if (!contentFile) {
					OT_LOG_E("Cannot open content file: " + contentFilePath.string());
				}
				else {
					std::stringstream contentBuffer;
					contentBuffer << contentFile.rdbuf();
					std::string fileContent = contentBuffer.str();
					contentFile.close();

					// Calculate hash from content file and set it to LibraryElement
					QCryptographicHash hashCalculator(QCryptographicHash::Algorithm::Md5);
					hashCalculator.addData(QByteArrayView(fileContent.data(), fileContent.size()));
					std::string hashValue = hashCalculator.result().toHex().toStdString();
					_element.setHash(hashValue);
				}
			}
			catch (const std::exception& e) {
				OT_LOG_E("Error reading content file '" + contentFilePath.string() + "': " + std::string(e.what()));
			}
		}
	}
}

void Application::addDataToLibraryElements(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _modelFolderPath) {
	for (auto& ptrModel : _elements) {
		std::string contentFileName = ptrModel->getFileName();
		std::filesystem::path contentFilePath = std::filesystem::path(_modelFolderPath) / contentFileName;
		if (std::filesystem::exists(contentFilePath)) {
			try {
				std::ifstream contentFile(contentFilePath, std::ios::binary);
				if (!contentFile) {
					OT_LOG_E("Cannot open content file: " + contentFilePath.string());
				}
				else {
					std::stringstream contentBuffer;
					contentBuffer << contentFile.rdbuf();
					std::string fileContent = contentBuffer.str();
					contentFile.close();
					// Set the file content to the LibraryElement
					ptrModel->setData(fileContent);
				}
			}
			catch (const std::exception& e) {
				OT_LOG_E("Error reading content file '" + contentFilePath.string() + "': " + std::string(e.what()));
			}
		}
		else {
			OT_LOG_E("Content file does not exist: " + contentFilePath.string());
		}
	}
}

std::string Application::getModelInformation(const ot::LibraryElementSelectionCfg& _selectionCfg, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	auto result = db.getDocumentList(_selectionCfg, _dbUserName, _dbUserPassword, _dbServerUrl);
	return result;
}

Application::LibraryElementExistenceStatus Application::updateOrCreateLibraryElement(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, bool _dependencyCheck) {

	LibraryElementExistenceStatus lastStatus = LibraryElementExistenceStatus::NotExisting;

	for (auto it = _elements.begin(); it != _elements.end();) {
		// Get the collection name from the current element
		std::string collectionName = (*it)->getCollectionName();
		std::string elementName = (*it)->getName();

		if (_dependencyCheck) {
			// Check if additional dependency exists if not then skip 
			std::string dependencyID = (*it)->getAdditionalInfoValue("DependencyID");
			std::string dependencyCollection = (*it)->getAdditionalInfoValue("DependencyCollection");

			if (dependencyID != std::to_string(ot::invalidUID) && !dependencyID.empty() && !dependencyCollection.empty()) {
				std::string dependencyDocJson = db.getCompleteDocument(dependencyCollection, _dbUserName, _dbUserPassword, _dbServerUrl, dependencyID);
				if (dependencyDocJson.empty()) {
					OT_LOG_E("No dependency document found for element '" + elementName + "' with dependency ID '" + dependencyID + "' in collection '" + dependencyCollection + "'. Skipping this element.");
					it = _elements.erase(it);
					continue;
				}
				else if (dependencyDocJson == "failed") {
					OT_LOG_E("Failed to fetch dependency document for element '" + elementName + "' with dependency ID '" + dependencyID + "' in collection '" + dependencyCollection + "'. Skipping this element.");
					++it;
					continue;
				}
			}
		}
		
		// Try to fetch the existing document from database
		std::string existingDocJson = db.getCompleteDocument(collectionName, _dbUserName, _dbUserPassword, _dbServerUrl, elementName);

		if (existingDocJson == "failed") {
			OT_LOG_E("Failed to fetch existing document for element '" + elementName + "' in collection '" + collectionName + "'. Skipping this element.");
			lastStatus = LibraryElementExistenceStatus::Error;
			++it;
			continue;
		}

		if (!existingDocJson.empty()) {
			// Element exists in database - compare hashes
			try {
				ot::JsonDocument existingDoc;
				existingDoc.fromJson(existingDocJson);

				// Get hash from database document
				std::string dbHash = ot::json::getString(existingDoc, "Hash");

				// Get hash from current element
				std::string currentHash = (*it)->getHash();

				// If hashes match, element exists with identical content
				if (dbHash == currentHash) {
					lastStatus = LibraryElementExistenceStatus::ExistingWithIdenticalContent;
					it = _elements.erase(it);
					continue;
				}
				else {
					// Hashes differ - element exists with different content
					lastStatus = LibraryElementExistenceStatus::ExistingWithDifferentContent;
				}
			}
			catch (const std::exception& e) {
				OT_LOG_W("Error comparing hashes for element '" + elementName + "': " + std::string(e.what()));
				lastStatus = LibraryElementExistenceStatus::Error;
			}
		}
		else {
			// Element does not exist
			lastStatus = LibraryElementExistenceStatus::NotExisting;
		}

		++it;
	}

	return lastStatus;
}

void Application::addLibraryElement(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
	// Process each received model
	for (auto& model : _elements) {
		std::string collectionName = model->getCollectionName();
		std::string elementName = model->getName();

		// Try to fetch existing document from database
		std::string existingDocJson = db.getCompleteDocument(collectionName, _dbUserName, _dbUserPassword, _dbServerUrl, elementName);

		uint32_t newVersion = 1;

		// If document exists, increment version
		if (!existingDocJson.empty()) {
			ot::JsonDocument existingDoc;
			existingDoc.fromJson(existingDocJson);

			// Get current version and increment
			if (existingDoc.HasMember("Version") && existingDoc["Version"].IsUint()) {
				newVersion = existingDoc["Version"].GetUint() + 1;
			}
		}

		// Set the new version in the model
		model->setVersion(newVersion);

		// Migrate/update data to GridFS
		if (!existingDocJson.empty()) {
			// Update existing data to GridFS and update metadata (version, hash)
			std::string gridfsIdResult = db.updateGridFSAndMetadata(collectionName, _dbUserName, _dbUserPassword, _dbServerUrl, elementName, newVersion, model->getHash(), model->getData());

			if (!gridfsIdResult.empty()) {
				OT_LOG_I("Successfully updated document '" + elementName + "' with new GridFS ID: " + gridfsIdResult);
			}
			else {
				OT_LOG_E("Failed to update document '" + elementName + "'");
			}
		}
		else {
			// Migrate new entry data to GridFS
			db.addNewDocument(collectionName, _dbUserName, _dbUserPassword, _dbServerUrl, *model);
		}
	}
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

			

			if (doc.HasMember("metaData") && doc["metaData"].IsObject()) {
				ot::ConstJsonObject metaDataObj = ot::json::getObject(doc, "metaData");

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

 std::string Application::sendAsyncMessageToModel(const ot::JsonDocument& _doc, const std::string& _modelUrl) {
	 ot::msg::sendAsync(this->getServiceURL(), _modelUrl, ot::EXECUTE, _doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);

	 // sendAsync is fire-and-forget, so we return an empty string or a status
	 return "";
 }

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

std::string Application::handleGetCompleteSelectedDocument(ot::JsonDocument& _document) {

	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string selectedDocument = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	auto result = db.getCompleteDocument(collectionName,dbUserName,dbUserPassword,dbServerUrl,selectedDocument);
	if (!result.empty()) {	
		return ot::ReturnMessage(ot::ReturnMessage::Ok, result).toJson();
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
	}
}

std::string Application::handleCreateDialogConfig(ot::JsonDocument& _document) {
	ot::ConstJsonObject configObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::LibraryElementSelectionCfg selectionCfg;
	selectionCfg.setFromJsonObject(configObj);

	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);
	std::string callbackService = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string uiServiceUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	selectionCfg.setUIServiceUrl(uiServiceUrl);
	selectionCfg.setCallBackService(callbackService);
	selectionCfg.serializeCallbackInfoToAdditionalInfo();


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
	modelCfgDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(dbUserName, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(dbUserPassword, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(dbServerUrl, modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_Info, ot::JsonString(selectionCfg.getAdditionalInfo(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_Config, modelCfg,modelCfgDoc.GetAllocator());
	modelCfgDoc.AddMember(OT_ACTION_PARAM_PROPERTY_Name, ot::JsonString(selectionCfg.getPropertyName(), modelCfgDoc.GetAllocator()), modelCfgDoc.GetAllocator());
	//Send config to UI
	std::string uiResponse = sendConfigToUI(modelCfgDoc, selectionCfg.getUIServiceUrl());
	
	return uiResponse;
}

std::string Application::handleModelDialogConfirmed(ot::JsonDocument& _document) {
	
	// Get model description from database
	std::string modelInfo = handleGetCompleteSelectedDocument(_document);
	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(modelInfo);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return "Failed";
	}

	// Convert model info from database to json doc
	ot::JsonDocument modelInfoDoc;
	modelInfoDoc.fromJson(rMsg.getWhat());
	
	// Get the additional info string which contains serialized callback data
	std::string additionalInfo = ot::json::getString(_document, OT_ACTION_PARAM_Info);
	
	// Deserialize the callback information
	ot::JsonDocument callbackInfoDoc;
	callbackInfoDoc.fromJson(additionalInfo);
	
	modelInfoDoc.AddMember("CollectionName", ot::JsonString(ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("RequestingEntityID", ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("CallbackService", ot::JsonString(ot::json::getString(callbackInfoDoc, "CallbackService"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("EntityType", ot::JsonString(ot::json::getString(callbackInfoDoc, "EntityType"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("NewEntityFolder", ot::JsonString(ot::json::getString(callbackInfoDoc, "NewEntityFolder"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("PropertyName", ot::JsonString(ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Name), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());

	// Create LibraryElementImportCfg and populate it with the complete modelInfoDoc
	ot::LibraryElement importCfg;
	importCfg.setFromJsonObject(modelInfoDoc.getConstObject());
	importCfg.toJson();

	// Creating dialog confirmed doc with the import config
	ot::JsonDocument dialogConfirmed;
	dialogConfirmed.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ModelDialogConfirmed, dialogConfirmed.GetAllocator()), dialogConfirmed.GetAllocator());
	
	// Add the complete import configuration as a JSON object
	ot::JsonObject importCfgObj;
	importCfg.addToJsonObject(importCfgObj, dialogConfirmed.GetAllocator());
	dialogConfirmed.AddMember(OT_ACTION_PARAM_Config, importCfgObj, dialogConfirmed.GetAllocator());
	
	std::string response = sendAsyncMessageToModel(dialogConfirmed, importCfg.getCallBackService());

	return "";
}

std::string Application::handleModelDialogCanceled(ot::JsonDocument& _document) {

	ot::JsonDocument modelInfoDoc;
	// Get the additional info string which contains serialized callback data
	std::string additionalInfo = ot::json::getString(_document, OT_ACTION_PARAM_Info);

	// Deserialize the callback information
	ot::JsonDocument callbackInfoDoc;
	callbackInfoDoc.fromJson(additionalInfo);
	modelInfoDoc.AddMember("CollectionName", ot::JsonString(ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("RequestingEntityID", ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("CallbackService", ot::JsonString(ot::json::getString(callbackInfoDoc, "CallbackService"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("EntityType", ot::JsonString(ot::json::getString(callbackInfoDoc, "EntityType"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("NewEntityFolder", ot::JsonString(ot::json::getString(callbackInfoDoc, "NewEntityFolder"), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());
	modelInfoDoc.AddMember("PropertyName", ot::JsonString(ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Name), modelInfoDoc.GetAllocator()), modelInfoDoc.GetAllocator());

	// Create LibraryElementImportCfg and populate it with the complete modelInfoDoc
	ot::LibraryElement importCfg;
	importCfg.setFromJsonObject(modelInfoDoc.getConstObject());

	ot::JsonDocument dialogCanceled;
	dialogCanceled.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ModelDialogCanceled, dialogCanceled.GetAllocator()), dialogCanceled.GetAllocator());

	// Add the complete import configuration as a JSON object
	ot::JsonObject importCfgObj;
	importCfg.addToJsonObject(importCfgObj, dialogCanceled.GetAllocator());
	dialogCanceled.AddMember(OT_ACTION_PARAM_Config, importCfgObj, dialogCanceled.GetAllocator());


	std::string response = sendAsyncMessageToModel(dialogCanceled, importCfg.getCallBackService());
	return "";
}

std::string Application::handleLibraryElementRequest(ot::JsonDocument& _document) {

	// Get the request configuration
	ot::ConstJsonObject configObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::LibraryElementRequest requestConfig;
	requestConfig.setFromJsonObject(configObj);

	// Extract database credentials
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);
	std::string collectionName = requestConfig.getCollectionName();
	std::string environmentInfo = requestConfig.getValue();

	// Get the complete document from database
	auto documentResult = db.getCompleteDocument(collectionName, dbUserName, dbUserPassword, dbServerUrl, environmentInfo);

	if (documentResult.empty()) {
		OT_LOG_E("Failed to load document: " + environmentInfo + " from collection: " + collectionName);
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Document not found").toJson();
	}

	// Convert result to JSON document
	ot::JsonDocument loadedDoc;
	loadedDoc.fromJson(documentResult);

	// Add the request information to the loaded document
	loadedDoc.AddMember("RequestingEntityID", requestConfig.getRequestingEntityID(), loadedDoc.GetAllocator());
	loadedDoc.AddMember("CollectionName", ot::JsonString(collectionName, loadedDoc.GetAllocator()), loadedDoc.GetAllocator());
	loadedDoc.AddMember("CallbackService", ot::JsonString(requestConfig.getCallBackService(), loadedDoc.GetAllocator()), loadedDoc.GetAllocator());
	loadedDoc.AddMember("EntityType", ot::JsonString(requestConfig.getEntityType(), loadedDoc.GetAllocator()), loadedDoc.GetAllocator());
	loadedDoc.AddMember("NewEntityFolder", ot::JsonString(requestConfig.getNewEntityFolder(), loadedDoc.GetAllocator()), loadedDoc.GetAllocator());
	loadedDoc.AddMember("PropertyName", ot::JsonString(requestConfig.getPropertyName(), loadedDoc.GetAllocator()), loadedDoc.GetAllocator());

	// Create LibraryElement and populate it with the complete loadedDoc
	ot::LibraryElement libraryElement;
	libraryElement.setFromJsonObject(loadedDoc.getConstObject());

	// Create the response document
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ModelDialogConfirmed, responseDoc.GetAllocator()), responseDoc.GetAllocator());

	// Add the complete library element as a JSON object
	ot::JsonObject libraryElementObj;
	libraryElement.addToJsonObject(libraryElementObj, responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Config, libraryElementObj, responseDoc.GetAllocator());

	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDoc).toJson();
}

std::string Application::handleAddUserLibraryElement(ot::JsonDocument& _document) {

	// Extract database credentials
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);
	std::string uiServiceUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);

	// Read incoming array (user library elements) and convert to LibraryElement
	ot::ConstJsonArray elementsArray = ot::json::getArray(_document, OT_ACTION_PARAM_Config);

	for (const ot::JsonValue& val : elementsArray) {
		if (!val.IsObject()) continue;

		ot::ConstJsonObject elementObj = val.GetObject();

		// Deserialize as UserLibraryElement to capture Owner and future fields
		ot::UserLibraryElement userElement;
		userElement.setFromJsonObject(elementObj);

		// Calculate hash
		fillLibraryElementWithHash(userElement, "");

		// Ensure DB/collection exists
		if (!db.ensureDatabaseAndCollection(userElement.getCollectionName(), dbUserName, dbUserPassword, dbServerUrl)) {
			OT_LOG_E("Failed to ensure database and collection '" + userElement.getCollectionName() + "'");
			continue;
		}
		OT_LOG_I("Database and collection '" + userElement.getCollectionName() + "' are ready");

		// Create a single-element list for the unified function
		std::list<std::shared_ptr<ot::LibraryElement>> singleElementList;
		singleElementList.push_back(std::make_shared<ot::UserLibraryElement>(userElement));

		// Check existence and filter using the unified function
		LibraryElementExistenceStatus existenceStatus = updateOrCreateLibraryElement(singleElementList, dbUserName, dbUserPassword, dbServerUrl, false);

		switch (existenceStatus) {
		case LibraryElementExistenceStatus::NotExisting:
		{
			// Element doesn't exist - add it directly
			OT_LOG_I("Adding new library element '" + userElement.getName() + "'");
			addLibraryElement(singleElementList, dbUserName, dbUserPassword, dbServerUrl);
			break;
		}

		case LibraryElementExistenceStatus::ExistingWithIdenticalContent:
		{
			// Element exists with identical content - skip it
			OT_LOG_I("Library element '" + userElement.getName() + "' already exists with identical content. Skipping.");
			break;
		}

		case LibraryElementExistenceStatus::ExistingWithDifferentContent:
		{
			// Element exists but content is different - restore and prompt user
			singleElementList.push_back(std::make_shared<ot::UserLibraryElement>(userElement));
			OT_LOG_I("Library element '" + userElement.getName() + "' exists with different content. Prompting user for overwrite...");
			promptUserForLibraryElementOverwrite(userElement, dbUserName, dbUserPassword, dbServerUrl, uiServiceUrl);
			break;
		}

		case LibraryElementExistenceStatus::Error:
		{
			// Error occurred
			OT_LOG_E("Error checking existence of library element '" + userElement.getName() + "'. Skipping.");
			break;
		}
		}
	}

	return ot::ReturnMessage(ot::ReturnMessage::Ok).toJson();
}

std::string Application::handleLibraryElementOverwritePromptResponse(ot::JsonDocument& _document) {
	ot::MessageDialogCfg::BasicButton result = ot::MessageDialogCfg::stringToButton(ot::json::getString(_document, OT_ACTION_PARAM_Result));
	std::string promptJsonString = ot::json::getString(_document, OT_ACTION_PARAM_Info);

	// Deserialize the prompt data
	ot::JsonDocument promptDoc;
	promptDoc.fromJson(promptJsonString);

	// Extract element and database credentials from the prompt data
	ot::ConstJsonObject elementObj = ot::json::getObject(promptDoc, "element");
	ot::UserLibraryElement element;
	element.setFromJsonObject(elementObj);

	std::string dbUserName = ot::json::getString(promptDoc, "dbUserName");
	std::string dbUserPassword = ot::json::getString(promptDoc, "dbUserPassword");
	std::string dbServerUrl = ot::json::getString(promptDoc, "dbServerUrl");

	std::string elementName = element.getName();
	std::string collectionName = element.getCollectionName();

	if ((result & ot::MessageDialogCfg::Yes) == ot::MessageDialogCfg::Yes) {
		// User wants to overwrite the existing element
		OT_LOG_I("User confirmed to overwrite library element: " + elementName);

		// When overwriting increase the version number by 1
		element.setVersion(element.getVersion() + 1);

		std::list<std::shared_ptr<ot::LibraryElement>> singleElementPtrList;
		singleElementPtrList.push_back(std::make_shared<ot::UserLibraryElement>(element));

		addLibraryElement(singleElementPtrList, dbUserName, dbUserPassword, dbServerUrl);
	}
	else if ((result & ot::MessageDialogCfg::No) == ot::MessageDialogCfg::No) {
		// User chose not to overwrite but to add as a new element with a unique name
		OT_LOG_I("User declined to overwrite. Adding element with unique name.");

		// Generate a unique name with suffix (e.g., "ElementName_1", "ElementName_2", etc.)
		std::string uniqueName = generateUniqueElementName(elementName, collectionName, dbUserName, dbUserPassword, dbServerUrl);
		element.setName(uniqueName);

		OT_LOG_I("Generated unique element name: " + uniqueName);

		// Add the element with the new unique name
		std::list<std::shared_ptr<ot::LibraryElement>> singleElementPtrList;
		singleElementPtrList.push_back(std::make_shared<ot::UserLibraryElement>(element));

		addLibraryElement(singleElementPtrList, dbUserName, dbUserPassword, dbServerUrl);

		OT_LOG_I("Library element '" + uniqueName + "' added successfully.");
	}

	return ot::ReturnMessage(ot::ReturnMessage::Ok).toJson();
}


// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LibraryManagementService, OT_INFO_SERVICE_TYPE_LibraryManagementService) {

}

Application::~Application() {
}

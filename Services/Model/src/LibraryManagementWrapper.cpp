// @otlicense
// File: LibraryManagementWrapper.cpp
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
#include "stdafx.h"
#include "LibraryManagementWrapper.h"
#include "Application.h"

// OpenTwin header
#include "OTModelEntities/DataBase.h"
#include "OTDataStorage/Connection/ConnectionAPI.h"
#include "OTCore/String.h"
#include "OTCore/FolderNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/EncodingGuesser.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/EntityResultTextData.h"
#include "OTModelEntities/EntityBinaryData.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTModelEntities/EntityFactory.h"
#include "OTModelEntities/Lms/LibraryEntityInterface.h"



std::list<std::string> LibraryManagementWrapper::getCircuitModels() {

	
	
	std::string lmsResonse;
	ot::JsonDocument lmsDocs;

	lmsDocs.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_GetDocumentList, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString("CircuitModels", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("ElementType", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Value, ot::JsonString("Diode", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::instance().getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::instance().getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::instance().getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	
	
	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), Application::instance()->getLMSUrl(), ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Getting Models from LMS failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to get Models");
		return {};
	}

	std::list<std::string> listOfModels;

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(lmsResonse);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return {};
	}

	ot::JsonDocument models;
	models.fromJson(rMsg.getWhat());

	if (models.IsObject()) {
		ot::ConstJsonObject obj = models.getConstObject();
		ot::ConstJsonArray docs = obj["Documents"].GetArray();

		for (const ot::JsonValue& val : docs) {
			ot::ConstJsonObject doc = val.GetObject();
			std::string content = doc["Name"].GetString();
			listOfModels.push_back(content);
		}

		return listOfModels;
	}
	else {
		OT_LOG_E("Models is not Json Object");
		return {};
	}
}

std::string LibraryManagementWrapper::getCircuitModel(std::string _modelName) {
	std::string lmsResonse;
	ot::JsonDocument lmsDocs;

	lmsDocs.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_GetDocument, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString("CircuitModels", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("Name", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(_modelName, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::instance().getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::instance().getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::instance().getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());


	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), Application::instance()->getLMSUrl(), ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Getting Models from LMS failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to get Models");
		return {};
	}

	std::list<std::string> listOfModels;

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(lmsResonse);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return {};
	}

	ot::JsonDocument models;
	models.fromJson(rMsg.getWhat());

	return rMsg.getWhat();	
}

std::string LibraryManagementWrapper::requestCreateConfig(const ot::JsonDocument& _doc) {
	std::string lmsResonse;
	
	/* In case of error:
		 Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		 Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min*/

	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), Application::instance()->getLMSUrl(), ot::EXECUTE, _doc.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Requesting config failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to request config");
		return {};
	}

	return lmsResonse;
}

void LibraryManagementWrapper::createLibraryEntity(const ot::LibraryElement& _importCfg) {
	Model* modelPtr = Application::instance()->getModel();
	if (!modelPtr) {
		OT_LOG_E("Model is null");
		return;
	}

	// Check if entity already exists
	std::string fullPath = _importCfg.getNewEntityFolder() + "/" + _importCfg.getName();
	if (checkEntityExists(fullPath, modelPtr)) {
		OT_LOG_W("Entity already exists: " + fullPath);
		return;
	}

	// Create and initialize entity
	EntityBase* entity = createAndInitializeEntity(_importCfg, modelPtr);
	if (!entity) return;

	// Add entity to model
	addEntityToModel(entity, modelPtr);
}

void LibraryManagementWrapper::updatePropertyOfEntity(const ot::LibraryElement& _importCfg, bool _dialogConfirmed) {
	Model* model = Application::instance()->getModel();
	auto entBase = model->getEntityByID(_importCfg.getRequestingEntityID());

	auto basePropertyModel = entBase->getProperties().getProperty("ModelSelection");
	auto modelProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyModel);

	EntityBase* circuitModelEntity = model->findEntityFromName(_importCfg.getNewEntityFolder() + "/" + _importCfg.getName());
	EntityBase* circuitModelFolderEntity = model->findEntityFromName(_importCfg.getNewEntityFolder());
	if (_dialogConfirmed && circuitModelEntity && circuitModelFolderEntity) {
		modelProperty->setValueName(_importCfg.getNewEntityFolder() + "/" + _importCfg.getName());
		modelProperty->setValueID(circuitModelEntity->getEntityID());
		modelProperty->setEntityContainerID(circuitModelFolderEntity->getEntityID());
	}
	else {
		modelProperty->setValueName("");
	}
	entBase->storeToDataBase();
	const std::string comment = "Property Updated";
	ot::UIDList topoList{entBase->getEntityID()};
	ot::UIDList versionList{entBase->getEntityStorageVersion()};
	model->updateTopologyEntities(topoList, versionList, comment, true);
}

EntityBase* LibraryManagementWrapper::createAndInitializeEntity(const ot::LibraryElement& _importCfg, Model* _model) {

	// Create entity from factory
	EntityFactory& factory = EntityFactory::instance();
	EntityBase* entity = factory.create(_importCfg.getEntityType());
	if (!entity) {
		OT_LOG_E("Failed to create entity of type: " + _importCfg.getEntityType());
		return nullptr;
	}

	// Initialize entity with model information
	entity->setEntityID(_model->createEntityUID());
	entity->setModelState(_model->getStateManager());
	entity->setObserver(_model);

	// Populate entity with library data
	auto* libInterface = dynamic_cast<ot::LibraryEntityInterface*>(entity);
	if (!libInterface) {
		OT_LOG_E("Entity does not implement LibraryEntityInterface: " + _importCfg.getEntityType());
		delete entity;
		return nullptr;
	}

	libInterface->setLibraryElement(_importCfg);

	std::string fullPath = _importCfg.getNewEntityFolder() + "/" + _importCfg.getName();
	entity->setName(fullPath);
	entity->storeToDataBase();

	return entity;
}

bool LibraryManagementWrapper::checkEntityExists(const std::string& _entityPath, Model* _model) {
	return _model->findEntityFromName(_entityPath) != nullptr;
}
void LibraryManagementWrapper::addEntityToModel(EntityBase* _entity, Model* _model) {
	auto* libInterface = dynamic_cast<ot::LibraryEntityInterface*>(_entity);
	if (!libInterface) return;

	// Collect topology entity
	ot::UIDList topoIDs{ _entity->getEntityID() };
	ot::UIDList topoVersions{ _entity->getEntityStorageVersion() };
	std::list<bool> forceVisible{ false };

	// Collect data entities
	ot::UIDList dataIDs, dataVersions;
	for (const auto& [dataID, dataVersion] : libInterface->getDataEntities()) {
		dataIDs.push_back(dataID);
		dataVersions.push_back(dataVersion);
	}

	// Add to model
	_model->addEntitiesToModel(topoIDs, topoVersions, forceVisible, dataIDs, dataVersions,
		topoIDs, "Added library entity", true, false, true);
}




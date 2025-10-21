
// Service header
#include "stdafx.h"
#include "LibraryManagementWrapper.h"
#include "Application.h"
#include "Model.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "DataBase.h"
#include "Connection/ConnectionAPI.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTGui/PropertyStringList.h"
#include "OTCore/ReturnMessage.h"
#include "EntityFileText.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"
#include "EntityResultTextData.h"
#include "EntityBinaryData.h"
#include "EntityBlock.h"



std::list<std::string> LibraryManagementWrapper::getCircuitModels() {

	
	
	std::string lmsResonse;
	ot::JsonDocument lmsDocs;

	lmsDocs.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_GetDocumentList, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString("CircuitModels", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("ElementType", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Value, ot::JsonString("Diode", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::GetDataBase()->getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	
	
	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
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
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::GetDataBase()->getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());


	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
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
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, _doc.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
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

void LibraryManagementWrapper::createModelTextEntity(const std::string& _modelInfo, const std::string& _folder, const std::string& _elementType, const std::string& _modelName) {
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	// Check if model alreaedy imported

	std::list<std::string> folderEntities = model->getListOfFolderItems(_folder, true);
	for (const std::string& model : folderEntities) {
		if (model == _folder + "/" + _modelName) {
			return;
		}
	}

	ot::JsonDocument circuitModelDoc;
	circuitModelDoc.fromJson(_modelInfo);

	std::string modelText = ot::json::getString(circuitModelDoc, OT_ACTION_PARAM_Content);
	std::string modelType = ot::json::getString(circuitModelDoc, OT_ACTION_PARAM_ModelType);
	


	ot::UID entIDData = model->createEntityUID();
	ot::UID entIDTopo = model->createEntityUID();

	// Create EntityFile Text
	std::unique_ptr<EntityFileText> circuitModel;
	circuitModel.reset(new EntityFileText(entIDTopo, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL));

	//Create the data entity
	EntityBinaryData fileContent(entIDData, circuitModel.get(), nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
	fileContent.setData(modelText.data(), modelText.size());
	fileContent.storeToDataBase();

	//ot::EncodingGuesser guesser;

	// set the data entity 
	circuitModel->setDataEntity(fileContent);
	circuitModel->setFileProperties("", "", "");

	circuitModel->setTextEncoding(ot::TextEncoding::UTF8);

	EntityPropertiesString* nameProp = EntityPropertiesString::createProperty("Model", "ElementType", _elementType, "Default", circuitModel->getProperties());
	nameProp->setReadOnly(true);

	EntityPropertiesString* modelTypeProp = EntityPropertiesString::createProperty("Model", "ModelType", modelType, "Default", circuitModel->getProperties());
	modelTypeProp->setReadOnly(true);

	circuitModel->getProperties().getProperty("Path", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("Filename", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("FileType", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("Text Encoding", "Text Properties")->setVisible(false);
	circuitModel->getProperties().getProperty("Syntax Highlight", "Text Properties")->setVisible(false);


	const std::string entityName = CreateNewUniqueTopologyName(folderEntities, _folder, _modelName);

	circuitModel->setName(entityName);

	circuitModel->storeToDataBase();
	m_entityIDsTopo.push_back(entIDTopo);
	m_entityVersionsTopo.push_back(circuitModel->getEntityStorageVersion());
	m_entityIDsData.push_back(entIDData);
	m_entityVersionsData.push_back(fileContent.getEntityStorageVersion());
	m_forceVisible.push_back(false);

	addModelToEntites();
}

void LibraryManagementWrapper::updatePropertyOfEntity(ot::UID _entityID, bool _dialogConfirmed, const std::string& _propertyValue) {
	Model* model = Application::instance()->getModel();
	auto entBase = model->getEntityByID(_entityID);

	auto basePropertyModel = entBase->getProperties().getProperty("ModelSelection");
	auto modelProperty = dynamic_cast<EntityPropertiesSelection*>(basePropertyModel);

	if (_dialogConfirmed) {
		modelProperty->addOption(_propertyValue);
		modelProperty->setValue(_propertyValue);
	}
	else {
		modelProperty->setValue("");
	}
	entBase->storeToDataBase();
	const std::string comment = "Property Updated";
	ot::UIDList topoList{entBase->getEntityID()};
	ot::UIDList versionList{entBase->getEntityStorageVersion()};
	model->updateTopologyEntities(topoList, versionList, comment, true);
}

void LibraryManagementWrapper::addModelToEntites() {
	Model* modelComp = Application::instance()->getModel();
	modelComp->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added file", true, false, true);
}

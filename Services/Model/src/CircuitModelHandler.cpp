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
#include "EntityFileText.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"
#include "EntityResultTextData.h"
#include "EntityBinaryData.h"

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
	//	
	//	createModelTextEntity(model);
	//	Model* modelComp = Application::instance()->getModel();
	//	modelComp->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added file", true, false);


	//	actionIsHandled = true;
	//}
		

	// Just for pushing, remove this later
	//	actionIsHandled = true; 
	// Bad idea!!
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

void CircuitModelHandler::createModelTextEntity(std::string _modelName) {
	std::string modelInfo = m_libraryMangementWrapper.getCircuitModel(_modelName);

	ot::JsonDocument circuitModelDoc;
	circuitModelDoc.fromJson(modelInfo);

	std::string modelText = ot::json::getString(circuitModelDoc, OT_ACTION_PARAM_Content);
	std::string modelType = ot::json::getString(circuitModelDoc, OT_ACTION_PARAM_ModelType);

	// Create entity Id's

	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	ot::UID entIDData = model->createEntityUID();
	ot::UID entIDTopo = model->createEntityUID();

	// Create EntityFile Text
	std::unique_ptr<EntityFileText> circuitModel;
	circuitModel.reset(new EntityFileText(entIDTopo, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL));

	//Create the data entity
	EntityBinaryData fileContent(entIDData, circuitModel.get(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
	fileContent.setData(modelText.data(),modelText.size());
	fileContent.StoreToDataBase();

	//ot::EncodingGuesser guesser;

	// set the data entity 
	circuitModel->setData(fileContent.getEntityID(), fileContent.getEntityStorageVersion());
	circuitModel->setFileProperties("", "", "");
	
	circuitModel->setTextEncoding(ot::TextEncoding::UTF8);

	EntityPropertiesString* nameProp = EntityPropertiesString::createProperty("Model", "Name", _modelName, "Default", circuitModel->getProperties());
	nameProp->setReadOnly(true);

	EntityPropertiesString* modelTypeProp = EntityPropertiesString::createProperty("Model", "ModelType", modelType, "Default", circuitModel->getProperties());
	modelTypeProp->setReadOnly(true);

	circuitModel->getProperties().getProperty("Path", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("Filename", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("FileType", "Selected File")->setVisible(false);
	circuitModel->getProperties().getProperty("Text Encoding", "Text Properties")->setVisible(false);
	circuitModel->getProperties().getProperty("Syntax Highlight", "Text Properties")->setVisible(false);

	std::list<std::string> folderEntities = model->getListOfFolderItems(ot::FolderNames::CircuitModelsFolder, true);
	const std::string entityName = CreateNewUniqueTopologyName(folderEntities, ot::FolderNames::CircuitModelsFolder, _modelName);
	circuitModel->setName(entityName);

	circuitModel->StoreToDataBase();
	m_entityIDsTopo.push_back(entIDTopo);
	m_entityVersionsTopo.push_back(circuitModel->getEntityStorageVersion());
	m_entityIDsData.push_back(entIDData);
	m_entityVersionsData.push_back(fileContent.getEntityStorageVersion());
	m_forceVisible.push_back(false);
}

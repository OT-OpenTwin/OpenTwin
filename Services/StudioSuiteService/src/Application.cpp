/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/EncodingGuesser.h"

#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccessBase.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "DocumentAPI.h"
#include "EntityResultText.h"
#include "EntityUnits.h"
#include "EntityMaterial.h"
#include "EntityProperties.h"
#include "EntityGeometry.h"
#include "EntityFile.h"
#include "EntityBinaryData.h"
#include "EntityParameter.h"
#include "EntityFileText.h"
#include "EntityBinaryData.h"

#include "InfoFileManager.h"
#include "Result1DManager.h"
#include "ParametricResult1DManager.h"

#include "boost/algorithm/string.hpp"

#include <thread>
#include <map>
#include <sstream>
#include <filesystem>

#include "zlib.h"
#include "base64.h"

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_STUDIOSUITE

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_STUDIOSUITE

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{
	m_importProjectButton = ot::ToolBarButtonCfg("Project", "Local Project", "Import", "Default/Import");
	m_importProjectButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_importProjectButton, this, &Application::handleImportProject);

	m_setCSTFileButton = ot::ToolBarButtonCfg("Project", "Local Project", "Set File", "Default/ProjectSaveAs");
	m_setCSTFileButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_setCSTFileButton, this, &Application::handleSetCSTFile);

	m_showInformationButton = ot::ToolBarButtonCfg("Project", "Versions", "Information", "Default/Information");
	m_showInformationButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_showInformationButton, this, &Application::handleShowInformation);

	m_commitChangesButton = ot::ToolBarButtonCfg("Project", "Versions", "Commit", "Default/AddSolver");
	m_commitChangesButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_commitChangesButton, this, &Application::handleCommitChanges);

	m_getChangesButton = ot::ToolBarButtonCfg("Project", "Versions", "Checkout", "Default/ArrowGreenDown");
	m_getChangesButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_getChangesButton, this, &Application::handleGetChanges);
}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED)
	{
		uploadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_DOWNLOAD_NEEDED)
	{
		downloadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_FILES_UPLOADED)
	{
		filesUploaded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_UNITS)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeUnits(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_MATERIALS)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeMaterials(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_PARAMETERS)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeParameters(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_HISTORY)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeHistory(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_SHAPEINFO)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		shapeInformation(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_RESULT1D)
	{
		bool appendData = ot::json::getBool(_doc, OT_ACTION_PARAM_APPEND);
		std::string data = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		size_t uncompressedDataLength = ot::json::getUInt64(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

		result1D(appendData, data, uncompressedDataLength);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_TRIANGLES)
	{
		std::list<std::string> names = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
		std::list<std::string> triangles = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Content);
		std::list<std::string> hash = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Hash);

		shapeTriangles(names, triangles, hash);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_GET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);

		return getLocalFileName(hostName);
	}
	else if (_action == OT_ACTION_CMD_UI_SS_SET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);
		std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);

		setLocalFileName(hostName, fileName);
	}
	else if (_action == OT_ACTION_CMD_UI_SS_GET_SIMPLE_FILENAME)
	{
		return getSimpleFileName();
	}

	return OT_ACTION_RETURN_UnknownAction;
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Project");
	
	_ui->addMenuGroup("Project", "Local Project");
	_ui->addMenuGroup("Project", "Versions");

	_ui->addMenuButton(m_importProjectButton);
	_ui->addMenuButton(m_setCSTFileButton);
	_ui->addMenuButton(m_showInformationButton);
	_ui->addMenuButton(m_commitChangesButton);
	_ui->addMenuButton(m_getChangesButton);

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::handleImportProject(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has already been initialized.");
		return;
	}
	 
	// Send the import message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_IMPORT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::handleSetCSTFile(void)
{
	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import a Studio Suite project file first.");
		return;
	}

	// Send the set file message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_SETCSTFILE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::handleShowInformation(void) {
	// Check whether the project has already been initialized
	if (!isProjectInitialized()) {
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import a Studio Suite project file first.");
		return;
	}

	std::string currentVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the information message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_INFORMATION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(currentVersion, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::handleCommitChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import a Studio Suite project file first.");
		return;
	}

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_COMMIT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::handleGetChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import a Studio Suite project file first.");
		return;
	}

	// Get the current model version
	std::string version = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::uploadNeeded(ot::JsonDocument& _doc)
{
	size_t count = ot::json::getInt64(_doc, OT_ACTION_PARAM_COUNT);

	std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);
	std::string hostName= ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);

	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	readProjectInformation(simpleFileName, hostNamesAndFileNames);

	addHostNameAndFileName(hostName, fileName, hostNamesAndFileNames);

	std::filesystem::path path(fileName);
	if (!simpleFileName.empty())
	{
		assert(simpleFileName == path.filename().string());
	}

	simpleFileName = path.filename().string();

	writeProjectInformation(simpleFileName, hostNamesAndFileNames);

	ot::UIDList entityID, versionID;

	for (size_t i = 0; i < count; i++)
	{
		entityID.push_back(this->getModelComponent()->createEntityUID());
		versionID.push_back(this->getModelComponent()->createEntityUID());
	}

	// Read the information
	infoFileManager.setData(this);
	infoFileManager.readInformation();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, infoFileManager.getInfoEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, infoFileManager.getInfoEntityVersion(), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::downloadNeeded(ot::JsonDocument& _doc)
{
	// Determine all files in the Files folder
	std::list<std::string> fileNames = ot::ModelServiceAPI::getListOfFolderItems("Files", true);

	std::list<ot::EntityInformation> fileInfo;
	ot::ModelServiceAPI::getEntityInformation(fileNames, fileInfo);

	ot::UIDList entityID, versionID;

	for (auto file : fileInfo)
	{
		if (file.getEntityType() == "EntityFile" && file.getEntityName() != "Files/Information")
		{
			entityID.push_back(file.getEntityID());
			versionID.push_back(file.getEntityVersion());
		}
	}

	// Get the current model version
	std::string version = ot::ModelServiceAPI::getCurrentModelVersion();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_DOWNLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::filesUploaded(ot::JsonDocument& _doc)
{
	std::string changeMessage = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);

	std::list<std::string> modifiedNameList = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
	std::list<ot::UID> fileEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityID);
	std::list<ot::UID> fileVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersion);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataID);
	std::list<ot::UID> dataVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataVersion);

	std::list<std::string> deletedNameList = ot::json::getStringList(_doc, OT_ACTION_CMD_MODEL_DeleteEntity);

	// We need to finalize the shape triangle information
	infoFileManager.writeInformation();
	infoFileManager.addDeletedShapesToList(deletedNameList);

	// Now we need to send the model change to the model service 
	for (auto item : modifiedNameList)
	{
		ot::UID fileEntityID = fileEntityIDList.front(); fileEntityIDList.pop_front();
		ot::UID fileVersion  = fileVersionList.front();  fileVersionList.pop_front();
		ot::UID dataEntityID = dataEntityIDList.front(); dataEntityIDList.pop_front();
		ot::UID dataVersion  = dataVersionList.front();  dataVersionList.pop_front();

		getModelComponent()->addNewTopologyEntity(fileEntityID, fileVersion, false);
		getModelComponent()->addNewDataEntity(dataEntityID, dataVersion, fileEntityID);
	}

	ot::ModelServiceAPI::deleteEntitiesFromModel(deletedNameList, false);

	getModelComponent()->storeNewEntities("Studio Suite project uploaded", false);

	// Determine the new version
	std::string newVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Set the label for the new version
	ot::JsonDocument doc1;
	doc1.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetVersionLabel, doc1.GetAllocator()), doc1.GetAllocator());
	doc1.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc1.GetAllocator()), doc1.GetAllocator());
	doc1.AddMember(OT_ACTION_PARAM_MODEL_VersionLabel, ot::JsonString(changeMessage, doc1.GetAllocator()), doc1.GetAllocator());

	std::string tmp;
	getModelComponent()->sendMessage(true, doc1, tmp);

	// Finally we send the new version to the frontend 
	ot::JsonDocument doc2;
	doc2.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_COPY, doc2.GetAllocator()), doc2.GetAllocator());
	doc2.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc2.GetAllocator()), doc2.GetAllocator());

	getUiComponent()->sendMessage(true, doc2, tmp);
}

void Application::changeUnits(const std::string &content)
{
	// Load the current units entity
	ot::EntityInformation entityInformation;
	ot::ModelServiceAPI::getEntityInformation("Units", entityInformation);
	EntityUnits* units = dynamic_cast<EntityUnits*> (ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion()));
	assert(units != nullptr);
	if (units == nullptr) return;

	// Loop through the various units and check whether an update is necessary
	std::stringstream buffer(content);
	bool changed = false;

	processSingleUnit("Dimension", buffer, units, changed);
	processSingleUnit("Temperature", buffer, units, changed);
	processSingleUnit("Voltage", buffer, units, changed);
	processSingleUnit("Current", buffer, units, changed);
	processSingleUnit("Resistance", buffer, units, changed);
	processSingleUnit("Conductance", buffer, units, changed);
	processSingleUnit("Capacitance", buffer, units, changed);
	processSingleUnit("Inductance", buffer, units, changed);
	processSingleUnit("Frequency", buffer, units, changed);
	processSingleUnit("Time", buffer, units, changed);

	// If a change is necessary, store the new entity
	if (changed)
	{
		units->getProperties().setAllPropertiesReadOnly();
		units->StoreToDataBase();
		getModelComponent()->addNewTopologyEntity(units->getEntityID(), units->getEntityStorageVersion(), false);
	}

	delete units; units = nullptr;
}

void Application::processSingleUnit(const std::string& unitName, std::stringstream& buffer, EntityUnits* units, bool& changed)
{
	std::string currentValue;
	std::getline(buffer, currentValue);

	EntityPropertiesSelection* scale = dynamic_cast<EntityPropertiesSelection*>(units->getProperties().getProperty(unitName));
	assert(scale != nullptr);
	if (scale == nullptr) return;

	if (scale->getValue() != currentValue)
	{
		changed = true;
		scale->setValue(currentValue);
	}
}

void Application::changeMaterials(const std::string &content)
{
	std::stringstream buffer(content);

	ot::UIDList currentMaterials = ot::ModelServiceAPI::getIDsOfFolderItemsOfType("Materials", "EntityMaterial", true);
	std::list<ot::EntityInformation> currentMaterialInfo;
	ot::ModelServiceAPI::getEntityInformation(currentMaterials, currentMaterialInfo);
	std::map<std::string, bool> materialProcessed;

	while (processSingleMaterial(buffer, materialProcessed));

	// Now we need to check if an existing material was not processed, which means that it no longer exists.
	// In this case, we need to delete the material

	std::list<std::string> obsoleteMaterials;

	for (auto material : currentMaterialInfo)
	{
		if (materialProcessed.count(material.getEntityName()) == 0)
		{
			obsoleteMaterials.push_back(material.getEntityName());
		}
	}

	if (!obsoleteMaterials.empty())
	{
		ot::ModelServiceAPI::deleteEntitiesFromModel(obsoleteMaterials, false);
	}
}

bool Application::processSingleMaterial(std::stringstream& buffer, std::map<std::string, bool> &materialProcessed)
{
	std::string materialName, materialColor, materialType, materialEps, materialMu, materialSigma;
	double materialEpsValue(1.0), materialMuValue(1.0), materialSigmaValue(0.0);

	std::getline(buffer, materialName);
	if (materialName.empty()) return false;

	std::getline(buffer, materialColor);
	double r(0.0), g(0.0), b(0.0);
	readDoubleTriple(materialColor, r, g, b);

	materialColors[materialName] = std::tuple<double, double, double>(r, g, b);

	std::getline(buffer, materialType);

	if (materialType == "Normal")
	{
		std::getline(buffer, materialEps);
		double epsx(0.0), epsy(0.0), epsz(0.0);
		readDoubleTriple(materialEps, epsx, epsy, epsz);
		materialEpsValue = sqrt((epsx * epsx + epsy * epsy + epsz * epsz)/3.0);

		std::getline(buffer, materialMu);
		double mux(0.0), muy(0.0), muz(0.0);
		readDoubleTriple(materialMu, mux, muy, muz);
		materialMuValue = sqrt((mux * mux + muy * muy + muz * muz) / 3.0);

		std::getline(buffer, materialSigma);
		double sigmax(0.0), sigmay(0.0), sigmaz(0.0);
		readDoubleTriple(materialSigma, sigmax, sigmay, sigmaz);
		materialSigmaValue = sqrt((sigmax * sigmax + sigmay * sigmay + sigmaz * sigmaz) / 3.0);
	}

	if (materialName[0] == '$')
	{
		// This is an internal material which we should not consider
		return true;
	}

	// Try loading the material
	EntityMaterial* material = nullptr;

	ot::EntityInformation entityInformation;
	if (ot::ModelServiceAPI::getEntityInformation("Materials/" + materialName, entityInformation))
	{
		material = dynamic_cast<EntityMaterial*> (ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion()));
	}

	bool changed = false;

	if (material == nullptr)
	{
		// We have a new material to create
		material = new EntityMaterial(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, getServiceName());
		material->setName("Materials/" + materialName);
		material->createProperties();

		EntityPropertiesSelection* type = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));
		EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Conductivity"));

		type->setValue(materialType == "Normal" ? "Volumetric" : "PEC");
		type->setNeedsUpdate();

		permittivity->setValue(materialEpsValue);
		permeability->setValue(materialMuValue);
		conductivity->setValue(materialSigmaValue);

		changed = true;
	}
	else
	{
		// We need to modify an existing material
		EntityPropertiesSelection* type = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));
		EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Conductivity"));

		type->setValue(materialType == "Normal" ? "Volumetric" : "PEC");

		permittivity->setValue(materialEpsValue);
		permeability->setValue(materialMuValue);
		conductivity->setValue(materialSigmaValue);

		if (material->getProperties().anyPropertyNeedsUpdate())
		{
			changed = true;
		}

		materialProcessed[material->getName()] = true;
	}

	// If a change is necessary, store the new entity
	if (changed)
	{
		material->updateFromProperties();
		material->getProperties().setAllPropertiesReadOnly();
		material->StoreToDataBase();
		getModelComponent()->addNewTopologyEntity(material->getEntityID(), material->getEntityStorageVersion(), false);
	}

	delete material; material = nullptr;

	return true;
}

void Application::changeParameters(const std::string& content)
{
	std::stringstream buffer(content);

	ot::UIDList currentParameters = ot::ModelServiceAPI::getIDsOfFolderItemsOfType("Parameters", "EntityParameter", true);
	std::list<ot::EntityInformation> currentParameterInfo;
	ot::ModelServiceAPI::getEntityInformation(currentParameters, currentParameterInfo);
	std::map<std::string, bool> parameterProcessed;

	processParameterBuffer(buffer, parameterProcessed);

	// Now we need to check if an existing parameter was not processed, which means that it no longer exists.
	// In this case, we need to delete the parameter

	std::list<std::string> obsoleteParameters;

	for (auto parameter : currentParameterInfo)
	{
		if (parameterProcessed.count(parameter.getEntityName()) == 0)
		{
			obsoleteParameters.push_back(parameter.getEntityName());
		}
	}

	if (!obsoleteParameters.empty())
	{
		ot::ModelServiceAPI::deleteEntitiesFromModel(obsoleteParameters, false);
	}
}

void Application::changeHistory(const std::string& content)
{
	EntityFileText* history = nullptr;

	{
		std::shared_ptr<EntityBinaryData> data = nullptr;

		ot::EntityInformation currentHistoryInfo;
		if (ot::ModelServiceAPI::getEntityInformation("History", currentHistoryInfo))
		{
			// A history already exists
			history = dynamic_cast<EntityFileText*> (ot::EntityAPI::readEntityFromEntityIDandVersion(currentHistoryInfo.getEntityID(), currentHistoryInfo.getEntityVersion()));

			data = history->getData();
		}
		else
		{
			// A new history item needs to be created
			history = new EntityFileText(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, getServiceName());
			history->setFileProperties("", "history", "txt");
			history->setName("History");

			data = std::make_shared<EntityBinaryData>(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
		}

		data->setData(content.data(), content.size());
		data->StoreToDataBase();

		ot::EncodingGuesser guesser;

		history->setData(data->getEntityID(), data->getEntityStorageVersion());
		history->setTextEncoding(guesser(content.data(), content.size()));
		history->getProperties().setAllPropertiesReadOnly();
		history->setEditable(false);
		history->StoreToDataBase();

		getModelComponent()->addNewDataEntity(data->getEntityID(), data->getEntityStorageVersion(), history->getEntityID());
		getModelComponent()->addNewTopologyEntity(history->getEntityID(), history->getEntityStorageVersion(), false);
	}

	delete history; history = nullptr;
}

void Application::processParameterBuffer(std::stringstream& buffer, std::map<std::string, bool>& parameterProcessed)
{
	std::string line;
	std::getline(buffer, line);

	int numberOfParameters = atoi(line.c_str());

	bool changed = false;

	for (int index = 0; index < numberOfParameters; index++)
	{
		std::string parameterName;
		std::getline(buffer, parameterName);

		std::string parameterValue;
		std::getline(buffer, parameterValue);

		std::string parameterDescription;
		std::getline(buffer, parameterDescription);

		// Try loading the parameter
		EntityParameter* parameter = nullptr;

		ot::EntityInformation entityInformation;
		if (ot::ModelServiceAPI::getEntityInformation("Parameters/" + parameterName, entityInformation))
		{
			parameter = dynamic_cast<EntityParameter*> (ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion()));
		}

		if (parameter == nullptr)
		{
			// We have a new parameter to create
			parameter = new EntityParameter(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, getServiceName());
			parameter->setName("Parameters/" + parameterName);
			parameter->createProperties();

			EntityPropertiesString* stringValue = dynamic_cast<EntityPropertiesString*>(parameter->getProperties().getProperty("Value"));
			EntityPropertiesDouble* doubleValue = dynamic_cast<EntityPropertiesDouble*>(parameter->getProperties().getProperty("#Value"));
			EntityPropertiesString* description = dynamic_cast<EntityPropertiesString*>(parameter->getProperties().getProperty("Description"));

			stringValue->setValue(parameterValue);
			doubleValue->setValue(atof(parameterValue.c_str()));
			description->setValue(parameterDescription);

			changed = true;
		}
		else
		{
			// We need to modify an existing parameter
			EntityPropertiesString* stringValue = dynamic_cast<EntityPropertiesString*>(parameter->getProperties().getProperty("Value"));
			EntityPropertiesDouble* doubleValue = dynamic_cast<EntityPropertiesDouble*>(parameter->getProperties().getProperty("#Value"));
			EntityPropertiesString* description = dynamic_cast<EntityPropertiesString*>(parameter->getProperties().getProperty("Description"));

			if (stringValue->getValue() != parameterValue)
			{
				stringValue->setValue(parameterValue);
				doubleValue->setValue(atof(parameterValue.c_str()));

				changed = true;
			}

			if (description->getValue() != parameterDescription)
			{
				description->setValue(parameterDescription);

				changed = true;
			}

			parameterProcessed[parameter->getName()] = true;
		}

		// If a change is necessary, store the new entity
		if (changed)
		{
			parameter->getProperties().setAllPropertiesReadOnly();
			parameter->StoreToDataBase();
			getModelComponent()->addNewTopologyEntity(parameter->getEntityID(), parameter->getEntityStorageVersion(), false);
		}

		delete parameter; parameter = nullptr;
	}
}

void Application::readDoubleTriple(const std::string& line, double& a, double& b, double& c)
{
	std::stringstream buffer(line);

	std::string sA, sB, sC;
	buffer >> sA;
	buffer >> sB;
	buffer >> sC;

	std::replace(sA.begin(), sA.end(), ',', '.');
	std::replace(sB.begin(), sB.end(), ',', '.');
	std::replace(sC.begin(), sC.end(), ',', '.');

	a = std::atof(sA.c_str());
	b = std::atof(sB.c_str());
	c = std::atof(sC.c_str());
}

void Application::shapeInformation(const std::string &content)
{
	std::map<std::string, bool> previousShape;
	infoFileManager.getShapes(previousShape);

	std::stringstream data(content);

	while (!data.eof())
	{
		std::string name, material;

		std::getline(data, name);
		std::getline(data, material);

		if (!name.empty() && !material.empty())
		{
			shapeMaterials[name] = material;
			previousShape[name] = true;
		}
	}

	for (auto shape : previousShape)
	{
		if (!shape.second)
		{
			// This shape existed before, but was not processed this time anyore -> the shape has been deleted
			infoFileManager.deleteShape(shape.first);
		}
	}
}

void Application::result1D(bool appendData, std::string& data, size_t uncompressedDataLength)
{
	ParametricResult1DManager parametricResultManager(this);

	if (!appendData)
	{
		// We need to clear the result1D information in the info file manager in case that we do not want to append
		infoFileManager.clearResult1D();
		parametricResultManager.clear();
	}

	if (data.empty())
	{
		return;
	}

	// Now we read the data from GridFS
	DataStorageAPI::DocumentAPI doc;
	uint8_t* dataBuffer = nullptr;
	size_t length = 0;

	bsoncxx::oid oid_obj{ data };
	bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

	doc.GetDocumentUsingGridFs(id, dataBuffer, length, DataBase::GetDataBase()->getProjectName());
	assert(length == uncompressedDataLength);

	doc.DeleteGridFSData(id, DataBase::GetDataBase()->getProjectName());

	try
	{
		{
			// Now the data is in the dataBuffer with length uncompressedDataLength
			// We need to process the buffer and read the data from it
			Result1DManager resultManager((char *) dataBuffer, uncompressedDataLength);

			// We can now delete the buffer
			delete[] dataBuffer;
			dataBuffer = nullptr;

			// Now we need to store the new hash information in the infoFileManager
			resultManager.addResult1DInformation(infoFileManager);

			// And finally store (add) the parametric data. In case that the data is not appended, the storage was already cleared above
			parametricResultManager.extractData(resultManager);
		}
		parametricResultManager.storeDataInResultCollection();
	}
	catch (std::exception &e)
	{
		std::string error = e.what();
		assert(0);

		// We have an problem processing the data.
		if (dataBuffer != nullptr)
		{
			delete[] dataBuffer;
			dataBuffer = nullptr;
		}
	}
}


void Application::shapeTriangles(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles, std::list<std::string>& shapeHash)
{
	std::string materialsFolder = "Materials";

	std::list<std::string> entityList{ materialsFolder };
	std::list<ot::EntityInformation> entityInfo;

	ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);

	assert(entityInfo.size() == 1);
	assert(entityInfo.front().getEntityName() == materialsFolder);

	ot::UID materialsFolderID = entityInfo.front().getEntityID();

	auto triangles = shapeTriangles.begin();
	auto hash = shapeHash.begin();

	for (auto name : shapeNames)
	{
		storeShape(name, *triangles, materialsFolder, materialsFolderID);
		infoFileManager.setShapeHash(name, *hash);

		triangles++;
		hash++;
	}
}

void Application::storeShape(const std::string& name, const std::string& triangles, 
							 const std::string &materialsFolder, ot::UID materialsFolderID)
{
	std::string otName = "Geometry\\" + name;
	std::replace(otName.begin(), otName.end(), '\\', '/');

	std::string material = shapeMaterials[name];
	std::tuple<double, double, double> rgb = materialColors[material];

	int colorR = (int) (255 * std::get<0>(rgb));
	int colorG = (int) (255 * std::get<1>(rgb));
	int colorB = (int) (255 * std::get<2>(rgb));

	ot::UID entityID = getModelComponent()->createEntityUID();
	ot::UID facetsID = getModelComponent()->createEntityUID();

	EntityGeometry* entityGeom = new EntityGeometry(entityID, nullptr, nullptr, nullptr, getServiceName());
	entityGeom->setName(otName);
	entityGeom->setEditable(false);

	entityGeom->createProperties(colorR, colorG, colorB, materialsFolder, materialsFolderID);

	EntityPropertiesEntityList* materialProp = dynamic_cast<EntityPropertiesEntityList*> (entityGeom->getProperties().getProperty("Material"));
	assert(materialProp != nullptr);
	if (materialProp != nullptr)
	{
		materialProp->setValueName(material);
	}

	entityGeom->getFacets()->setEntityID(facetsID);

	createFacets(triangles, entityGeom->getFacets()->getNodeVector(), entityGeom->getFacets()->getTriangleList(), entityGeom->getFacets()->getEdgeList());

	entityGeom->getProperties().setAllPropertiesReadOnly();
	entityGeom->getFacets()->StoreToDataBase();
	entityGeom->StoreToDataBase();

	getModelComponent()->addNewDataEntity(entityGeom->getFacets()->getEntityID(), entityGeom->getFacets()->getEntityStorageVersion(), entityGeom->getEntityID());
	getModelComponent()->addNewDataEntity(entityGeom->getBrepEntity()->getEntityID(), entityGeom->getBrepEntity()->getEntityStorageVersion(), entityGeom->getEntityID());
	getModelComponent()->addNewTopologyEntity(entityGeom->getEntityID(), entityGeom->getEntityStorageVersion(), false);

	delete entityGeom;
	entityGeom = nullptr;
}

void Application::createFacets(const std::string& data, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges)
{
	// Process the STL data
	std::stringstream stlData(data);

	ot::UID vertex[3];
	Geometry::Node node[3];
	int vertexID = 0;
	size_t vertexCount = 0;

	std::list<Geometry::Node> nodesList;

	while (!stlData.eof())
	{
		std::string line;
		std::getline(stlData, line);

		boost::trim_left(line);
		boost::to_lower(line);

		std::stringstream lineBuffer(line);

		std::string keyword;
		lineBuffer >> keyword;

		if (keyword == "vertex")
		{
			double pX(0.0), pY(0.0), pZ(0.0);

			lineBuffer >> pX >> pY >> pZ;

			node[vertexID].setCoords(pX, pY, pZ);

			vertex[vertexID] = vertexCount;
			vertexCount++;
			vertexID++;

			if (vertexID == 3)
			{
				// We need to calculate the facet normal, since this information can be incorrect in Studio Suite generated STL files

				double vector1[] = { node[1].getCoord(0) - node[0].getCoord(0), node[1].getCoord(1) - node[0].getCoord(1), node[1].getCoord(2) - node[0].getCoord(2) };
				double vector2[] = { node[2].getCoord(0) - node[0].getCoord(0), node[2].getCoord(1) - node[0].getCoord(1), node[2].getCoord(2) - node[0].getCoord(2) };

				double normalX = vector1[1] * vector2[2] - vector1[2] * vector2[1];
				double normalY = vector1[2] * vector2[0] - vector1[0] * vector2[2];
				double normalZ = vector1[0] * vector2[1] - vector1[1] * vector2[0];

				double abs = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);

				normalX /= abs;
				normalY /= abs;
				normalZ /= abs;

				node[0].setNormals(normalX, normalY, normalZ);
				node[1].setNormals(normalX, normalY, normalZ);
				node[2].setNormals(normalX, normalY, normalZ);

				nodesList.push_back(node[0]);
				nodesList.push_back(node[1]);
				nodesList.push_back(node[2]);

				// Three vertices were added, so we have a new facet
				triangles.push_back(Geometry::Triangle(vertex[0], vertex[1], vertex[2], 0));
				vertexID = 0;
			}
		}
	}

	// Finally we need to store the list of nodes in the nodes array
	nodes.reserve(nodesList.size());
	for (auto n : nodesList)
	{
		nodes.push_back(n);
	}
}

void Application::writeProjectInformation(const std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	assert(!simpleFileName.empty());

	std::stringstream data;
	data << simpleFileName << std::endl;
	for (auto item : hostNamesAndFileNames)
	{
		data << item.first << std::endl;
		data << item.second << std::endl;
	}

	std::string stringData = data.str();

	mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

	long long modelVersion = getCurrentModelEntityVersion();

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< "Version" << modelVersion
		<< bsoncxx::builder::stream::finalize;

	auto modifyDoc = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "ProjectInformation" << stringData
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	collection.update_one(queryDoc.view(), modifyDoc.view());
}

long long Application::getCurrentModelEntityVersion(void)
{
	// We search for the last model entity in the database and determine its version
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return 0;  // No model entity found

	return result->view()["Version"].get_int64();
}

bool Application::readProjectInformation(std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	// Get the project information string from the model entity
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return false;  // No model entity found (this project is not yet initialized)

	std::string projectInformation;

	try
	{
		projectInformation = result->view()["ProjectInformation"].get_utf8().value.data();
	}
	catch (std::exception)
	{
		return false; // This project does not have project information yet (not initialized)
	}

	std::stringstream content(projectInformation);

	std::getline(content, simpleFileName);
	while (!content.eof())
	{
		std::string hostName, fileName;
		std::getline(content, hostName);
		std::getline(content, fileName);

		if (!hostName.empty() && !fileName.empty())
		{
			hostNamesAndFileNames.push_back(std::pair<std::string, std::string>(hostName, fileName));
		}
	}

	return true;
}

bool Application::isProjectInitialized()
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	return readProjectInformation(simpleFileName, hostNamesAndFileNames);
}

std::string Application::getLocalFileName(const std::string &hostName)
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (!readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		return ""; // This project has not yet been initialized
	}

	// Try to find the Hostname in the list
	for (auto item : hostNamesAndFileNames)
	{
		if (item.first == hostName)
		{
			// We have found this hostName
			return item.second;
		}
	}

	return ""; // We could not find the host name in the list
}

std::string Application::getSimpleFileName()
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (!readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		return ""; // This project has not yet been initialized
	}

	return simpleFileName;
}

void Application::addHostNameAndFileName(const std::string& hostName, const std::string& fileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	// First, we delete an previously existing file for the given host
	for (auto item : hostNamesAndFileNames)
	{
		if (item.first == hostName)
		{
			// We have found this hostName
			hostNamesAndFileNames.remove(item);
			break;
		}
	}

	// Now we add the new file for the given host name
	hostNamesAndFileNames.push_back(std::pair<std::string, std::string>(hostName, fileName));
}

void Application::setLocalFileName(const std::string& hostName, const std::string& fileName)
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		addHostNameAndFileName(hostName, fileName, hostNamesAndFileNames);

		std::filesystem::path path(fileName);
		if (!simpleFileName.empty())
		{
			assert(simpleFileName == path.filename().string());
		}

		writeProjectInformation(simpleFileName, hostNamesAndFileNames);
	}
	else
	{
		assert(0);
	}
}

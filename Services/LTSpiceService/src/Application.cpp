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

// OpenTwin header
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/ActionDispatcher.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccessBase.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"
#include "EntityUnits.h"
#include "EntityMaterial.h"
#include "EntityProperties.h"
#include "EntityGeometry.h"
#include "EntityFile.h"
#include "EntityBinaryData.h"

#include "InfoFileManager.h"
#include "Result1DManager.h"
#include "ParametricResult1DManager.h"
#include "ParametricCombination.h"

#include "boost/algorithm/string.hpp"

#include <thread>
#include <map>
#include <sstream>
#include <filesystem>
#include <regex>

#include "zlib.h"
#include "base64.h"

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_LTSPICE

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_LTSPICE

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED)
	{
		uploadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_DOWNLOAD_NEEDED)
	{
		downloadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_FILES_UPLOADED)
	{
		filesUploaded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_GET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);

		return getLocalFileName(hostName);
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_SET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);
		std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);

		setLocalFileName(hostName, fileName);
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_GET_SIMPLE_FILENAME)
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

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("Project", "Local Project", "Import", "Import", modelWrite, "Import", "Default");
	_ui->addMenuButton("Project", "Local Project", "Set File", "Set File", modelWrite, "ProjectSaveAs", "Default");
	_ui->addMenuButton("Project", "Versions", "Information", "Information", modelWrite, "Information", "Default");
	_ui->addMenuButton("Project", "Versions", "Commit", "Commit", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("Project", "Versions", "Checkout", "Checkout", modelWrite, "ArrowGreenDown", "Default");

	connectToolBarButton("Project:Local Project:Import", this, &Application::importProject);
	connectToolBarButton("Project:Local Project:Set File", this, &Application::setLTSpiceFile);
	connectToolBarButton("Project:Versions:Information", this, &Application::showInformation);
	connectToolBarButton("Project:Versions:Commit", this, &Application::commitChanges);
	connectToolBarButton("Project:Versions:Checkout", this, &Application::getChanges);

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged()
{
	//if (isUiConnected()) {
	//	std::list<std::string> enabled;
	//	std::list<std::string> disabled;

	//	if (selectedEntities.size() > 0)
	//	{
	//		enabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}
	//	else
	//	{
	//		disabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}

	//	m_uiComponent->setControlsEnabledState(enabled, disabled);
	//}
}

void Application::importProject(void)
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
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_IMPORT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::setLTSpiceFile(void)
{
	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Send the set file message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_SETLTSPICEFILE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::commitChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_COMMIT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::showInformation(void)
{
	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	std::string currentVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the information message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_INFORMATION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(currentVersion, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::getChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Get the current model version
	std::string version = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_GET, doc.GetAllocator()), doc.GetAllocator());
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
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD, doc.GetAllocator()), doc.GetAllocator());
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
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_DOWNLOAD, doc.GetAllocator()), doc.GetAllocator());
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

	getModelComponent()->storeNewEntities("", false, false);

	extractResults();

	getModelComponent()->storeNewEntities("LTSpice project uploaded", false);

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
	doc2.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_COPY, doc2.GetAllocator()), doc2.GetAllocator());
	doc2.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc2.GetAllocator()), doc2.GetAllocator());

	getUiComponent()->sendMessage(true, doc2, tmp);
}

void Application::changeUnits(const std::string &content)
{
	// Load the current units entity
	ot::EntityInformation entityInformation;
	ot::ModelServiceAPI::getEntityInformation("Units", entityInformation);
	EntityUnits* units = dynamic_cast<EntityUnits*> (ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion(), getClassFactory()));
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
		material = dynamic_cast<EntityMaterial*> (ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion(), getClassFactory()));
	}

	bool changed = false;

	if (material == nullptr)
	{
		// We have a new material to create
		material = new EntityMaterial(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, getServiceName());
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

	// First, we need to decode the data back into a byte buffer
	int decoded_compressed_data_length = Base64decode_len(data.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, data.c_str());

	data.clear();

	// Decompress the data
	char* dataBuffer = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)dataBuffer, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	try
	{
		{
			// Now the data is in the dataBuffer with length uncompressedDataLength
			// We need to process the buffer and read the data from it
			Result1DManager resultManager(dataBuffer, uncompressedDataLength);

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
	auto hash      = shapeHash.begin();

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

	EntityGeometry* entityGeom = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
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

void Application::extractResults()
{
	std::string projectFileName = "Files/" + std::filesystem::path(getSimpleFileName()).stem().string();
	if (projectFileName.empty()) return;

	// First, we get a list of all parametric runs and their parameter combinations from the .log file
	std::list<ParametricCombination> parameterRuns; 
	getParametricCombinations(projectFileName + ".log", parameterRuns);

	// Now we read the curves from the .raw file
	LTSpiceData spiceData = getCurveData(projectFileName + ".raw");

	// Split the curves into the individual parametric runs (if necessary)


	// Finally, we store the parametric data of the curves


}

void Application::getParametricCombinations(const std::string& logFileName, std::list<ParametricCombination>& parameterRuns)
{
	parameterRuns.clear();

	// Read the data of the log file entity into a string logFileContent
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(logFileName, entityInfo);
	
	EntityFile *fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), getClassFactory()));
	if (fileEntity == nullptr) return;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getData();
	if (data == nullptr) return;

	std::vector<char> logFileData = data->getData();
	std::string logFileContent(logFileData.begin(), logFileData.end());

	// Now extract all lines with a step message from the logfile data
	std::vector<std::string> stepLines;
	std::istringstream stream(logFileContent);
	std::string line;

	while (std::getline(stream, line)) {
		std::string lowerStr = line;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		if (lowerStr.find(".step") != std::string::npos) {
			stepLines.push_back(line);
		}
	}

	// Finally, we need to process each line and extract its parameter combinations
	std::vector<std::pair<std::string, double>> params;
	std::regex stepRegex(R"((\w+)=([-+]?\d*\.?\d+))"); // Regex für Parameter=Wert
	std::smatch match;

	for (auto line : stepLines)
	{
		ParametricCombination parameterCombination;

		auto searchStart = line.cbegin();
		while (std::regex_search(searchStart, line.cend(), match, stepRegex)) {
			std::string param = match[1].str();           // parameter name
			double value = std::stod(match[2].str());     // parameter value
			parameterCombination.addParameter(param, value);
			searchStart = match.suffix().first;
		}

		parameterRuns.push_back(parameterCombination);
	}
}

LTSpiceData Application::getCurveData(const std::string& rawFileName)
{
	LTSpiceData result;

	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(rawFileName, entityInfo);

	EntityFile* fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), getClassFactory()));
	if (fileEntity == nullptr) return result;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getData();
	if (data == nullptr) return result;

	std::vector<char> rawFileData = data->getData();

	std::stringstream dataStream(std::ios::in | std::ios::out | std::ios::binary);
	dataStream.write(rawFileData.data(), rawFileData.size());

	result = readLTSpiceRaw(dataStream);

	return result;
}

bool Application::getLine(std::stringstream& data, std::string &line)
{
	line.clear();
	while (!data.eof())
	{
		char c = 0;
		data.get(c);

		if (c == '\n')
		{
			data.get(c); // Skip the following 0
			return true;
		}
		if (c != 0 && c != '\r') line.push_back(c);
	}

	return !(line.empty());
}

double bytesToDouble(const uint8_t* bytes) {
	// 8-Byte-Wert für die Interpretation
	uint64_t intRepresentation = 0;

	// Big-Endian zu uint64_t umwandeln
	for (int i = 0; i < 8; ++i) {
		intRepresentation = (intRepresentation << 8) | bytes[i];
	}

	// In double umwandeln
	double result;
	std::memcpy(&result, &intRepresentation, sizeof(double)); // Typ-Punning
	return result;
}

LTSpiceData Application::readLTSpiceRaw(std::stringstream& data) 
{
	LTSpiceData result;

	bool binaryData = false;

	std::string line;
	while (getLine(data, line)) {
		if (line.find("Flags:") != std::string::npos) {
			if (line.find("complex") != std::string::npos) {
				result.isComplex = true;
			}
		}
		else if (line.find("No. Variables:") != std::string::npos) {
			int numVariables = std::stoi(line.substr(15));
			result.variables.reserve(numVariables);
		}
		else if (line.find("Variables:") != std::string::npos) {
			for (int i = 0; i < result.variables.capacity(); i++) {
				std::string varline;
				if (getLine(data, varline))
				{
					std::stringstream varStream(varline);
					LTSpiceVariable var;
					varStream >> var.index >> var.name >> var.unit;
					result.variables.push_back(var);
				}
			}
		}
		else if (line.find("Binary:") != std::string::npos) {
			binaryData = true;
			break;  // Start of the binary data block
		}
		else if (line.find("Values:") != std::string::npos) {
			break;  // Start of the ascii data block
		}
	}

	if (binaryData)
	{
		// Read the binary data (distinguish between real and complex data)
		while (!data.eof()) {
			std::vector<std::complex<double>> dataRow;
			for (size_t i = 0; i < result.variables.size(); i++) {
				if (i == 0)
				{
					// This variable is stored in double-precision
					if (result.isComplex) {
						double realPart, imagPart;
						data.read(reinterpret_cast<char*>(&realPart), sizeof(double));
						data.read(reinterpret_cast<char*>(&imagPart), sizeof(double));
						dataRow.emplace_back(realPart, imagPart);
					}
					else {
						double realValue;
						data.read(reinterpret_cast<char*>(&realValue), sizeof(double));
						dataRow.emplace_back(realValue, 0.0f);
					}
				}
				else
				{
					// This variable is stored in single-precision
					if (result.isComplex) {
						double realPart, imagPart;
						data.read(reinterpret_cast<char*>(&realPart), sizeof(double));
						data.read(reinterpret_cast<char*>(&imagPart), sizeof(double));
						dataRow.emplace_back(realPart, imagPart);
					}
					else {
						float realValue;
						data.read(reinterpret_cast<char*>(&realValue), sizeof(float));
						dataRow.emplace_back(realValue, 0.0f);
					}
				}
			}
			if (data) result.data.push_back(dataRow);
		}
	}
	else
	{
		// Read the ascii data (distinguish between real and complex data)
		assert(0); // Not yet implemented


	}

	return result;
}


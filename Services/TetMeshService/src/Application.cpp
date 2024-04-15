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
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityMeshTet.h"
#include "GmshMeshCreation.h"
#include "MeshExport.h"
#include "MeshImport.h"

// Open twin header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

#include "ModelState.h"

#include <thread>	
#include <filesystem>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_TetMeshService

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_TetMeshService

std::string Application::materialsFolder;
ot::UID Application::materialsFolderID = 0;

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
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{
	getClassFactory().SetNextHandler(&classFactoryCAD);
	classFactoryCAD.SetChainRoot(&(getClassFactory()));
}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::modelSelectionChangedNotification(void)
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (selectedEntities.size() > 0)
		{
			enabled.push_back("Mesh:Tet Mesh:Update Tet Mesh");
			enabled.push_back("Mesh:Import / Export:Export Tet Mesh");
		}
		else
		{
			disabled.push_back("Mesh:Tet Mesh:Update Tet Mesh");
			disabled.push_back("Mesh:Import / Export:Export Tet Mesh");
		}

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Mesh");

	_ui->addMenuGroup("Mesh", "Tet Mesh");
	_ui->addMenuGroup("Mesh", "Import / Export");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("Mesh", "Tet Mesh", "Create Tet Mesh", "Create Tet Mesh", modelWrite, "AddAllObjects", "Default");
	_ui->addMenuButton("Mesh", "Tet Mesh", "Update Tet Mesh", "Update Tet Mesh", modelWrite, "UpdateMesh", "Default");

	_ui->addMenuButton("Mesh", "Import / Export", "Import Tet Mesh", "Import Tet Mesh", modelWrite, "Import", "Default");
	_ui->addMenuButton("Mesh", "Import / Export", "Export Tet Mesh", "Export Tet Mesh", modelWrite, "ProjectSaveAs", "Default");

	modelSelectionChangedNotification();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{
}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "Mesh:Tet Mesh:Create Tet Mesh")	createMesh();
	else if (action == "Mesh:Tet Mesh:Update Tet Mesh")	updateMesh();
	else if (action == "Mesh:Import / Export:Import Tet Mesh") importMesh();
	else if (action == "Mesh:Import / Export:Export Tet Mesh") exportMesh();
	else assert(0); // Unhandled button action
	return std::string();
}

std::string Application::handleExecuteFunction(ot::JsonDocument& _document) {

	std::string function = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);

	if (function == "exportMeshFile")
	{
		std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);

		exportMeshFile(fileName);
	}
	else if (function == "importMeshFile")
	{
		std::string originalName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);

		std::string content = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
		ot::UID uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

		// Process the file content
		importMeshFile(originalName, content, uncompressedDataLength);
	}
	else
	{
		assert(0);  // Unknown function to execute
	}

	return std::string();
}

std::string Application::handleModelSelectionChanged(ot::JsonDocument& _document) {
	selectedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	modelSelectionChangedNotification();
	return std::string();
}

void Application::createMesh(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> meshItems = m_modelComponent->getListOfFolderItems("Meshes");

	// Create a unique name for the new mesh item
	int count = 1;
	std::string meshName;
	do
	{
		meshName = "Meshes/Tet " + std::to_string(count);
		count++;
	} while (std::find(meshItems.begin(), meshItems.end(), meshName) != meshItems.end());

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create the new mesh item
	EntityMeshTet *meshEntity = new EntityMeshTet(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());

	meshEntity->setName(meshName);
	meshEntity->setEditable(true);

	if (materialsFolder.empty())
	{
		// The materials folder information has not yet been retrieved. We get the information about the entity from the model
		materialsFolder = "Materials";

		std::list<std::string> entityList{materialsFolder};
		std::list<ot::EntityInformation> entityInfo;

		m_modelComponent->getEntityInformation(entityList, entityInfo);

		assert(entityInfo.size() == 1);
		assert(entityInfo.front().getName() == materialsFolder);

		materialsFolderID = entityInfo.front().getID();
	}

	meshEntity->createProperties(materialsFolder, materialsFolderID);

	meshEntity->StoreToDataBase();

	// Register the new mesh item in the model
	std::list<ot::UID> topologyEntityIDList = { meshEntity->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { meshEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create tet mesh");
}

void Application::updateMesh(void)
{
	if (!EnsureDataBaseConnection())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to connect to data base.\n");
		return;
	}

	if (selectedEntities.empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// We first get a list of all selected entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	m_modelComponent->getEntityInformation(selectedEntities, selectedEntityInfo);

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> mesherRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getType() == "EntityMeshTet")  
		{
			if (entity.getName().substr(0, 7) == "Meshes/")
			{
				size_t index = entity.getName().find('/', 7);
				if (index != std::string::npos)
				{
					mesherRunMap[entity.getName().substr(0, index - 1)] = true;
				}
				else
				{
					mesherRunMap[entity.getName()] = true;
				}
			}
		}
	}

	std::list<std::string> mesherRunList;
	for (auto mesher : mesherRunMap)
	{
		mesherRunList.push_back(mesher.first);
	}

	if (mesherRunList.empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No tet mesh item has been selected.\n");
		return;
	}

	// Now we retrieve information about the mesh items
	std::list<ot::EntityInformation> mesherInfo;
	m_modelComponent->getEntityInformation(mesherRunList, mesherInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsMesher;

	for (auto info : mesherInfo)
	{
		prefetchIdsMesher.push_back(std::pair<unsigned long long, unsigned long long>(info.getID(), info.getVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsMesher);

	// Now read the solver objects for each solver
	std::map<std::string, EntityMeshTet *> mesherMap;
	for (auto info : mesherInfo)
	{
		EntityMeshTet *entity = dynamic_cast<EntityMeshTet*> (m_modelComponent->readEntityFromEntityIDandVersion(info.getID(), info.getVersion(), getClassFactory()));
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			mesherMap[info.getName()] = entity;
		}
	}

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::mesherThread, this, mesherInfo, mesherMap);
	workerThread.detach();
}

void Application::mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityMeshTet *> mesherMap) 
{
	for (auto mesher : mesherInfo)
	{
		runSingleMesher(mesher, mesherMap[mesher.getName()]);
	}
}

void Application::runSingleMesher(ot::EntityInformation &mesher, EntityMeshTet *meshEntity)
{
	if (!EnsureDataBaseConnection()) return;

	GmshMeshCreation tetMesher(this);
	tetMesher.updateMesh(meshEntity);
}

void Application::exportMesh(void)
{
	if (getCurrentlySelectedMeshName().empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: You need to select a single mesh to export.\n");
		return;
	}

	MeshExport meshExporter(this);
	std::string fileExtensions = meshExporter.getFileExtensions();

	// First of all, we need to request a file name for the export from the frontend
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SelectFileForStoring, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Export Mesh File", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileExtensions, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("exportMeshFile", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::importMesh(void)
{
	MeshImport meshImporter(this);
	std::string fileExtensions = meshImporter.getFileExtensions();

	// Get a file name for the STEP file from the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import Mesh File", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileExtensions, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importMeshFile", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, true, doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::exportMeshFile(const std::string &fileName)
{
	if (!EnsureDataBaseConnection())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to connect to data base.\n");
		return;
	}

	// Determine the file extension (defines the type of the mesh export)
	size_t index = fileName.rfind('.');
	if (index == std::string::npos)
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: The export file type has not been specified. Please make sure to specify a valid file extension.\n");
		return;
	}

	std::string extension = fileName.substr(index);

	ot::UID currentMeshDataID = getCurrentlySelectedMeshDataID();
	if (currentMeshDataID == 0)
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to load the data of the selected mesh.\n");
		return;
	}

	MeshExport meshExporter(this);
	if (!meshExporter.loadMesh(currentMeshDataID))
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to load the data of the selected mesh.\n");
		return;
	}

	std::string fileContent;
	unsigned long long uncompressedDataLength = 0;
	meshExporter.getExportFileContent(extension, fileContent, uncompressedDataLength);

	meshExporter.cleanUp();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SaveFileContent, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Mesh Export", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, uncompressedDataLength, doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

std::string Application::getCurrentlySelectedMeshName(void)
{
	if (selectedEntities.empty()) return "";

	// We first get a list of all selected entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	m_modelComponent->getEntityInformation(selectedEntities, selectedEntityInfo);

	std::string selectedMeshItem;

	std::map<std::string, bool> mesherRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getType() == "EntityMeshTet")
		{
			if (entity.getName().substr(0, 7) == "Meshes/")
			{
				std::string thisMeshName;

				size_t index = entity.getName().find('/', 7);
				if (index != std::string::npos)
				{
					thisMeshName = entity.getName().substr(0, index - 1);
				}
				else
				{
					thisMeshName = entity.getName();
				}

				if (selectedMeshItem.empty())
				{
					selectedMeshItem = thisMeshName;
				}
				else
				{
					if (selectedMeshItem != thisMeshName)
					{
						return "";  // Multiple meshes selected
					}
				}
			}
		}
	}

	return selectedMeshItem;
}

ot::UID Application::getCurrentlySelectedMeshDataID(void)
{
	std::string currentMeshName = getCurrentlySelectedMeshName();
	if (currentMeshName.empty()) return 0;

	std::string currentMeshDataName = currentMeshName + "/Mesh";

	// Get the ID of the mesh data item
	std::list<string> entityList;
	std::list<ot::EntityInformation> entityInfo;

	entityList.push_back(currentMeshDataName);

	m_modelComponent->getEntityInformation(entityList, entityInfo);
	if (entityInfo.empty()) return 0;

	return entityInfo.front().getID();
}

void Application::importMeshFile(const std::string& originalName, const std::string& content, ot::UID uncompressedDataLength)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// Get the file name from the full path of the original file
	std::string meshBaseName = "Meshes/" + std::filesystem::path(originalName).stem().string();

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> meshItems = m_modelComponent->getListOfFolderItems("Meshes");

	// Determine a unique name for the new mesh
	std::string meshName = meshBaseName;

	if (std::find(meshItems.begin(), meshItems.end(), meshName) != meshItems.end())
	{
		// Create a unique name for the new mesh item
		int count = 1;
		do
		{
			meshName = meshBaseName + " " + std::to_string(count);
			count++;
		} while (std::find(meshItems.begin(), meshItems.end(), meshName) != meshItems.end());
	}

	// Run a worker thread to import the mesh and store it with the given name
	std::thread workerThread(&Application::exportMeshThread, this, meshName, originalName, content, uncompressedDataLength);
	workerThread.detach();
}

void Application::exportMeshThread(std::string meshName, std::string originalName, std::string content, ot::UID uncompressedDataLength)
{
	MeshImport meshImport(this);
	meshImport.importMesh(meshName, originalName, content, uncompressedDataLength);
}

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
#include "CartesianMeshCreation.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityMeshCartesian.h"

// Open twin header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "ModelState.h"

#include <thread>	

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_CartesianMeshService

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_CartesianMeshService

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

std::string Application::processAction(const std::string & _action, ot::JsonDocument & _doc)
{
	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument & _doc)
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
			enabled.push_back("Mesh:Cartesian Mesh:Update Cartesian Mesh");
		}
		else
		{
			disabled.push_back("Mesh:Cartesian Mesh:Update Cartesian Mesh");
		}

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Mesh");

	_ui->addMenuGroup("Mesh", "Cartesian Mesh");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("Mesh", "Cartesian Mesh", "Create Cartesian Mesh", "Create Cartesian Mesh", modelWrite, "AddAllObjects", "Default");
	_ui->addMenuButton("Mesh", "Cartesian Mesh", "Update Cartesian Mesh", "Update Cartesian Mesh", modelWrite, "UpdateMesh", "Default");

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

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "Mesh:Cartesian Mesh:Create Cartesian Mesh")	createMesh();
	else if (action == "Mesh:Cartesian Mesh:Update Cartesian Mesh")	updateMesh();
	else assert(0); // Unhandled button action
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
	std::list<std::string> meshItems = ot::ModelServiceAPI::getListOfFolderItems("Meshes");

	// Create a unique name for the new mesh item
	int count = 1;
	std::string meshName;
	do
	{
		meshName = "Meshes/Cartesian " + std::to_string(count);
		count++;
	} while (std::find(meshItems.begin(), meshItems.end(), meshName) != meshItems.end());

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create the new mesh item
	EntityMeshCartesian *meshEntity = new EntityMeshCartesian(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());

	meshEntity->setName(meshName);
	meshEntity->setEditable(true);

	if (materialsFolder.empty())
	{
		// The materials folder information has not yet been retrieved. We get the information about the entity from the model
		materialsFolder = "Materials";

		std::list<std::string> entityList{materialsFolder};
		std::list<ot::EntityInformation> entityInfo;

		ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);

		assert(entityInfo.size() == 1);
		assert(entityInfo.front().getEntityName() == materialsFolder);

		materialsFolderID = entityInfo.front().getEntityID();
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

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create cartesian mesh");
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
	ot::ModelServiceAPI::getEntityInformation(selectedEntities, selectedEntityInfo);

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> mesherRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getEntityType() == "EntityMeshCartesian")
		{
			if (entity.getEntityName().substr(0, 7) == "Meshes/")
			{
				size_t index = entity.getEntityName().find('/', 7);
				if (index != std::string::npos)
				{
					mesherRunMap[entity.getEntityName().substr(0, index - 1)] = true;
				}
				else
				{
					mesherRunMap[entity.getEntityName()] = true;
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
		m_uiComponent->displayMessage("\nERROR: No cartesian mesh item has been selected.\n");
		return;
	}

	// Now we retrieve information about the mesh items
	std::list<ot::EntityInformation> mesherInfo;
	ot::ModelServiceAPI::getEntityInformation(mesherRunList, mesherInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsMesher;

	for (auto info : mesherInfo)
	{
		prefetchIdsMesher.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsMesher);

	// Now read the solver objects for each solver
	std::map<std::string, EntityBase *> mesherMap;
	for (auto info : mesherInfo)
	{
		EntityBase *entity = ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion(), getClassFactory());
		mesherMap[info.getEntityName()] = entity;
	}

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::mesherThread, this, mesherInfo, mesherMap);
	workerThread.detach();
}

void Application::mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityBase *> mesherMap) 
{
	for (auto mesher : mesherInfo)
	{
		runSingleMesher(mesher, mesherMap[mesher.getEntityName()]);
	}
}

void Application::runSingleMesher(ot::EntityInformation &mesher, EntityBase *meshEntity)
{
	if (!EnsureDataBaseConnection()) return;

	CartesianMeshCreation cartesianMesher;
	cartesianMesher.updateMesh(this, meshEntity);
}


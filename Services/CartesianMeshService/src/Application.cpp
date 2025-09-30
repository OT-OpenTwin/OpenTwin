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
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier()),
	visualizationModelID(-1)
{
	getClassFactory().SetNextHandler(&classFactoryCAD);
	classFactoryCAD.SetChainRoot(&(getClassFactory()));
}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Mesh");

	_ui->addMenuGroup("Mesh", "Cartesian Mesh");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("Mesh", "Cartesian Mesh", "Create Cartesian Mesh", "Create Cartesian Mesh", modelWrite, "AddAllObjects", "Default");
	_ui->addMenuButton("Mesh", "Cartesian Mesh", "Update Cartesian Mesh", "Update Cartesian Mesh", modelWrite, "UpdateMesh", "Default");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::modelSelectionChanged()
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (this->getSelectedEntities().size() > 0)
		{
			enabled.push_back("Mesh:Cartesian Mesh:Update Cartesian Mesh");
		}
		else
		{
			disabled.push_back("Mesh:Cartesian Mesh:Update Cartesian Mesh");
		}

		this->getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "Mesh:Cartesian Mesh:Create Cartesian Mesh")	createMesh();
	else if (action == "Mesh:Cartesian Mesh:Update Cartesian Mesh")	updateMesh();
	else assert(0); // Unhandled button action
	return std::string();
}


void Application::createMesh(void)
{
	if (this->getUiComponent() == nullptr) {
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
	ot::UID entityID = this->getModelComponent()->createEntityUID();

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
	if (this->getSelectedEntities().empty())
	{
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}


	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> mesherRunMap;
	for (auto& entity : this->getSelectedEntityInfos())
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
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No cartesian mesh item has been selected.\n");
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
	CartesianMeshCreation cartesianMesher;
	cartesianMesher.updateMesh(this, meshEntity);
}


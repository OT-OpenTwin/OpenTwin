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
#include "OTCommunication/actionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"

// Application specific includes
#include "FDTDLauncher.h"
#include "EntitySolverFDTD.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_FDTD

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_FDTD

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{
	m_runSolverButton = ot::ToolBarButtonCfg("FDTD", "Solver", "Run Solver", "Default/RunSolver");
	m_runSolverButton.setButtonLockFlags(ot::LockModelWrite);
	m_createSolverButton = ot::ToolBarButtonCfg("FDTD", "Solver", "Create Solver", "Default/AddSolver");
	m_createSolverButton.setButtonLockFlags(ot::LockModelWrite);
	connectToolBarButton(m_createSolverButton, this, &Application::addSolver);
	connectToolBarButton(m_runSolverButton, this, &Application::runSolver);
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

	const std::string pageName = "FDTD";

	_ui->addMenuPage(pageName);

	const std::string groupNameSolver = "Solver";
	//const std::string groupNameSignal = "Signal Type";
	//const std::string groupNamePort = "Port";

	_ui->addMenuGroup(pageName, groupNameSolver);
	//_ui->addMenuGroup(pageName, groupNameSignal);
	//_ui->addMenuGroup(pageName, groupNamePort);

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);


	_ui->addMenuButton(pageName, groupNameSolver, "Create Solver", "Create Solver", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton(pageName, groupNameSolver, "Run Solver", "Run Solver", modelWrite, "RunSolver", "Default");
	//_ui->addMenuButton(pageName, groupNameSignal, "Create Signal", "Create Signal", modelWrite, "Signal", "Default");
	//_ui->addMenuButton(pageName, groupNamePort, "Add Port", "Add Port", modelWrite, "Port", "Default");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged()
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (getSelectedEntities().size() > 0)
		{
			enabled.push_back("FDTD:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("FDTD:Solver:Run Solver");
		}

		getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

void Application::EnsureVisualizationModelIDKnown(void)
{
	if (visualizationModelID > 0) return;
	if (getModelComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// The visualization model isnot known yet -> get it from the model
	visualizationModelID = ot::ModelServiceAPI::getCurrentVisualizationModelID();
}

void Application::addSolver(void)
{
	if (getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = getModelComponent()->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = "Solvers/FDTD" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverFDTD *solverEntity = new EntitySolverFDTD(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);
	solverEntity->createProperties(meshFolderName, meshFolderID, meshName, meshID);

	solverEntity->StoreToDataBase();

	// Register the new solver item in the model
	std::list<ot::UID> topologyEntityIDList = { solverEntity->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { solverEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create solver");
}

void Application::runSolver(void)
{
	if (getSelectedEntities().empty())
	{
		if (getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto& entity : getSelectedEntityInfos())
	{
		if (entity.getEntityType() == "EntitySolverFDTD")
		{
			if (entity.getEntityName().substr(0, 8) == "Solvers/")
			{
				size_t index = entity.getEntityName().find('/', 8);
				if (index != std::string::npos)
				{
					solverRunMap[entity.getEntityName().substr(0, index - 1)] = true;
				}
				else
				{
					solverRunMap[entity.getEntityName()] = true;
				}
			}
		}
	}

	std::list<std::string> solverRunList;
	for (auto solver : solverRunMap)
	{
		solverRunList.push_back(solver.first);
	}

	if (solverRunList.empty())
	{
		if (getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// Now we retrieve information about the solver items
	std::list<ot::EntityInformation> solverInfo;
	ot::ModelServiceAPI::getEntityInformation(solverRunList, solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : solverInfo)
	{
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsSolver);

	// Now read the solver objects for each solver
	std::list<std::string> meshNameList;
	std::map<std::string, EntityBase *> solverMap;
	for (auto info : solverInfo)
	{
		EntityBase *entity = ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion(), getClassFactory());
		solverMap[info.getEntityName()] = entity;
	}

	std::list<ot::EntityInformation> meshInfo;
	ot::ModelServiceAPI::getEntityChildInformation("Meshes", meshInfo, false);

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::solverThread, this, solverInfo, meshInfo, solverMap);
	workerThread.detach();
}

void Application::solverThread(std::list<ot::EntityInformation> solverInfo, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase *> solverMap) {
	ot::LockTypeFlags lock;
	lock.setFlag(ot::LockModelWrite);
	lock.setFlag(ot::LockNavigationWrite);
	lock.setFlag(ot::LockViewWrite);
	lock.setFlag(ot::LockProperties);

	getUiComponent()->lockUI(lock);

	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, meshInfo, solverMap[solver.getEntityName()]);
	}

	getUiComponent()->unlockUI(lock);
}

void Application::runSingleSolver(ot::EntityInformation &solver, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity) 
{
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	getUiComponent()->displayMessage("\nFDTD solver started: " + solverName + "\n\n");

	if (solverEntity == nullptr)
	{
		getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
		return;
	}

	EntityPropertiesEntityList *mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);

	if (mesh == nullptr)
	{
		getUiComponent()->displayMessage("ERROR: Unable to read mesh information for solver.\n");
		return;
	}

	// First check whether a mesh with the given ID exists
	bool meshFound = false;
	ot::UID meshEntityID = mesh->getValueID();

	for (auto meshItem : meshInfo)
	{
		if (meshItem.getEntityID() == meshEntityID)
		{
			meshFound = true;
			break;
		}
	}

	// If the mesh with the given ID does not exist anymore, we search for a mesh with the same name as the last selected one.
	if (!meshFound)
	{
		meshEntityID = 0;
		for (auto meshItem : meshInfo)
		{
			if (meshItem.getEntityName() == mesh->getValueName())
			{
				meshEntityID = meshItem.getEntityID();
				break;
			}
		}
	}

	if (meshEntityID == 0)
	{
		getUiComponent()->displayMessage("ERROR: The specified mesh does not exist: " + mesh->getValueName() + "\n");
		return;
	}

	deleteSingleSolverResults(solverEntity);

	FDTDLauncher FDTDSolver(this);
	getModelComponent()->clearNewEntityList();

	std::string logFileText;
	std::string output = FDTDSolver.startSolver(logFileText, DataBase::GetDataBase()->getDataBaseServerURL(), getUiComponent()->getServiceURL(),
												 DataBase::GetDataBase()->getProjectName(), solverEntity, getServiceID(), getSessionCount(), getModelComponent());
	getUiComponent()->displayMessage(output + "\n");

	// Store the output in a result item

	EntityResultText *text = getModelComponent()->addResultTextEntity(solver.getEntityName() + "/Output", logFileText + output);

	getModelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	// Store the newly created items in the data base
	getModelComponent()->storeNewEntities("added solver results");
}

void Application::deleteSingleSolverResults(EntityBase* solverEntity)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(solverEntity->getName() + "/Results");
	entityNameList.push_back(solverEntity->getName() + "/Output");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);
}

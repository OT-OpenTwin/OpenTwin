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
#include "OTCommunication/ActionDispatcher.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

// Application specific includes
#include "ElmerFEMLauncher.h"
#include "EntitySolverElmerFEM.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_ELMERFEM

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_ELMERFEM

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{
	connectAction(OT_ACTION_CMD_MODEL_ExecuteAction, this, &Application::handleExecuteModelAction);
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
	_ui->addMenuPage("ElmerFEM");
	_ui->addMenuPage("Model");

	//_ui->addMenuGroup("Model", "Sources");
	_ui->addMenuGroup("ElmerFEM", "Solver");
	_ui->addMenuGroup("ElmerFEM", "Sources");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("ElmerFEM", "Solver", "Create Solver", "Create Solver", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("ElmerFEM", "Solver", "Run Solver", "Run Solver", modelWrite, "RunSolver", "Default");

	_ui->addMenuButton("ElmerFEM", "Sources", "Define Electrostatic Potential", "Define Electrostatic Potential", modelWrite, "DefinePotential", "Default");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "ElmerFEM:Solver:Create Solver")	      addSolver();
	else if (action == "ElmerFEM:Solver:Run Solver")		  runSolver();
	else if (action == "ElmerFEM:Sources:Define Electrostatic Potential")  definePotential();
	else assert(0); // Unhandled button action
}

void Application::modelSelectionChanged() 
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (this->getSelectedEntities().size() > 0)
		{
			enabled.push_back("ElmerFEM:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("ElmerFEM:Solver:Run Solver");
		}

		this->getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

void Application::definePotential(void)
{
	// Get the list of all geometry entities from the model service
	std::list<ot::EntityInformation> selectedGeometryEntities, selectedTetMeshEntities;
	ot::ModelServiceAPI::getSelectedEntityInformation(selectedGeometryEntities, "EntityGeometry");
	ot::ModelServiceAPI::getSelectedEntityInformation(selectedTetMeshEntities, "EntityMeshTetItem");

	// Get an entity list
	ot::UIDList updateEntities;
	for (auto entity : selectedGeometryEntities) updateEntities.push_back(entity.getEntityID());
	for (auto entity : selectedTetMeshEntities) updateEntities.push_back(entity.getEntityID());

	// Create a property object with the new properties and get it as JSON string
	EntityProperties properties;
	EntityPropertiesDouble::createProperty("Solver", "Electrostatic Potential", 0.0, "ElmerFEM", properties);
	properties.setAllPropertiesNonProtected();

	ot::PropertyGridCfg cfg;
	properties.addToConfiguration(nullptr, false, cfg);

	// Set the message to the model service
	ot::ModelServiceAPI::addPropertiesToEntities(updateEntities, cfg);

	ot::ModelServiceAPI::modelChangeOperationCompleted("Added electrostatic potential definitions");
}

void Application::addSolver(void) {
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = this->getModelComponent()->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = "Solvers/ElmerFEM" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverElmerFEM*solverEntity = new EntitySolverElmerFEM(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
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

void Application::runSolver(void) {
	if (this->getSelectedEntities().empty())
	{
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto& entity : this->getSelectedEntityInfos())
	{
		if (entity.getEntityType() == "EntitySolverElmerFEM")
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
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
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

	this->getUiComponent()->lockUI(lock);

	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, meshInfo, solverMap[solver.getEntityName()]);
	}

	this->getUiComponent()->unlockUI(lock);
}

void Application::runSingleSolver(ot::EntityInformation &solver, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity) 
{
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	this->getUiComponent()->displayMessage("\nElmer FEM solver started: " + solverName + "\n\n");

	if (solverEntity == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
		return;
	}

	EntityPropertiesEntityList *mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);

	if (mesh == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read mesh information for solver.\n");
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
		this->getUiComponent()->displayMessage("ERROR: The specified mesh does not exist: " + mesh->getValueName() + "\n");
		return;
	}

	deleteSingleSolverResults(solverEntity);

	ElmerFEMLauncher elmerFEMSolver(this);
	getModelComponent()->clearNewEntityList();

	std::string logFileText;
	std::string output = elmerFEMSolver.startSolver(logFileText, DataBase::GetDataBase()->getDataBaseServerURL(), this->getUiComponent()->getServiceURL(),
												    DataBase::GetDataBase()->getProjectName(), solverEntity, static_cast<int>(getServiceID()), getSessionCount(), this->getModelComponent());
	this->getUiComponent()->displayMessage(output + "\n");

	// Store the output in a result item

	EntityResultText *text = this->getModelComponent()->addResultTextEntity(solver.getEntityName() + "/Output", logFileText + output);

	getModelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	// Store the newly created items in the data base
	this->getModelComponent()->storeNewEntities("added solver results");
}

void Application::deleteSingleSolverResults(EntityBase* solverEntity)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(solverEntity->getName() + "/Results");
	entityNameList.push_back(solverEntity->getName() + "/Output");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);
}

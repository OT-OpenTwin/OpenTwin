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

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	// This method is called once the service can start its operation
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
}

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("FDTD");

	_ui->addMenuGroup("FDTD", "Solver");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("FDTD", "Solver", "Create Solver", "Create Solver", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("FDTD", "Solver", "Run Solver", "Run Solver", modelWrite, "RunSolver", "Default");

	modelSelectionChanged();

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

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "FDTD:Solver:Create Solver")	          addSolver();
	else if (action == "FDTD:Solver:Run Solver")		  runSolver();
	else assert(0); // Unhandled button action
	return std::string();
}

void Application::modelSelectionChanged()
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (m_selectedEntities.size() > 0)
		{
			enabled.push_back("FDTD:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("FDTD:Solver:Run Solver");
		}

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}
}

void Application::EnsureVisualizationModelIDKnown(void)
{
	if (visualizationModelID > 0) return;
	if (m_modelComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// The visualization model isnot known yet -> get it from the model
	visualizationModelID = ot::ModelServiceAPI::getCurrentVisualizationModelID();
}

void Application::addSolver(void)
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
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

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
	if (!EnsureDataBaseConnection())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to connect to data base.\n");
		return;
	}

	if (m_selectedEntities.empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto& entity : m_selectedEntityInfos)
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
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
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

	m_uiComponent->lockUI(lock);

	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, meshInfo, solverMap[solver.getEntityName()]);
	}

	m_uiComponent->unlockUI(lock);
}

void Application::runSingleSolver(ot::EntityInformation &solver, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity) 
{
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	m_uiComponent->displayMessage("\nFDTD solver started: " + solverName + "\n\n");

	if (solverEntity == nullptr)
	{
		m_uiComponent->displayMessage("ERROR: Unable to read solver information.\n");
		return;
	}

	EntityPropertiesEntityList *mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);

	if (mesh == nullptr)
	{
		m_uiComponent->displayMessage("ERROR: Unable to read mesh information for solver.\n");
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
		m_uiComponent->displayMessage("ERROR: The specified mesh does not exist: " + mesh->getValueName() + "\n");
		return;
	}

	deleteSingleSolverResults(solverEntity);

	FDTDLauncher fdtdSolver(this);
	modelComponent()->clearNewEntityList();

	std::string logFileText;
	std::string output = fdtdSolver.startSolver(logFileText, DataBase::GetDataBase()->getDataBaseServerURL(), m_uiComponent->getServiceURL(),
												 DataBase::GetDataBase()->getProjectName(), solverEntity, getServiceIDAsInt(), getSessionCount(), m_modelComponent);
	m_uiComponent->displayMessage(output + "\n");

	// Store the output in a result item

	EntityResultText *text = m_modelComponent->addResultTextEntity(solver.getEntityName() + "/Output", logFileText + output);

	modelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	modelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	// Store the newly created items in the data base
	m_modelComponent->storeNewEntities("added solver results");
}

void Application::deleteSingleSolverResults(EntityBase* solverEntity)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(solverEntity->getName() + "/Results");
	entityNameList.push_back(solverEntity->getName() + "/Output");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);
}

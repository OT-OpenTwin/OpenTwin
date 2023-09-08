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
#include "OpenTwinCore/rJSON.h"				// json convenience functions
#include "OpenTwinCommunication/actionTypes.h"		// action member and types definition
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"

// Application specific includes
#include "GetDPLauncher.h"
#include "EntitySolverGetDP.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_GETDP

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_GETDP

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

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing("uiService", true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("GetDP");
	_ui->addMenuPage("Model");

	//_ui->addMenuGroup("Model", "Sources");
	_ui->addMenuGroup("GetDP", "Solver");
	_ui->addMenuGroup("GetDP", "Sources");

	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	//_ui->addMenuButton("Model", "Sources", "Add Terminal", "Add Terminal", modelWrite, "FaceSelect", "Default");
	_ui->addMenuButton("GetDP", "Solver", "Create Solver", "Create Solver", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("GetDP", "Solver", "Run Solver", "Run Solver", modelWrite, "RunSolver", "Default");

	_ui->addMenuButton("GetDP", "Sources", "Define Electrostatic Potential", "Define Electrostatic Potential", modelWrite, "DefinePotential", "Default");

	modelSelectionChangedNotification();

	enableMessageQueuing("uiService", false);
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

std::string Application::handleExecuteModelAction(OT_rJSON_doc& _document) {
	std::string action = ot::rJSON::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "GetDP:Solver:Create Solver")	          addSolver();
	else if (action == "GetDP:Solver:Run Solver")		  runSolver();
	else if (action == "Model:Sources:Add Terminal")	  addTerminal();
	else if (action == "GetDP:Sources:Define Electrostatic Potential")  definePotential();
	else assert(0); // Unhandled button action
	return std::string();
}

std::string Application::handleModelSelectionChanged(OT_rJSON_doc& _document) {
	selectedEntities = ot::rJSON::getULongLongList(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	modelSelectionChangedNotification();
	return std::string();
}

void Application::modelSelectionChangedNotification(void)
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (selectedEntities.size() > 0)
		{
			enabled.push_back("GetDP:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("GetDP:Solver:Run Solver");
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
	visualizationModelID = m_modelComponent->getCurrentVisualizationModelID();
}

void Application::addTerminal(void)
{
	EnsureVisualizationModelIDKnown();
	if (visualizationModelID == 0) return;

	std::map<std::string, std::string> options;
	options.insert_or_assign("colorR", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("GetDP", "Terminal color", 0, 255)));
	options.insert_or_assign("colorG", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("GetDP", "Terminal color", 1, 171)));
	options.insert_or_assign("colorB", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("GetDP", "Terminal color", 2, 0)));
	options.insert_or_assign("BaseName", "Terminals/terminal");
	options.insert_or_assign("ModelStateName", "create terminal");

	ot::ServiceBase *receiver = getConnectedServiceByName("Model");

	if (receiver != nullptr)
	{
		m_uiComponent->enterEntitySelectionMode(visualizationModelID, ot::components::UiComponent::FACE, true, "", ot::components::UiComponent::PORT, "create a new terminal", options, receiver->serviceID());
	}
}

void Application::definePotential(void)
{
	// Get the list of all geometry entities from the model service
	std::list<ot::EntityInformation> selectedGeometryEntities, selectedTetMeshEntities;
	m_modelComponent->getSelectedEntityInformation(selectedGeometryEntities, "EntityGeometry");
	m_modelComponent->getSelectedEntityInformation(selectedTetMeshEntities, "EntityMeshTetItem");

	// Get an entity list
	ot::UIDList updateEntities;
	for (auto entity : selectedGeometryEntities) updateEntities.push_back(entity.getID());
	for (auto entity : selectedTetMeshEntities) updateEntities.push_back(entity.getID());

	// Create a property object with the new properties and get it as JSON string
	EntityProperties properties;
	EntityPropertiesDouble::createProperty("Solver", "Electrostatic Potential", 0.0, "GetDP", properties);
	properties.setAllPropertiesNonProtected();

	std::string propertiesJson = properties.getJSON(nullptr, false);

	// Set the message to the model service
	m_modelComponent->addPropertiesToEntities(updateEntities, propertiesJson);

	m_modelComponent->modelChangeOperationCompleted("Added electrostatic potential definitions");
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
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = "Solvers/GetDP" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	m_modelComponent->getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverGetDP *solverEntity = new EntitySolverGetDP(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());
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

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
										 dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create solver");
}

void Application::runSolver(void)
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
	std::map<std::string, bool> solverRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getType() == "EntitySolverGetDP")
		{
			if (entity.getName().substr(0, 8) == "Solvers/")
			{
				size_t index = entity.getName().find('/', 8);
				if (index != std::string::npos)
				{
					solverRunMap[entity.getName().substr(0, index - 1)] = true;
				}
				else
				{
					solverRunMap[entity.getName()] = true;
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
	m_modelComponent->getEntityInformation(solverRunList, solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : solverInfo)
	{
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getID(), info.getVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsSolver);

	// Now read the solver objects for each solver
	std::list<std::string> meshNameList;
	std::map<std::string, EntityBase *> solverMap;
	for (auto info : solverInfo)
	{
		ClassFactory classFactory;
		EntityBase *entity = m_modelComponent->readEntityFromEntityIDandVersion(info.getID(), info.getVersion(), classFactory);
		solverMap[info.getName()] = entity;
	}

	std::list<ot::EntityInformation> meshInfo;
	m_modelComponent->getEntityChildInformation("Meshes", meshInfo);

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::solverThread, this, solverInfo, meshInfo, solverMap);
	workerThread.detach();
}

void Application::solverThread(std::list<ot::EntityInformation> solverInfo, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase *> solverMap) {
	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, meshInfo, solverMap[solver.getName()]);
	}
}

void Application::runSingleSolver(ot::EntityInformation &solver, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity) 
{
	std::string solverName = solver.getName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	m_uiComponent->displayMessage("\nGetDP solver started: " + solverName + "\n\n");

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
		if (meshItem.getID() == meshEntityID)
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
			if (meshItem.getName() == mesh->getValueName())
			{
				meshEntityID = meshItem.getID();
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

	GetDPLauncher getDPSolver(this);
	modelComponent()->clearNewEntityList();

	std::string logFileText;
	std::string output = getDPSolver.startSolver(logFileText, DataBase::GetDataBase()->getDataBaseServerURL(), m_uiComponent->serviceURL(),
												 DataBase::GetDataBase()->getProjectName(), solverEntity, getServiceIDAsInt(), getSessionCount(), m_modelComponent);
	m_uiComponent->displayMessage(output + "\n");

	// Store the output in a result item

	EntityResultText *text = m_modelComponent->addResultTextEntity(solver.getName() + "/Output", logFileText + output);

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

	modelComponent()->deleteEntitiesFromModel(entityNameList, false);
}

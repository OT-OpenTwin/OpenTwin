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
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/ActionTypes.h"	// action member and types definition
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

// Application specific includes
#include "PHREECLauncher.h"
#include "EntitySolverPHREEC.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_PHREEC

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_PHREEC

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

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
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
	_ui->addMenuPage("PHREEC");

	_ui->addMenuGroup("PHREEC", "Sources");
	_ui->addMenuGroup("PHREEC", "Solver");

	ot::LockTypeFlags modelWrite;
	modelWrite.setFlag(ot::LockModelWrite);

	_ui->addMenuButton("PHREEC", "Sources", "Add Terminal", "Add Terminal", modelWrite, "FaceSelect", "Default", "Ctrl+T");
	_ui->addMenuButton("PHREEC", "Solver", "Create Solver", "Create Solver", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("PHREEC", "Solver", "Run Solver", "Run Solver", modelWrite, "RunSolver", "Default", "F6");
	
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
	if (action == "PHREEC:Solver:Create Solver")	addSolver();
	else if (action == "PHREEC:Solver:Run Solver")		runPHREEC();
	else if (action == "PHREEC:Sources:Add Terminal")	addTerminal();
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
			enabled.push_back("PHREEC:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("PHREEC:Solver:Run Solver");
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

void Application::addTerminal(void)
{
	EnsureVisualizationModelIDKnown();
	if (visualizationModelID == 0) return;

	std::map<std::string, std::string> options;
	options.insert_or_assign("colorR", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("PHREEC", "Terminal color", 0, 255)));
	options.insert_or_assign("colorG", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("PHREEC", "Terminal color", 1, 171)));
	options.insert_or_assign("colorB", std::to_string(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("PHREEC", "Terminal color", 2, 0)));
	options.insert_or_assign("BaseName", "Terminals/terminal");
	options.insert_or_assign("ModelStateName", "create terminal");

	ot::ServiceBase *receiver = getConnectedServiceByName("Model");

	if (receiver != nullptr)
	{
		m_uiComponent->enterEntitySelectionMode(visualizationModelID, ot::components::UiComponent::FACE, true, "", ot::components::UiComponent::PORT, "create a new terminal", options, receiver->getServiceID());
	}
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
		solverName = "Solvers/PHREEC" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverPHREEC *solverEntity = new EntitySolverPHREEC(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);

	solverEntity->getProperties().createProperty(new EntityPropertiesEntityList("Mesh", meshFolderName, meshFolderID, meshName, meshID), "Mesh");
	EntityPropertiesDouble::createProperty("Solver", "Frequency [MHz]", 0.0, "PHREEC", solverEntity->getProperties());
	EntityPropertiesBoolean::createProperty("Specials", "Debug", false, "PHREEC", solverEntity->getProperties());

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

void Application::runPHREEC(void)
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
		if (entity.getEntityType() == "EntitySolverPHREEC" || entity.getEntityType() == "EntitySolver")  // EntitySolver for backward compatibility
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

	// Get the current model version
	std::string modelVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::solverThread, this, solverInfo, modelVersion, meshInfo, solverMap);
	workerThread.detach();
}

void Application::solverThread(std::list<ot::EntityInformation> solverInfo, std::string modelVersion, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase *> solverMap) {
	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, modelVersion, meshInfo, solverMap[solver.getEntityName()]);
	}
}

void Application::runSingleSolver(ot::EntityInformation &solver, std::string &modelVersion, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity) {
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	m_uiComponent->displayMessage("\nPHREEC solver started: " + solverName + "\n\n");

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

	EntityPropertiesBoolean *debug = dynamic_cast<EntityPropertiesBoolean*>(solverEntity->getProperties().getProperty("Debug"));
	assert(debug != nullptr);

	bool debugFlag = false;
	if (debug != nullptr) debugFlag = debug->getValue();

	std::string frequencyHz = "0";

	EntityPropertiesDouble *frequency = dynamic_cast<EntityPropertiesDouble*>(solverEntity->getProperties().getProperty("Frequency [MHz]"));
	if (frequency != nullptr)
	{
		double frequencyMHz = frequency->getValue();
		frequencyHz = std::to_string(frequencyMHz * 1e6);
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

	PHREECLauncher phreecSolver(this);

	std::string output = phreecSolver.startSolver(DataBase::GetDataBase()->getDataBaseServerURL(), frequencyHz, m_uiComponent->getServiceURL(),
		DataBase::GetDataBase()->getProjectName(), modelVersion, meshEntityID, debugFlag, getServiceIDAsInt(), getSessionCount(), m_modelComponent);
	m_uiComponent->displayMessage(output + "\n");

	// Store the output in a result item

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible;
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	EntityResultText *text = m_modelComponent->addResultTextEntity(solver.getEntityName() + "/Output", output);

	topologyEntityIDList.push_back(text->getEntityID());
	topologyEntityVersionList.push_back(text->getEntityStorageVersion());
	topologyEntityForceVisible.push_back(false);

	dataEntityIDList.push_back(text->getTextDataStorageId());
	dataEntityVersionList.push_back(text->getTextDataStorageVersion());
	dataEntityParentList.push_back(text->getEntityID());

	// Test of curve visualization (needs to be removed)
	// REMOVE FROM HERE
	
	/*
	std::string resultRawBase = solver.getName() + "/$raw/1D/";
	std::string result1DBase = solver.getName() + "/1D/";

	{
		int ncurves = 11;

		std::list<std::pair<UID, std::string>> curvesList;

		m_modelComponent->reserveNewEntityUIDs(2 * ncurves + 1);

		for (int icurve = 0; icurve < ncurves; icurve++)
		{
			std::vector<double> xdata, ydataRe, ydataIm;
			xdata.reserve(1000);
			ydataRe.reserve(1000);

			for (int i = 0; i < 1000; i++)
			{
				double x = 2 * 3.141592654 / 1000 * i;
				double y = sin(x - 3.141592654 / ncurves * icurve);

				xdata.push_back(x);
				ydataRe.push_back(y);
			}

			std::string name = "linear_" + std::to_string(icurve + 1);

			UID curveID = m_modelComponent->addResult1DEntity(m_modelComponent->grabNextReservedEntityUID(), m_modelComponent->grabNextReservedEntityUID(),
				resultRawBase + name, xdata, ydataRe, ydataIm, "Frequency", "GHz", "", "dB", icurve);

			curvesList.push_back(std::pair<UID, std::string>(curveID, name));

			entityIDList.push_back(curveID);
			entityVersionList.push_back(1);
			entityParentList.push_back(solverEntity->getEntityID());
		}

		UID plotID = m_modelComponent->addPlot1DEntity(m_modelComponent->grabNextReservedEntityUID(), result1DBase + "Linear scale", "test curves", curvesList);
		entityIDList.push_back(plotID);
		entityVersionList.push_back(1);
		entityParentList.push_back(0);
	}

	{
		std::list<std::pair<UID, std::string>> curvesList;

		m_modelComponent->reserveNewEntityUIDs(2 + 1);

		std::vector<double> xdata, ydataRe, ydataIm;
		xdata.reserve(1000);
		ydataRe.reserve(1000);

		for (int i = 0; i < 1000; i++)
		{
			double x = 10e9 / 1000 * i;
			double y = x;

			xdata.push_back(x);
			ydataRe.push_back(y);
		}

		std::string name = "log_1";

		UID curveID = m_modelComponent->addResult1DEntity(m_modelComponent->grabNextReservedEntityUID(), m_modelComponent->grabNextReservedEntityUID(),
			resultRawBase + name, xdata, ydataRe, ydataIm, "Frequency", "GHz", "", "dB", 1);

		curvesList.push_back(std::pair<UID, std::string>(curveID, name));

		entityIDList.push_back(curveID);
		entityVersionList.push_back(1);
		entityParentList.push_back(solverEntity->getEntityID());

		UID plotID = m_modelComponent->addPlot1DEntity(m_modelComponent->grabNextReservedEntityUID(), result1DBase + "Logscale", "logscale test curves", curvesList);
		entityIDList.push_back(plotID);
		entityVersionList.push_back(1);
		entityParentList.push_back(0);

	}

	{
		std::list<std::pair<UID, std::string>> curvesList;

		m_modelComponent->reserveNewEntityUIDs(2 + 1);

		std::vector<double> xdata, ydataRe, ydataIm;
		xdata.reserve(1000);
		ydataRe.reserve(1000);
		ydataIm.reserve(1000);

		for (int i = 0; i < 1000; i++)
		{
			double x = 2 * 3.141592654 / 1000 * i;
			double yre = sin(x);
			double yim = cos(x);

			xdata.push_back(x);
			ydataRe.push_back(yre);
			ydataIm.push_back(yim);
		}

		std::string name = "complex_1";

		UID curveID = m_modelComponent->addResult1DEntity(m_modelComponent->grabNextReservedEntityUID(), m_modelComponent->grabNextReservedEntityUID(),
			resultRawBase + name, xdata, ydataRe, ydataIm, "Frequency", "GHz", "", "dB", 1);

		curvesList.push_back(std::pair<UID, std::string>(curveID, name));

		entityIDList.push_back(curveID);
		entityVersionList.push_back(1);
		entityParentList.push_back(solverEntity->getEntityID());

		UID plotID = m_modelComponent->addPlot1DEntity(m_modelComponent->grabNextReservedEntityUID(), result1DBase + "Complex", "Complex test curves", curvesList);
		entityIDList.push_back(plotID);
		entityVersionList.push_back(1);
		entityParentList.push_back(0);
	}

	*/
	// REMOVE UNTIL HERE

	// Store the newly created items in the data base
	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added solver results");
}

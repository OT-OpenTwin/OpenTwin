/*********************************************************************
 * @file   Application.cpp
 * @brief  Handles service representation in UI.
 * 
 * @author Jan Wagner
 * @date   August 2022
 *********************************************************************/

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/EntityInformation.h"

// Application specific includes
#include "EntitySolverFITTD.h"
#include "EntitySolverMonitor.h"
#include "EntitySolverPort.h"
#include "EntitySignalType.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "FolderNames.h"


#include "ClassFactory.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_FITTD

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_FITTD

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
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonCreateSolver.GetFullDescription())
		{
			addSolver();
		}
		else if (action == _buttonRunSolver.GetFullDescription())
		{
			runSolver();
		}
		else if (action == _buttonAddPort.GetFullDescription())
		{
			addPort();
		}
		else if (action == _buttonAddMonitor.GetFullDescription())
		{
			addMonitor();
		}
		else if (action == _buttonCreateSignal.GetFullDescription())
		{
			addSignalType();
		}
		else assert(0); // Unhandled button action
	}
	else {
		return OT_ACTION_RETURN_UnknownAction;
	}
	return "";
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	const std::string pageName = "FIT-TD";
	
	_ui->addMenuPage(pageName);

	const std::string groupNameSolver = "Solver";
	const std::string groupNamePort = "Port";
	const std::string groupNameMonitor = "Monitor";
	const std::string groupNameSignal = "Signal Type";

	_ui->addMenuGroup(pageName, groupNameSolver);
	_ui->addMenuGroup(pageName, groupNamePort);
	_ui->addMenuGroup(pageName, groupNameMonitor);
	_ui->addMenuGroup(pageName, groupNameSignal);

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_buttonCreateSolver.SetDescription(pageName, groupNameSolver, "Create Solver");
	_buttonRunSolver.SetDescription(pageName, groupNameSolver, "Run Solver");
	_buttonAddPort.SetDescription(pageName, groupNamePort, "Add Port");
	_buttonAddMonitor.SetDescription(pageName, groupNameMonitor, "Add Monitor");
	_buttonCreateSignal.SetDescription(pageName, groupNameSignal, "Create Signal");

	_ui->addMenuButton( _buttonRunSolver, modelWrite, "RunSolver");
	_ui->addMenuButton(_buttonCreateSolver, modelWrite, "AddSolver");
	_ui->addMenuButton( _buttonAddPort, modelWrite, "Port");
	_ui->addMenuButton( _buttonAddMonitor, modelWrite, "Monitor");
	_ui->addMenuButton(_buttonCreateSignal, modelWrite, "Signal");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged(void)
{
	uiComponent()->setControlState(_buttonRunSolver.GetFullDescription(), (m_selectedEntities.size() > 0));

	uiComponent()->setControlState(_buttonCreateSignal.GetFullDescription(), (m_selectedEntities.size() == 1));
	uiComponent()->setControlState(_buttonAddMonitor.GetFullDescription(), (m_selectedEntities.size() == 1));
	uiComponent()->setControlState(_buttonAddPort.GetFullDescription(), (m_selectedEntities.size() == 1));
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

void Application::addMonitor(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(m_selectedEntities.size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(m_modelComponent != nullptr);
	m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getType() == "EntitySolverFITTD");
	
	auto solverEntity = dynamic_cast<EntitySolverFITTD*>( m_modelComponent->readEntityFromEntityIDandVersion(selectedSolver->getID(), selectedSolver->getVersion(), getClassFactory()));
	assert(solverEntity != nullptr);
	
	//Create new monitor
	ot::UID newMonitorID = m_modelComponent->createEntityUID();
	auto newMonitor = new EntitySolverMonitor(newMonitorID, nullptr, nullptr, nullptr, nullptr, "Model");

	// Get a list of all items of this specific solver
	std::string solverName = selectedSolver->getName();
	std::string fullMonitorPath = solverName + "/" + FolderNames::GetFolderNameMonitors();
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems(fullMonitorPath);

	std::string monitorName;
	int count(0);
	do
	{
		monitorName = fullMonitorPath + "/Monitor" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), monitorName) != solverItems.end());

	newMonitor->setName(monitorName);
	newMonitor->setInitiallyHidden(false);
	newMonitor->createProperties();
	newMonitor->StoreToDataBase();

	// Register the new solver item in the model

	std::list<ot::UID> topologyEntityIDList = { newMonitor->getEntityID()};
	std::list<ot::UID> topologyEntityVersionList = { newMonitor->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new monitor");
}

void Application::addPort(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(m_selectedEntities.size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(m_modelComponent != nullptr);
	m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getType() == "EntitySolverFITTD");

	auto solverEntity = dynamic_cast<EntitySolverFITTD*>(m_modelComponent->readEntityFromEntityIDandVersion(selectedSolver->getID(), selectedSolver->getVersion(), getClassFactory()));
	assert(solverEntity != nullptr);

	//Create new monitor
	ot::UID newPortID = m_modelComponent->createEntityUID();
	auto newPort = new EntitySolverPort(newPortID, nullptr, nullptr, nullptr, nullptr, "Model");

	// Find the next free numbered entity name
	std::string solverName = selectedSolver->getName();
	std::string fullPortPath = solverName + "/" + FolderNames::GetFolderNamePorts();
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems(fullPortPath);

	std::string portName;
	int count(0);
	do
	{
		portName = fullPortPath + "/Port" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), portName) != solverItems.end());

	newPort->setName(portName);
	newPort->setInitiallyHidden(false);

	std::string signalTypeFolderName = solverName + "/" + FolderNames::GetFolderNameSignalType();
	ot::EntityInformation entityInfos;
	m_modelComponent->getEntityInformation(signalTypeFolderName, entityInfos);
	std::list<ot::EntityInformation> childInfos;
	m_modelComponent->getEntityChildInformation(entityInfos.getID(), childInfos, false);
	std::string firstSignalName;
	ot::UID firstSignalID;
	childInfos.size() != 0 ? firstSignalName = childInfos.front().getName() : firstSignalName = "";
	childInfos.size() != 0 ? firstSignalID = childInfos.front().getID() : firstSignalID = 0;


	newPort->createProperties(signalTypeFolderName, entityInfos.getVersion(), firstSignalName, firstSignalID);
	newPort->StoreToDataBase();

	// Register the new solver item in the model

	std::list<ot::UID> topologyEntityIDList = { newPort->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { newPort->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new monitor");
}

void Application::addSignalType(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(m_selectedEntities.size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(m_modelComponent != nullptr);
	m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getType() == "EntitySolverFITTD");

	auto solverEntity = dynamic_cast<EntitySolverFITTD*>(m_modelComponent->readEntityFromEntityIDandVersion(selectedSolver->getID(), selectedSolver->getVersion(), getClassFactory()));
	assert(solverEntity != nullptr);

	// Get a list of all items of this specific solver
	const std::string folderName = FolderNames::GetFolderNameSignalType();
	std::string solverName = selectedSolver->getName();
	const std::string signalTypeFolderPath = solverName + "/" + folderName;
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems(signalTypeFolderPath);
	std::string signalName;
	int count(0);
	do
	{
		signalName = signalTypeFolderPath + "/Signal" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), signalName) != solverItems.end());


	//Create new Signal Type
	ot::UID newSignalID = m_modelComponent->createEntityUID();
	auto newSignal = new EntitySignalType(newSignalID, nullptr, nullptr, nullptr, nullptr, "Model");
	
	//Set properties
	newSignal->setName(signalName);
	newSignal->setInitiallyHidden(false);
	newSignal->createProperties();
	newSignal->StoreToDataBase();

	//Register the new solver item in the model
	std::list<ot::UID> topologyEntityIDList = { newSignal->getEntityID()};
	std::list<ot::UID> topologyEntityVersionList = { newSignal->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new signal type");

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
	std::string solverFolder = FolderNames::GetFolderNameSolver();
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems(solverFolder);

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = solverFolder + "/FIT-TD" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	m_modelComponent->getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverFITTD *solverEntity = new EntitySolverFITTD(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);
	solverEntity->createProperties(meshFolderName, meshFolderID, meshName, meshID);	

	solverEntity->StoreToDataBase();

	//Create default signal
	ot::UID newSignalID = m_modelComponent->createEntityUID();
	auto newSignal = new EntitySignalType(newSignalID, nullptr, nullptr, nullptr, nullptr, "Model");
	newSignal->setName(solverName + "/" + FolderNames::GetFolderNameSignalType() + "/DefaultSignal");
	newSignal->setInitiallyHidden(false);
	newSignal->setEditable(false);
	newSignal->createProperties();

	newSignal->StoreToDataBase();

	// Register the new solver item in the model
	std::list<ot::UID> topologyEntityIDList = { solverEntity->getEntityID(), newSignal->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { solverEntity->getEntityStorageVersion(), newSignal->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false, false};
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

	if (m_selectedEntities.empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// We first get a list of all selected entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfo);

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getType() == "EntitySolverFITTD")
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

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::SolverThread,this, solverRunList);
	workerThread.detach();
}

void Application::SolverThread(std::list<std::string> solverRunList)
{
	// Lock the UI
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockNavigationWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockProperties);

	m_uiComponent->lockUI(lockFlags);
	
	for (std::string solverName : solverRunList)
	{
		try
		{
			m_uiComponent->displayMessage("Start solver: "+ solverName +"\n");
			MicroServiceInterfaceFITTDSolver newSolver(solverName, getServiceIDAsInt(), getSessionCount(), getClassFactory());
			SetupSolverService(newSolver);
			newSolver.RemoveOldResults();
			newSolver.CreateSolver();
			newSolver.Run();
			m_uiComponent->displayMessage(solverName + ": run finished.\n\n");
		}
		catch (std::exception& e)
		{
			std::string errorMessage = "Exception in " + solverName + ": " + std::string(e.what());
			m_uiComponent->displayMessage(errorMessage);
		}
	}

	// Release the UI
	m_uiComponent->unlockUI(lockFlags);
}

void Application::SetupSolverService(MicroServiceInterfaceFITTDSolver & newSolver)
{
	// Set some general properties
	newSolver.setDataBaseURL(DataBase::GetDataBase()->getDataBaseServerURL());
	newSolver.setProjectName(DataBase::GetDataBase()->getProjectName());
	newSolver.setUIComponent(m_uiComponent);
	newSolver.setModelComponent(m_modelComponent);
}



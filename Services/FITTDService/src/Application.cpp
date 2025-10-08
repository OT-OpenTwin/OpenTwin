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
#include "EntityInformation.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

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

Application& Application::instance(void)
{
	static Application g_instance;
	return g_instance;
}

// ##################################################################################################################################

// Required functions

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

	// Setup buttons
	_buttonRunSolver = ot::ToolBarButtonCfg(pageName, groupNameSolver, "Run Solver", "Default/RunSolver");
	_buttonRunSolver.setButtonLockFlags(modelWrite);

	_buttonCreateSolver = ot::ToolBarButtonCfg(pageName, groupNameSolver, "Create Solver", "Default/AddSolver");
	_buttonCreateSolver.setButtonLockFlags(modelWrite);

	_buttonAddPort = ot::ToolBarButtonCfg(pageName, groupNamePort, "Add Port", "Default/Port");
	_buttonAddPort.setButtonLockFlags(modelWrite);

	_buttonAddMonitor = ot::ToolBarButtonCfg(pageName, groupNameMonitor, "Add Monitor", "Default/Monitor");
	_buttonAddMonitor.setButtonLockFlags(modelWrite);

	_buttonCreateSignal = ot::ToolBarButtonCfg(pageName, groupNameSignal, "Create Signal", "Default/Signal");
	_buttonCreateSignal.setButtonLockFlags(modelWrite);

	// Add buttons to UI
	_ui->addMenuButton(_buttonRunSolver);
	_ui->addMenuButton(_buttonCreateSolver);
	_ui->addMenuButton(_buttonAddPort);
	_ui->addMenuButton(_buttonAddMonitor);
	_ui->addMenuButton(_buttonCreateSignal);

	// Connect button handlers
	connectButton(this, &Application::addSolver, _buttonCreateSolver);
	connectButton(this, &Application::runSolver, _buttonRunSolver);
	connectButton(this, &Application::addPort, _buttonAddPort);
	connectButton(this, &Application::addMonitor, _buttonAddMonitor);
	connectButton(this, &Application::addSignalType, _buttonCreateSignal);

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged(void)
{
	getUiComponent()->setControlState(_buttonRunSolver.getFullPath(), (this->getSelectedEntities().size() > 0));

	getUiComponent()->setControlState(_buttonCreateSignal.getFullPath(), (this->getSelectedEntities().size() == 1));
	getUiComponent()->setControlState(_buttonAddMonitor.getFullPath(), (this->getSelectedEntities().size() == 1));
	getUiComponent()->setControlState(_buttonAddPort.getFullPath(), (this->getSelectedEntities().size() == 1));
}

void Application::addMonitor(void)
{
	if (this->getModelComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(this->getSelectedEntities().size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(this->getModelComponent() != nullptr);
	ot::ModelServiceAPI::getEntityInformation(this->getSelectedEntities(), selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getEntityType() == "EntitySolverFITTD");
	
	auto solverEntity = dynamic_cast<EntitySolverFITTD*>( ot::EntityAPI::readEntityFromEntityIDandVersion(selectedSolver->getEntityID(), selectedSolver->getEntityVersion(), getClassFactory()));
	assert(solverEntity != nullptr);
	
	//Create new monitor
	ot::UID newMonitorID = this->getModelComponent()->createEntityUID();
	auto newMonitor = new EntitySolverMonitor(newMonitorID, nullptr, nullptr, nullptr, nullptr, "Model");

	// Get a list of all items of this specific solver
	std::string solverName = selectedSolver->getEntityName();
	std::string fullMonitorPath = solverName + "/" + FolderNames::GetFolderNameMonitors();
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems(fullMonitorPath);

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

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new monitor");
}

void Application::addPort(void) {
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(this->getSelectedEntities().size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(this->getModelComponent() != nullptr);
	ot::ModelServiceAPI::getEntityInformation(this->getSelectedEntities(), selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getEntityType() == "EntitySolverFITTD");

	auto solverEntity = dynamic_cast<EntitySolverFITTD*>(ot::EntityAPI::readEntityFromEntityIDandVersion(selectedSolver->getEntityID(), selectedSolver->getEntityVersion(), getClassFactory()));
	assert(solverEntity != nullptr);

	//Create new monitor
	ot::UID newPortID = this->getModelComponent()->createEntityUID();
	auto newPort = new EntitySolverPort(newPortID, nullptr, nullptr, nullptr, nullptr, "Model");

	// Find the next free numbered entity name
	std::string solverName = selectedSolver->getEntityName();
	std::string fullPortPath = solverName + "/" + FolderNames::GetFolderNamePorts();
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems(fullPortPath);

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
	ot::ModelServiceAPI::getEntityInformation(signalTypeFolderName, entityInfos);
	std::list<ot::EntityInformation> childInfos;
	ot::ModelServiceAPI::getEntityChildInformation(entityInfos.getEntityID(), childInfos, false);
	std::string firstSignalName;
	ot::UID firstSignalID;
	childInfos.size() != 0 ? firstSignalName = childInfos.front().getEntityName() : firstSignalName = "";
	childInfos.size() != 0 ? firstSignalID = childInfos.front().getEntityID() : firstSignalID = 0;


	newPort->createProperties(signalTypeFolderName, entityInfos.getEntityVersion(), firstSignalName, firstSignalID);
	newPort->StoreToDataBase();

	// Register the new solver item in the model

	std::list<ot::UID> topologyEntityIDList = { newPort->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { newPort->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new monitor");
}

void Application::addSignalType(void) {
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	assert(this->getSelectedEntities().size() == 1);
	//Get the selected solver
	std::list<ot::EntityInformation> selectedEntityInfo;
	assert(this->getModelComponent() != nullptr);
	ot::ModelServiceAPI::getEntityInformation(this->getSelectedEntities(), selectedEntityInfo);
	auto selectedSolver = selectedEntityInfo.begin();
	assert(selectedSolver->getEntityType() == "EntitySolverFITTD");

	auto solverEntity = dynamic_cast<EntitySolverFITTD*>(ot::EntityAPI::readEntityFromEntityIDandVersion(selectedSolver->getEntityID(), selectedSolver->getEntityVersion(), getClassFactory()));
	assert(solverEntity != nullptr);

	// Get a list of all items of this specific solver
	const std::string folderName = FolderNames::GetFolderNameSignalType();
	std::string solverName = selectedSolver->getEntityName();
	const std::string signalTypeFolderPath = solverName + "/" + folderName;
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems(signalTypeFolderPath);
	std::string signalName;
	int count(0);
	do
	{
		signalName = signalTypeFolderPath + "/Signal" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), signalName) != solverItems.end());


	//Create new Signal Type
	ot::UID newSignalID = this->getModelComponent()->createEntityUID();
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

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new signal type");

}

void Application::addSolver(void) {
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::string solverFolder = FolderNames::GetFolderNameSolver();
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems(solverFolder);

	// Now get a new entity ID for creating the new item
	ot::UID entityID = this->getModelComponent()->createEntityUID();

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

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverFITTD *solverEntity = new EntitySolverFITTD(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);
	solverEntity->createProperties(meshFolderName, meshFolderID, meshName, meshID);	

	solverEntity->StoreToDataBase();

	//Create default signal
	ot::UID newSignalID = this->getModelComponent()->createEntityUID();
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

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create solver");
}

void Application::runSolver(void) {
	if (this->getSelectedEntities().empty())
	{
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// We first get a list of all selected entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (this->getModelComponent() == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	ot::ModelServiceAPI::getEntityInformation(this->getSelectedEntities(), selectedEntityInfo);

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getEntityType() == "EntitySolverFITTD")
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

	this->getUiComponent()->lockUI(lockFlags);
	
	for (std::string solverName : solverRunList)
	{
		try
		{
			this->getUiComponent()->displayMessage("Start solver: "+ solverName +"\n");
			MicroServiceInterfaceFITTDSolver newSolver(solverName, static_cast<int>(getServiceID()), getSessionCount(), getClassFactory());
			SetupSolverService(newSolver);
			newSolver.RemoveOldResults();
			newSolver.CreateSolver();
			newSolver.Run();
			this->getUiComponent()->displayMessage(solverName + ": run finished.\n\n");
		}
		catch (std::exception& e)
		{
			std::string errorMessage = "Exception in " + solverName + ": " + std::string(e.what());
			this->getUiComponent()->displayMessage(errorMessage);
		}
	}

	// Release the UI
	this->getUiComponent()->unlockUI(lockFlags);
}

void Application::SetupSolverService(MicroServiceInterfaceFITTDSolver & newSolver)
{
	// Set some general properties
	newSolver.setDataBaseURL(DataBase::GetDataBase()->getDataBaseServerURL());
	newSolver.setProjectName(DataBase::GetDataBase()->getProjectName());
	newSolver.setUIComponent(this->getUiComponent());
	newSolver.setModelComponent(this->getModelComponent());
}



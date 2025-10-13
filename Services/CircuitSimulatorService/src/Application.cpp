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
#include "QtWrapper.h"
#include "SimulationResults.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTCommunication/ActionTypes.h"
#include "EntitySolverCircuitSimulator.h"
#include "EntityBlockConnection.h"
#include "DataBase.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityGraphicsScene.h"
#include "OTCore/EntityName.h"
#include "Helper.h"

// Third Party Header
#include <ngspice/sharedspice.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <string.h>

Application * g_instance{ nullptr };

#undef GetObject

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService, new UiNotifier(), new ModelNotifier()) {
	ClassFactory& classFactory = getClassFactory();
	ClassFactoryBlock* classFactoryBlock = new ClassFactoryBlock();
	classFactoryBlock->setChainRoot(&classFactory);
	classFactory.setNextHandler(classFactoryBlock);
	m_SimulationRunning = false;
	
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, this, &Application::handleNewGraphicsItem);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, this, &Application::handleRemoveGraphicsItem);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &Application::handleNewGraphicsItemConnection);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection, this, &Application::handleRemoveGraphicsItemConnection);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, this, &Application::handleConnectionToConnection);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, this, &Application::handleConnectionChanged);
}

Application::~Application() {

}

// ##################################################################################################################################################################################################################

// Trying to create more circuits

void Application::createNewCircuit() {
	
	std::list<std::string> existingCircuits = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::CircuitsFolder);
	std::string circuitName = ot::EntityName::createUniqueEntityName(ot::FolderNames::CircuitsFolder, "Circuit", existingCircuits);

	std::unique_ptr<EntityGraphicsScene> entityCircuitRoot(new EntityGraphicsScene(this->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	entityCircuitRoot->setEditable(true);
	entityCircuitRoot->setName(circuitName);

	entityCircuitRoot->StoreToDataBase();
	ot::ModelServiceAPI::addEntitiesToModel({ entityCircuitRoot->getEntityID() }, { entityCircuitRoot->getEntityStorageVersion() }, { false }, {}, {}, {}, "Added new circuit");
	
	Circuit circuit;
	circuit.setEditorName(circuitName);
	circuit.setId(circuitName);
	Application::instance()->getNGSpice().getMapOfCircuits().insert_or_assign(circuitName, circuit);
}

void Application::createInitialCircuit() {
	//// We check first if we already added the voltage source to the circuit as initial element
	//std::list<std::string> initialCircuitElements = ot::ModelServiceAPI::getListOfFolderItems("Circuits/Circuit 1");

	//if (initialCircuitElements.empty()) {
	//	// If not then we add it
	//	ot::Point2DD position = ot::Point2DD::Point2DD(5.5813953488372094, -14.883720930232558);
	//	auto temp = m_blockEntityHandler.CreateBlockEntity(m_blockEntityHandler.getInitialCircuitName(), "EntityBlockCircuitVoltageSource", position);
	//}

	//// Else we do nothing
	/*std::this_thread::sleep_for(std::chrono::seconds(3));*/

	std::list<std::string> circuits = ot::ModelServiceAPI::getListOfFolderItems("Circuits");
	if (circuits.empty()) {
	
		EntityContainer* entityCircuitRoot = new EntityContainer(this->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		entityCircuitRoot->setName(getCircuitRootName() + m_blockEntityHandler.getInitialCircuitName());

		entityCircuitRoot->StoreToDataBase();
		ot::ModelServiceAPI::addEntitiesToModel({ entityCircuitRoot->getEntityID() }, { entityCircuitRoot->getEntityStorageVersion() }, { false }, {}, {}, {}, "Added FolderEntity");
	}
	
}

void Application::modelSelectionChanged() {
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (this->getSelectedEntities().size() > 0) {
			enabled.push_back("Circuit Simulator:Simulate:Run Simulation");
		}
		else {
			disabled.push_back("Circuit Simulator:Simulate:Run Simulation");
		}

		this->getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

void Application::addSolver() {
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	//First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems("Solvers");

	// Get new Entity Id for the new item
	ot::UID entityID = this->getModelComponent()->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do {
		solverName = "Solvers/CircuitSimulator" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available circuits 
	std::string circuitFolderName, circuitName;
	ot::UID circuitFolderID{ 0 }, circuitID{ 0 };

	// Create the new solver item and store it in the data base
	EntitySolverCircuitSimulator* solverEntity = new EntitySolverCircuitSimulator(entityID, nullptr, nullptr, nullptr, nullptr, getServiceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);

	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation("Circuits", entityInfo);

	solverEntity->createProperties("Circuits", entityInfo.getEntityID(), circuitName, circuitID);
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

void Application::runCircuitSimulation() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (this->getSelectedEntities().empty()) {
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	//Here we first need to check which solvers are selected and then run them one by on
	std::map<std::string, bool> solverRunMap;
	for (auto& entity : this->getSelectedEntityInfos()) {
		if (entity.getEntityType() == "EntitySolverCircuitSimulator" || entity.getEntityType() == "EntitySolver") {
			if (entity.getEntityName().substr(0, 8) == "Solvers/") {
				size_t index = entity.getEntityName().find('/', 8);
				if (index != std::string::npos) {
					solverRunMap[entity.getEntityName().substr(0, index - 1)] = true;
				}
				else {
					solverRunMap[entity.getEntityName()] = true;
				}
			}
		}
	}

	std::list<std::string> solverRunList;
	for (auto solver : solverRunMap) {
		solverRunList.push_back(solver.first);
	}

	if (solverRunList.empty()) {
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	//Now we retrieve information about the solver items
	ot::ModelServiceAPI::getEntityInformation(solverRunList, m_solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : m_solverInfo) {
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsSolver);

	//Now read the solver objects for each solver
	for (auto info : m_solverInfo) {
		EntityBase* entity = ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion(), getClassFactory());
		m_solverMap[info.getEntityName()] = entity;
	}

	// Get the current model version
	m_modelVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	if (!m_solverInfo.empty()) {
		auto solver = m_solverInfo.front();
		m_solverInfo.pop_front();
		runSingleSolver(solver, m_modelVersion, m_solverMap[solver.getEntityName()]);
		m_solverMap.erase(solver.getEntityName());
	}
}

void Application::runNextSolvers() {
	// run next solver

	if (!m_solverInfo.empty()) {
		auto solver = m_solverInfo.front();
		m_solverInfo.pop_front();
		runSingleSolver(solver, m_modelVersion, m_solverMap[solver.getEntityName()]);
		m_solverMap.erase(solver.getEntityName());
	}

	this->getUiComponent()->displayMessage("No next solver, simulation completed");

	return;
}

void Application::runSingleSolver(ot::EntityInformation& solver, std::string& modelVersion, EntityBase* solverEntity) {
	

	this->getUiComponent()->lockUI(ot::LockTypeFlag::LockModelWrite);
	m_SimulationRunning = true;

	// Enusre that subprocessHandler is null before starting
	m_subprocessHandler = nullptr;
	
	// Get circuit name
	EntityPropertiesEntityList* circuitName = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Circuit"));
	assert(circuitName != nullptr);

	// Get simulation type
	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);

	
	this->getUiComponent()->setProgressInformation("Simulating", false);
	this->getUiComponent()->setProgress(0);


	// Get only circuit name
	std::string name =  extractStringAfterDelimiter(circuitName->getValueName(), '/', 1);

	// Get solver name
	std::string solverName = solver.getEntityName();

	if (name == "failed")
	{
		OT_LOG_E("No Circuit found or selected!");
		finishFailedSimulation();
		m_SimulationRunning = false;
		return;
	}

	// Setting the members of SimulationResults
	SimulationResults::getInstance()->setSolverInformation(solverName, simulationTypeProperty->getValue(), circuitName->getValueName());

	// Set circuit
	m_blockEntityHandler.setPackageName(name);

	// Get all Entities

	auto allEntitiesByBlockID = m_blockEntityHandler.findAllBlockEntitiesByBlockID(circuitName->getValueName());
	auto allConnectionEntitiesByID = m_blockEntityHandler.findAllEntityBlockConnections(circuitName->getValueName());

	if (allEntitiesByBlockID.empty()) {
		OT_LOG_E("The " + name + " is empty!");
		finishFailedSimulation();
		return;
	}

	if (allConnectionEntitiesByID.empty()) {
		OT_LOG_E("No connections found in " + name);
		finishFailedSimulation();
		return;
	}

	// Now we get the blockConnectionMap from helper in BlockEntities
	std::map<ot::UID, ot::UIDList> connectionBlockMap = Helper::buildMap(allConnectionEntitiesByID,allEntitiesByBlockID);
	if (connectionBlockMap.empty()) {
		OT_LOG_E("No valid connections found in " + name);
		finishFailedSimulation();
		return;
	}
	// Here i generate my netlist

	std::list<std::string> _netlist = m_ngSpice.ngSpice_Initialize (connectionBlockMap,solverEntity,allConnectionEntitiesByID,allEntitiesByBlockID, name);
	if (_netlist.empty())
	{
		OT_LOG_E("NGSpice Initialize function failed!");
		m_ngSpice.clearBufferStructure(name);
		finishFailedSimulation();
		return;
	}

	// After generating the netlist I start the subprocess
	// Initialization time of subprocess
	QDateTime initTime = QDateTime::currentDateTime();
	SimulationResults::getInstance()->handleCircuitExecutionTiming(initTime, "startingInitTime");

	// Initialization of subprocess
	const int sessionCount = Application::instance()->getSessionCount();
	const int serviceID = static_cast<int>(Application::instance()->getServiceID());
	if (m_subprocessHandler == nullptr) {
		m_subprocessHandler = new SubprocessHandler(m_serverName, sessionCount, serviceID);
	}
	else {
		OT_LOG_E("Initialization of subprocess failed!");
		finishFailedSimulation();
	}

#ifndef _DEBUG
	// Starting subprocess
	if (m_subprocessHandler->RunSubprocess()) {
		OT_LOG_D("Sub process started successfull.");
	}
	else {
		OT_LOG_E("Sub process failed to start.");
		return;
	}
#endif // !_DEBUG



	// Now i set the netlist to the ConnectionManager and afterwards it is being send
	sendNetlistToSubService(_netlist);

	m_ngSpice.clearBufferStructure(name);
}

void Application::sendNetlistToSubService(std::list<std::string>& _netlist) {
	if (m_qtWrapper == nullptr) {
		OT_LOG_W("QtWrapper is nullptr");
	}

	m_qtWrapper->getConnectionManager()->setNetlist(ConnectionManager::ExecuteNetlist, _netlist);
	if (m_qtWrapper->getConnectionManager()->sendNetlist()) {
		OT_LOG_D("Netlist sent successfull");
		return;
	}
	else {
		OT_LOG_E("Failed to send netlist");
		return;
	}
	

}

std::string Application::extractStringAfterDelimiter(const std::string& inputString, char delimiter, size_t occurrence) {
	size_t pos = 0;
	size_t count = 0;

	
	while (count < occurrence && (pos = inputString.find(delimiter, pos)) != std::string::npos) {
		++count;
		++pos; 
	}

	if (count == occurrence) {
		
		return inputString.substr(pos);
	}
	else if(delimiter == '/') {
		return "failed";
	}
	else if(delimiter == '_') {
		return "first";
	}
	else
	{
		return "failed";
	}
}

void Application::finishSimulation() {
	std::lock_guard<std::mutex> lock(m_mutex);
	this->getUiComponent()->setProgress(100);
	this->getUiComponent()->closeProgressInformation();
	this->getUiComponent()->unlockUI(ot::LockTypeFlag::LockModelWrite);
	m_SimulationRunning = false;
	SimulationResults::getInstance()->storeLogDataInResultText();
	SimulationResults::getInstance()->clearUp();
	runNextSolvers();

}

void Application::finishFailedSimulation() {
	OT_LOG_E("Simulation Failed! Shutting down!");

	this->getUiComponent()->closeProgressInformation();
	this->getUiComponent()->unlockUI(ot::LockTypeFlag::LockModelWrite);
	m_SimulationRunning = false;
	SimulationResults::getInstance()->storeLogDataInResultText();
	SimulationResults::getInstance()->clearUp();
}

SubprocessHandler* Application::getSubProcessHandler() {
	return m_subprocessHandler;
}

void Application::handleNewGraphicsItem(ot::JsonDocument& _document) {
	//Here we get the Item Information
	std::string itemName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	ot::Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	//check and store information
	OT_LOG_D("Handling new graphics item request ( name = \"" + itemName + "\"; editor = \"" + editorName + "\"; x = " + std::to_string(pos.x()) + "; y = " + std::to_string(pos.y()) + " )");

	
	m_blockEntityHandler.setPackageName(editorName);
	auto temp = m_blockEntityHandler.CreateBlockEntity(editorName, itemName, pos);	
}

void Application::handleRemoveGraphicsItem(ot::JsonDocument& _document) {
	ot::UIDList items;

	// Add Item UIDs to the list above (Items to be removed)
	ot::UID itemUID = ot::json::getUInt64(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
	items.push_back(itemUID);
}

void Application::handleNewGraphicsItemConnection(ot::JsonDocument& _document) {
	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));
	m_blockEntityHandler.setPackageName(pckg.name());
	m_blockEntityHandler.addBlockConnection(pckg.connections(),pckg.name());
}

ot::ReturnMessage Application::handleRemoveGraphicsItemConnection(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	// Here we would check and remove the connection information
	OT_LOG_D("Handling remove graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to remove connections
	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(editorName, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	
	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc.GetAllocator());

	std::string tmp;
	if (this->getUiComponent()->sendMessage(true, reqDoc, tmp)) {
		return ot::ReturnMessage::Ok;
	}
	else {
		return ot::ReturnMessage::Failed;
	}
}

void Application::handleConnectionToConnection(ot::JsonDocument& _document) {

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	std::string editorName = pckg.name();
	
	ot::Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_POSITION));
	m_blockEntityHandler.setPackageName(editorName);
	m_blockEntityHandler.addConnectionToConnection(pckg.connections(), editorName, pos);
}

void Application::handleConnectionChanged(ot::JsonDocument& _document) {
	ot::GraphicsConnectionCfg connection;
	connection.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
}

// ############################## ####################################################################################################################################################################################

// Required functions

void Application::initialize() {
#ifdef _DEBUG
	m_serverName = "TestServerCircuit";
#else
	// Encode project name to base64 to avoid issues with special characters
	const std::string hexString = ot::String::toBase64Url(getSessionID());
	m_serverName = OT_INFO_SERVICE_TYPE_CircuitSimulatorService "_" + hexString;
#endif // _DEBUG

	m_qtWrapper = new QtWrapper();
	m_qtWrapper->run(m_serverName);
}

void Application::uiConnected(ot::components::UiComponent * _ui) {
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	const std::string page = "Circuit Simulator";

	const std::string groupEdit = "Edit";
	const std::string groupSimulate = "Simulate";

	_ui->addMenuPage(page);
	_ui->addMenuGroup(page, groupEdit);
	_ui->addMenuGroup(page, groupSimulate);

	auto flags = ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite;

	m_buttonAddSolver = ot::ToolBarButtonCfg(page, groupEdit, "Add Solver", "Default/Add");
	_ui->addMenuButton(m_buttonAddSolver.setButtonLockFlags(flags));
	connectToolBarButton(m_buttonAddSolver, this, &Application::addSolver);

	m_buttonAddCircuit = ot::ToolBarButtonCfg(page, groupEdit, "Add Circuit", "Default/Add");
	_ui->addMenuButton(m_buttonAddCircuit.setButtonLockFlags(flags));
	connectToolBarButton(m_buttonAddCircuit, this, &Application::createNewCircuit);

	m_buttonRunSimulation = ot::ToolBarButtonCfg(page, groupSimulate, "Run Simulation", "Default/RunSolver");
	_ui->addMenuButton(m_buttonRunSimulation.setButtonLockFlags(flags));
	connectToolBarButton(m_buttonRunSimulation, this, &Application::runCircuitSimulation);

	m_blockEntityHandler.setUIComponent(_ui);
	
	m_blockEntityHandler.OrderUIToCreateBlockPicker();
	m_blockEntityHandler.setPackageName("Circuit 1");
	//createInitialCircuit();
	SimulationResults::getInstance()->setUIComponent(_ui);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::modelConnected(ot::components::ModelComponent * _model) {
	m_blockEntityHandler.setModelComponent(_model);
	SimulationResults::getInstance()->setModelComponent(_model);
}

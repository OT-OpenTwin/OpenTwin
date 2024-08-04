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
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/Msg.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
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

// Third Party Header
#include <ngspice/sharedspice.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <string.h>

Application * g_instance{ nullptr };

#define EXAMPLE_NAME_BLOCK1 "Resistor"
#define EXAMPLE_NAME_Block2 "VoltageSource"
#define EXAMPLE_NAME_Block3 "Diode"
#define EXAMPLE_NAME_BLOCK4 "Transistor"
#define EXAMPLE_NAME_BLOCK5 "Connector"
#undef GetObject

namespace ottest {
	static unsigned long long currentBlockUid = 0;

	static unsigned long long currentResistorUid = 1;

	static unsigned long long currentNodeNumber = 0;

	static unsigned long long currentDiodeID = 1;

	static unsigned long long currentTransistoriD = 1;

	static unsigned long long currenConnectorID = 1;

	static unsigned long long currentEditorID = 1;
	
}


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
	classFactoryBlock->SetChainRoot(&classFactory);
	classFactory.SetNextHandler(classFactoryBlock);
}

Application::~Application() {

}

// ##################################################################################################################################################################################################################

// Custom functions
std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	


	if (action == "Circuit Simulator:Edit:Add Solver") {
		addSolver();
	}
	else if (action == "Circuit Simulator:Simulate:Run Simulation") {
		runCircuitSimulation();
		
	}
	else if (action == "Circuit Simulator:Edit:Add Circuit") {
		createNewCircuit();
	}
	else {
		//OT_LOG_W("Unknown model action");
		assert(0);
		}// Unhandled button action
	return std::string();
}

// Trying to create more circuits
void Application::createNewCircuit() {
	std::list<std::string> circuits = m_modelComponent->getListOfFolderItems("Circuits");

	int count = 1;
	std::string circuitName;
	do {
		circuitName = "Circuits/Circuit " + std::to_string(count);
		count++;
	} while (std::find(circuits.begin(), circuits.end(), circuitName) != circuits.end());

	//std::string circuitName = "Circuit " + std::to_string(ottest::currentEditorID);
	circuitName = extractStringAfterDelimiter(circuitName, '/', 1);
	
	ot::GraphicsNewEditorPackage* editor = new ot::GraphicsNewEditorPackage(circuitName, circuitName);
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	editor->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	Circuit circuit;
	circuit.setEditorName(editor->title());
	circuit.setId(editor->name());
	Application::instance()->getNGSpice().getMapOfCircuits().insert_or_assign(editor->name(), circuit);

	// Message is queued, no response here
	std::string tmp;
	m_uiComponent->sendMessage(true, doc, tmp);

	ottest::currentEditorID++;
}




std::string Application::handleModelSelectionChanged(ot::JsonDocument& _document) {
	selectedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	modelSelectionChangedNotification();
	return std::string();
}

void Application::modelSelectionChangedNotification(void) {
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (selectedEntities.size() > 0) {
			enabled.push_back("Circuit Simulator:Simulate:Run Simulation");
		}
		else {
			disabled.push_back("Circuit Simulator:Simulate:Run Simulation");
		}

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}
}

void Application::addSolver()
{
	if (!EnsureDataBaseConnection()) {
		assert(0); // Data base connection failed
		return;
	}

	if (m_uiComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	//First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems("Solvers");

	// Get new Entity Id for the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create a unique na,e for the new solver item
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
	EntitySolverCircuitSimulator* solverEntity = new EntitySolverCircuitSimulator(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);

	ot::EntityInformation entityInfo;
	m_modelComponent->getEntityInformation("Circuits", entityInfo);

	solverEntity->createProperties("Circuits", entityInfo.getID(), circuitName, circuitID);
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

void Application::runCircuitSimulation() {
	if (!EnsureDataBaseConnection()) {
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: Unable to connect to data base. \n");
		return;
	}

	if (selectedEntities.empty()) {
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	//First we get a list of all selected Entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	m_modelComponent->getEntityInformation(selectedEntities, selectedEntityInfo);

	//Here we first need to check which solvers are selected and then run them one by on
	std::map<std::string, bool> solverRunMap;
	for (auto entity : selectedEntityInfo) {
		if (entity.getType() == "EntitySolverCircuitSimulator" || entity.getType() == "EntitySolver") {
			if (entity.getName().substr(0, 8) == "Solvers/") {
				size_t index = entity.getName().find('/', 8);
				if (index != std::string::npos) {
					solverRunMap[entity.getName().substr(0, index - 1)] = true;
				}
				else {
					solverRunMap[entity.getName()] = true;
				}
			}
		}
	}

	std::list<std::string> solverRunList;
	for (auto solver : solverRunMap) {
		solverRunList.push_back(solver.first);
	}

	if (solverRunList.empty()) {
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		m_uiComponent->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	//Now we retrieve information about the solver items
	std::list<ot::EntityInformation> solverInfo;
	m_modelComponent->getEntityInformation(solverRunList, solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : solverInfo) {
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getID(), info.getVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdsSolver);

	//Now read the solver objects for each solver
	std::map<std::string, EntityBase*> solverMap;
	for (auto info : solverInfo) {
		EntityBase* entity = m_modelComponent->readEntityFromEntityIDandVersion(info.getID(), info.getVersion(), getClassFactory());
		solverMap[info.getName()] = entity;
	}

	// Get the current model version
	std::string modelVersion = m_modelComponent->getCurrentModelVersion();

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::solverThread, this, solverInfo, modelVersion, solverMap);
	workerThread.detach();
}

void Application::solverThread(std::list<ot::EntityInformation> solverInfo, std::string modelVersion, std::map<std::string, EntityBase*> solverMap) {
	for (auto solver : solverInfo) {
		runSingleSolver(solver, modelVersion, solverMap[solver.getName()]);
	}
}

void Application::runSingleSolver(ot::EntityInformation& solver, std::string& modelVersion, EntityBase* solverEntity) {
	EntityPropertiesEntityList* circuitName = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Circuit"));
	assert(circuitName != nullptr);

	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);

	

	std::string name =  extractStringAfterDelimiter(circuitName->getValueName(), '/', 1);
	std::string solverName = solver.getName();

	m_blockEntityHandler.setPackageName(name);
	auto allEntitiesByBlockID = m_blockEntityHandler.findAllBlockEntitiesByBlockID();
	auto allConnectionEntitiesByID = m_blockEntityHandler.findAllEntityBlockConnections();



	m_ngSpice.ngSpice_Initialize (solverEntity,allConnectionEntitiesByID,allEntitiesByBlockID, name);
	m_blockEntityHandler.createResultCurves(solverName,simulationTypeProperty->getValue(),circuitName->getValueName());
	m_ngSpice.clearBufferStructure(name);
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
		OT_LOG_E("Substring of FolderEntityName not Found!");
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

std::string Application::handleNewGraphicsItem(ot::JsonDocument& _document) {
	//Here we get the Item Information
	std::string itemName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	ot::Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	//check and store information
	OT_LOG_D("Handling new graphics item request ( name = \"" + itemName + "\"; editor = \"" + editorName + "\"; x = " + std::to_string(pos.x()) + "; y = " + std::to_string(pos.y()) + " )");

	/*CircuitElement element;
	element.setItemName(itemName);
	element.setEditorName(editorName);
	element.setUID(std::to_string(++ottest::currentBlockUid));
	if (element.getItemName() == "EntityBlockCircuitElement")
	{
		std::string voltageSource = "V1";
		element.setNetlistElementName(voltageSource);
	}
	else if (element.getItemName() == "Diode")
	{
		std::string temp = std::to_string(ottest::currentDiodeID++);
		std::string diode = "D";
		element.setNetlistElementName(diode + temp);
	}

	else if (element.getItemName() == "Transistor")
	{
		std::string temp = std::to_string(ottest::currentTransistoriD++);
		std::string tran = "Q";
		element.setNetlistElementName(tran + temp);
	}
	else if (element.getItemName() == "Connector")
	{
		std::string temp = std::to_string(ottest::currenConnectorID++);
		std::string conn = "C";
		element.setNetlistElementName(conn + temp);
	}
	else
	{
		std::string temp = std::to_string(ottest::currentTransistoriD++);
		std::string transistor = "Q";
		element.setNetlistElementName(transistor + temp);
	}

	auto it = m_ngSpice.mapOfCircuits.find(editorName);
	if (it == m_ngSpice.mapOfCircuits.end())
	{
		Application::instance()->uiComponent()->displayMessage("No Circuit found");
	}
	else
	{
		it->second.addElement(element.getUID(), element);
	}*/
	m_blockEntityHandler.setPackageName(editorName);
	m_blockEntityHandler.CreateBlockEntity(editorName, itemName, pos);
	return "";
	

	//// Create the Package
	//ot::GraphicsScenePackage pckg("Circuit");

	//// Create item configuration for the item to add
	//ot::GraphicsItemCfg* itm = nullptr;
	//if (itemName == EXAMPLE_NAME_BLOCK1) { itm = ottest::createResistor(EXAMPLE_NAME_BLOCK1); }
	//else if (itemName == EXAMPLE_NAME_Block2) { itm = ottest::createVoltageSource(EXAMPLE_NAME_Block2); }
	//else if (itemName == EXAMPLE_NAME_Block3) { itm = ottest::createDiode(EXAMPLE_NAME_Block3); }
	//else if (itemName == EXAMPLE_NAME_BLOCK4) { itm = ottest::createTransistor(EXAMPLE_NAME_BLOCK4); }
	//else if (itemName == EXAMPLE_NAME_BLOCK5) { itm = ottest::createConnector(EXAMPLE_NAME_BLOCK5); }
	//else
	//{
	//	m_uiComponent->displayMessage("[ERROR] Unknown item: " + itemName + "\n");
	//	return OT_ACTION_RETURN_VALUE_FAILED;
	//}
	//
	//itm->setPosition(pos);
	//itm->setUid(std::to_string(++ottest::currentBlockUid));
	//pckg.addItem(itm);

	//

	//// Here i Create a buffer Object for generating netlist
	//
	


	//ot::JsonDocument reqDoc;
	//reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	//ot::JsonObject pckgObj;
	//pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	//reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());
	//
	//this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc.GetAllocator());

	//m_uiComponent->sendMessage(true, reqDoc);

	//return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}


std::string Application::handleRemoveGraphicsItem(ot::JsonDocument& _document) {	
	ot::UIDList items;

	// Add Item UIDs to the list above (Items to be removed)
	ot::UID itemUID = ot::json::getUInt64(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
	items.push_back(itemUID);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}



std::string Application::handleNewGraphicsItemConnection(ot::JsonDocument& _document) {
	//std::string itemName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	//ot::Point2DD pos;
	//pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));
	m_blockEntityHandler.setPackageName(pckg.name());
	m_blockEntityHandler.AddBlockConnection(pckg.connections(),pckg.name());
	
	
	
	//for (auto c : pckg.connections())
	//{
	//	Connection connection(c);


	//	auto it = m_ngSpice.mapOfCircuits.find(pckg.name());

	//	//This i do because i have a connection item which is used as a bridge between two items and these connections need to have
	//	//the same nodeNumber

	//	if (it->second.findElement(connection.destUid()) == "Connector")
	//	{
	//		connection.setNodeNumber(std::to_string(0));
	//	}
	//	else if (it->second.findElement(connection.originUid()) == "Connector")
	//	{
	//		connection.setNodeNumber(std::to_string(0));
	//	}
	//	else
	//	{
	//		connection.setNodeNumber(std::to_string(ottest::currentNodeNumber++));
	//	}

	//	if (it == m_ngSpice.mapOfCircuits.end())
	//	{
	//		OT_LOG_E("Circuit not found { \"CircuitName\": \"" + pckg.name() + "\" }");
	//	}

	//	else
	//	{
	//		//Here I add the connection to the Origin Element
	//		it->second.addConnection(connection.originUid(), connection);




	//		//Some Tests
	//		//std::cout << "Size: " << it->second.getElement(connection.originUid()).getList().size() << std::endl;

	//		//Here I add the connection to the Destination Element
	//		it->second.addConnection(connection.destUid(), connection);

	//		//test of print



	//	}

	//}
	return "";
	
}

std::string Application::handleRemoveGraphicsItemConnection(ot::JsonDocument& _document) {
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
	if (m_uiComponent->sendMessage(true, reqDoc, tmp)) {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
	}
	else {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed);
	}
}

std::string Application::handleItemChanged(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	ot::GraphicsItemCfg* itemConfig = ot::GraphicsItemCfgFactory::instance().createFromJSON(ot::json::getObject(_document, OT_ACTION_PARAM_Config), OT_JSON_MEMBER_GraphicsItemCfgType);
	if (!itemConfig) return "";

	const ot::UID blockID = itemConfig->getUid();
	const ot::Transform transform = itemConfig->getTransform();
	m_blockEntityHandler.UpdateBlockPosition(blockID, itemConfig->getPosition(), transform, &getClassFactory());

	return "";
}

// ############################## ####################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (!EnsureDataBaseConnection())
	{
		assert(0);
	}
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument & _doc) {
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument & _doc) {
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui) {
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	_ui->addMenuPage("Circuit Simulator");
	_ui->addMenuGroup("Circuit Simulator", "Edit");
	_ui->addMenuGroup("Circuit Simulator", "Simulate");
	_ui->addMenuButton("Circuit Simulator", "Edit","Add Solver", "Add Solver", ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, "Add","Default");
	_ui->addMenuButton("Circuit Simulator","Simulate","Run Simulation","Run Simulation", ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, "Kriging", "Default");
	_ui->addMenuButton("Circuit Simulator", "Edit", "Add Circuit", "Add Circuit", ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, "Add", "Default");

	m_blockEntityHandler.setUIComponent(_ui);
	/*m_blockEntityHandler.setPackageName("Circuit"); */
	m_blockEntityHandler.OrderUIToCreateBlockPicker();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::uiDisconnected(const ot::components::UiComponent * _ui) {

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model) {
	m_blockEntityHandler.setModelComponent(_model);
	
}

void Application::modelDisconnected(const ot::components::ModelComponent * _model) {

}

void Application::serviceConnected(ot::ServiceBase * _service) {

}

void Application::serviceDisconnected(const ot::ServiceBase * _service) {

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void) {

}

bool Application::startAsRelayService(void) const {
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

// ##################################################################################################################################################################################################################

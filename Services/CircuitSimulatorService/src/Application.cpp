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
#include "OTGui/GraphicsCollectionCfg.h"
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
#include "OTCommunication/ActionTypes.h"


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

namespace ottest
{
	static unsigned long long currentBlockUid = 0;

	static unsigned long long currentResistorUid = 1;

	static unsigned long long currentNodeNumber = 0;

	static unsigned long long currentDiodeID = 1;

	static unsigned long long currentTransistoriD = 1;

	static unsigned long long currenConnectorID = 1;

	ot::GraphicsFlowItemConnector getDefaultConnectorStyle(void) {
		ot::GraphicsFlowItemConnector cfg;

		cfg.setTextColor(ot::Color(0,0,0,0));

		return cfg;
	}

	

	ot::GraphicsItemCfg* createDiode(const std::string _name)
	{
		ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
		myStack->setName(_name);
		myStack->setTitle(_name);
		myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);
		

		ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
		image->setImagePath("CircuitElementImages/Diod2.png");
		image->setSizePolicy(ot::SizePolicy::Dynamic);
		image->setMaintainAspectRatio(true);

		myStack->addItemBottom(image,false,true);

		ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();
		myLayout->setMinimumSize(ot::Size2DD(150.0, 150.0));

		myStack->addItemTop(myLayout,true,false);

		ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
		connection1->setName("Input3");
		ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		connection1->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		connection1->setBackgroundPainer(painter1);
		connection1->setAlignment(ot::AlignCenter);
		connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

		ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
		connection2->setName("Ouput3");
		ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		connection2->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		connection2->setBackgroundPainer(painter2);
		connection2->setAlignment(ot::AlignCenter);
		connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));
		
		connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

		myLayout->addChildItem(connection1);
		myLayout->addStrech(1);
		myLayout->addChildItem(connection2);
		
		
		return myStack;
	}

	ot::GraphicsItemCfg* createTransistor(const std::string _name)
	{
		ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
		myStack->setName(_name);
		myStack->setTitle(_name);
		myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

		ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
		image->setImagePath("CircuitElementImages/Transistor.png");
		image->setSizePolicy(ot::SizePolicy::Dynamic);
		image->setMaintainAspectRatio(true);

		myStack->addItemBottom(image, false, true);

		ot::GraphicsGridLayoutItemCfg* myLayout = new ot::GraphicsGridLayoutItemCfg(5,5);
		myLayout->setMinimumSize(ot::Size2DD(100.0, 100.0));
		myStack->addItemTop(myLayout,true,false);

		ot::GraphicsEllipseItemCfg* base = new ot::GraphicsEllipseItemCfg();
		base->setName("base");
		ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		base->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		base->setBackgroundPainer(painter1);
		base->setAlignment(ot::AlignLeft);
		base->setMaximumSize(ot::Size2DD(10.0, 10.0));
		base->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		base->setMargins(2.9, 0.0, 0.0, 0.0);

		ot::GraphicsEllipseItemCfg* collector = new ot::GraphicsEllipseItemCfg();
		collector->setName("collector");
		ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		collector->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		collector->setBackgroundPainer(painter2);
		collector->setAlignment(ot::AlignTop);
		collector->setMaximumSize(ot::Size2DD(10.0, 10.0));
		collector->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		collector->setMargins(0.0, 0.0, 0.0, 30.0);

		ot::GraphicsEllipseItemCfg* emitter = new ot::GraphicsEllipseItemCfg();
		emitter->setName("emitter");
		ot::FillPainter2D* painter3 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		emitter->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		emitter->setBackgroundPainer(painter3);
		emitter->setAlignment(ot::AlignBottom);
		emitter->setMaximumSize(ot::Size2DD(10.0, 10.0));
		emitter->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		emitter->setMargins(0.0, 0.0, 0.0, 30.0);

		myLayout->addChildItem(2,0,base);
		myLayout->addChildItem(0, 4, collector);
		myLayout->addChildItem(4, 4, emitter);

		myLayout->setColumnStretch(4, 1);
		
		

		return myStack;

	}

	ot::GraphicsItemCfg* createConnector(const std::string _name)
	{
		ot::GraphicsEllipseItemCfg* connector = new ot::GraphicsEllipseItemCfg();
		connector->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);
		connector->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

		connector->setName("Connector");
		connector->setMinimumSize(ot::Size2DD(50.0, 50.0));
		connector->setAlignment(ot::AlignCenter);

		return connector;
	}
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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions
std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) 
{
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	//if (action == "Circuit Simulator:Edit:New Circuit") return 	createNewCircuitEditor();
	if (action == "Circuit Simulator:Simulate:New Simulation")
	{
		auto allEntitiesByBlockID = m_blockEntityHandler.findAllBlockEntitiesByBlockID();
		m_ngSpice.ngSpice_Initialize(allEntitiesByBlockID);
	}
	//else {
		//OT_LOG_W("Unknown model action");
	//	assert(0);
	//	}// Unhandled button action
	return std::string();
}

std::string Application::handleNewGraphicsItem(ot::JsonDocument& _document)
{
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


std::string Application::handleRemoveGraphicsItem(ot::JsonDocument& _document)
{	
	std::list<std::string> items;

	// Add Item UIDs to the list above (Items to be removed)
	std::string itemUID = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
	items.push_back(itemUID);



	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleNewGraphicsItemConnection(ot::JsonDocument& _document)
{
	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

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

std::string Application::handleRemoveGraphicsItemConnection(ot::JsonDocument& _document)
{
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
	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}



// ############################## ####################################################################################################################################################################################

// Required functions

void Application::run(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);
	}
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	_ui->addMenuPage("Circuit Simulator");
	_ui->addMenuGroup("Circuit Simulator", "Edit");
	_ui->addMenuGroup("Circuit Simulator", "Simulate");
	_ui->addMenuButton("Circuit Simulator", "Edit","New Circuit", "New Circuit", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Add","Default");
	_ui->addMenuButton("Circuit Simulator","Simulate","New Simulation","New Simulation", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Add", "Default");

	m_blockEntityHandler.setUIComponent(_ui);
	m_blockEntityHandler.OrderUIToCreateBlockPicker();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	m_blockEntityHandler.setModelComponent(_model);
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

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

// ##################################################################################################################################################################################################################
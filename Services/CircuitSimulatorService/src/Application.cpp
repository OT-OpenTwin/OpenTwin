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
#include "OpenTwinCore/OwnerServiceGlobal.h"
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/Msg.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemCfg.h"


// Third Party Header
#include "ngspice/sharedspice.h"
#include <iostream>
#include <string>



Application * g_instance{ nullptr };

#define EXAMPLE_NAME_BLOCK1 "First"
#define EXAMPLE_NAME_Block2 "Second"

#undef GetObject

namespace ottest
{
	static unsigned long long currentBlocikUid = 0;

	ot::GraphicsFlowConnectorCfg getDefaultConnectorStyle(void) {
		ot::GraphicsFlowConnectorCfg cfg;

		cfg.setTextColor(ot::Color(0, 0, 0));

		return cfg;
	}

	ot::GraphicsItemCfg* createTestBlock1(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		
		flow.setBackgroundColor(ot::Color(0, 255, 255));

		flow.addLeft("Input1", "Input1", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Black);
		flow.addRight("Output1", "Output1", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Black);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock2(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		
		
		flow.setBackgroundColor(ot::Color(0, 255, 255));

		flow.addLeft("Input2", "Input2", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("Output2", "Output2", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Blue);

		return flow.createGraphicsItem(_name, _name);
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
std::string Application::handleExecuteModelAction(OT_rJSON_doc& _document) 
{
	std::string action = ot::rJSON::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "Circuit Simulator:Edit:New Circuit") return 	createNewCircuitEditor();
	else if (action == "Circuit Simulator:Simulate:New Simulation") return ngSpice_Initialize();
	else {
		OT_LOG_W("Unknown model action");
		assert(0);
	}// Unhandled button action
	return std::string();
}

std::string Application::handleNewGraphicsItem(OT_rJSON_doc& _document)
{
	//Here we get the Item Information
	std::string itemName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	OT_rJSON_val posObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition].GetObject();
	ot::Point2DD pos;
	pos.setFromJsonObject(posObj);

	//check and store information
	OT_LOG_D("Handling new graphics item request ( name = \"" + itemName + "\"; editor = \"" + editorName + "\"; x = " + std::to_string(pos.x()) + "; y = " + std::to_string(pos.y()) + " )");

	// Create the Package
	ot::GraphicsScenePackage pckg("Circuit");

	// Create item configuration for the item to add
	ot::GraphicsItemCfg* itm = nullptr;
	if (itemName == EXAMPLE_NAME_BLOCK1) { itm = ottest::createTestBlock1(EXAMPLE_NAME_BLOCK1); }
	else if (itemName == EXAMPLE_NAME_Block2) { itm = ottest::createTestBlock2(EXAMPLE_NAME_Block2); }
	else
	{
		m_uiComponent->displayMessage("[ERROR] Unknown item: " + itemName + "\n");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	itm->setPosition(pos);
	itm->setUid(std::to_string(++ottest::currentBlocikUid));
	pckg.addItem(itm);

	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

	OT_rJSON_createValueObject(pckgObj);
	pckg.addToJsonObject(reqDoc, pckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);

	ot::OwnerServiceGlobal::instance().addToJsonObject(reqDoc, reqDoc);

	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok, ot::rJSON::toJSON(pckgObj));
}


std::string Application::handleRemoveGraphicsItem(OT_rJSON_doc& _document)
{	
	std::list<std::string> items;

	// Add Item UIDs to the list above (Items to be removed)
	std::string itemUID = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
	items.push_back(itemUID);



	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleNewGraphicsItemConnection(OT_rJSON_doc& _document)
{
	OT_rJSON_checkMember(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, Object);
	OT_rJSON_val pckgObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_Package].GetObject();

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(pckgObj);

	// Here we would check and store the connection information
	OT_LOG_D("Handling new graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to add connections
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);

	// Add received package to reuest (all connections are allowed)
	OT_rJSON_createValueObject(reqPckgObj);
	pckg.addToJsonObject(reqDoc, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);

	ot::OwnerServiceGlobal::instance().addToJsonObject(reqDoc, reqDoc);
	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleRemoveGraphicsItemConnection(OT_rJSON_doc& _document)
{
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	OT_rJSON_checkMember(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, Object);
	OT_rJSON_val pckgObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_Package].GetObject();

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(pckgObj);

	// Here we would check and remove the connection information
	OT_LOG_D("Handling remove graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to remove connections
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection);
	
	// Add received package to reuest (all connections are allowed)
    OT_rJSON_createValueObject(reqPckgObj);
	pckg.addToJsonObject(reqDoc, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, editorName);

	ot::OwnerServiceGlobal::instance().addToJsonObject(reqDoc, reqDoc);
	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application:: createNewCircuitEditor(void)
{
	if (m_uiComponent) {
		

		ot::GraphicsNewEditorPackage pckg("Circuit", "Circuit Network");
		ot::GraphicsCollectionCfg* a = new ot::GraphicsCollectionCfg("CircuitElements", "Circuit Elements");
		ot::GraphicsCollectionCfg* a1 = new ot::GraphicsCollectionCfg("PassiveElements", "Passive Elements");
		
		a->addChildCollection(a1);
		a1->addItem(ottest::createTestBlock1(EXAMPLE_NAME_BLOCK1)); // In die Funktion kommt wie bei playground getItem bspw.
		a1->addItem(ottest::createTestBlock2(EXAMPLE_NAME_Block2));
		
		pckg.addCollection(a);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		ot::OwnerServiceGlobal::instance().addToJsonObject(doc, doc); //Eigentlich nur die OT_JSON_MEMBER_OwnerServiceGlobalId gewrapped

		std::string response;
		std::string req = ot::rJSON::toJSON(doc);

		OT_LOG_D("Requesting empty graphics editor ( editor = \"" + pckg.name() + "\"; title = \"" + pckg.title() + "\" )");

		if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
			return OT_ACTION_RETURN_VALUE_FAILED;
		}
	}

	return OT_ACTION_RETURN_VALUE_OK;
}


//Callback Functions for NGSpice

 int Application::MySendCharFunction(char* output, int ident, void* userData)
{
	 OT_LOG_D(output);

	return 0;
}

int Application::MySendStat(char* outputReturn, int ident, void* userData)
{
	Application::instance()->uiComponent()->displayMessage(std::string(outputReturn) + "\n");
	

	return 0;
}

int Application:: MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{
	
	OT_LOG_D(std::to_string(exitstatus));

	return 0;

}

//Initialize Function for NGSpice

std::string Application::ngSpice_Initialize()
{
	SendChar* printfcn = MySendCharFunction;
	SendStat* statfcn = MySendStat;
	ControlledExit* ngexit = MyControlledExit;
	std::string myString;

	int status = ngSpice_Init(MySendCharFunction, MySendStat, MyControlledExit, nullptr, nullptr, nullptr, nullptr);

	if (status == 0)
	{
		OT_LOG_D("Worked");

		std::list<std::string> enabled;
		
		std::list<std::string> disabled;
		disabled.push_back("Circuit Simulator:Simulate:New Simulation");
		m_uiComponent->setControlsEnabledState(enabled, disabled);

	}
	else if (status == 1)
	{
		OT_LOG_E("Something went wrong");
	}


	// Here i do 3 test Simulations

	//const char* netlistPath = "C:\\Users\\Sebastian\\Desktop\\NGSpice_Dateien_Test\\TransientTest.cir";

	//Transient Analysis

	// The command always have to be char* so i make them as arrays and write a command into
	/*char run[100] = "run";

	sprintf_s(run, "source %s", netlistPath);
	ngSpice_Command(run);

	
	char tran[100] = "tran 1ms 10ms";
	ngSpice_Command(tran);*/
	
	//AC-Analysis

	//char command[100];
	//sprintf_s(command, "source %s", netlistPath);
	//ngSpice_Command(command);

	//sprintf_s(command, "ac dec 1 10k 10");
	//ngSpice_Command(command);

	////Plot
	//char* currentPlot = ngSpice_CurPlot();
	//std::cout << "Aktueller Plot: " << currentPlot << std::endl;

	const char* netlistPath = "C:\\Users\\Sebastian\\Desktop\\NGSpice_Dateien_Test\\inv-example.cir";

	char command[100];
	sprintf_s(command, "source %s", netlistPath);
	ngSpice_Command(command);


	//For waiting then end of Simulation
	//while (ngSpice_running()) {
	//	using namespace std::chrono_literals;
	//	std::this_thread::sleep_for(1ms);	// Warten
	//}

	// quit NGSpice
	/*char quit[100] = "quit";
	ngSpice_Command(quit);*/

	myString = std::to_string(status);

	return myString;

	
	
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

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
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

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

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

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

// ##################################################################################################################################################################################################################
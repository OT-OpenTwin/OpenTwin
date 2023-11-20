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
#include "OTGui/GraphicsFlowItemCfg.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"





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

#undef GetObject

namespace ottest
{
	static unsigned long long currentBlockUid = 0;

	static unsigned long long currentResistorUid = 1;

	static unsigned long long currentNodeNumber = 0;

	static unsigned long long currentDiodeID = 1;

	ot::GraphicsFlowConnectorCfg getDefaultConnectorStyle(void) {
		ot::GraphicsFlowConnectorCfg cfg;

		cfg.setTextColor(ot::Color(0, 0, 0));

		return cfg;
	}

	ot::GraphicsItemCfg* createResistor(const std::string _name)
	{
		//First I create a stack item;
		ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
		myStack->setName("Resistor");
		myStack->setTitle("Resistor");
		

		//Second I create an Image
		ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
		image->setImagePath("CircuitElementImages/ResistorBG");
		image->setSizePolicy(ot::SizePolicy::Dynamic);

		myStack->addItemBottom(image, false, true);

		//Then I create a layout
		ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();
		
		myLayout->setMinimumSize(ot::Size2DD(400.0, 200.0));
		myStack->addItemTop(myLayout, true, false);
		

		
		

		//Now i want connections on the item for this i need rectangle items
		ot::GraphicsRectangularItemCfg* connection1 = new ot::GraphicsRectangularItemCfg();
		connection1->setName("Input1");
		connection1->setSize(ot::Size2DD(10.0, 10.0));
		ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		connection1->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		connection1->setBackgroundPainer(painter1);
		connection1->setAlignment(ot::AlignCenter);
		connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

		ot::GraphicsRectangularItemCfg* connection2 = new ot::GraphicsRectangularItemCfg();
		connection2->setName("Output1");
		connection2->setSize(ot::Size2DD(10.0, 10.0));
		ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
		connection2->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
		connection2->setBackgroundPainer(painter2);
		connection2->setAlignment(ot::AlignCenter);
		connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));

		connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
		

		//Here i add them to the Layout
		myLayout->addChildItem(connection1);
		myLayout->addStrech(1);
		myLayout->addChildItem(connection2);
		

		
		

		return myStack;
		
	}

	ot::GraphicsItemCfg* createVoltageSource(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 0, 255);
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		
		
		flow.setBackgroundColor(ot::Color(255, 255, 0));

		flow.addLeft("Input2", "Input2", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("Output2", "Output2", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Blue);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createDiode(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(255, 0, 0);
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());

		flow.setBackgroundColor(ot::Color(0, 0, 255));
		
		flow.addLeft("Input3", "Input3", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Cyan);
		flow.addRight("Output3", "Output3", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Cyan);

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
	if (itemName == EXAMPLE_NAME_BLOCK1) { itm = ottest::createResistor(EXAMPLE_NAME_BLOCK1); }
	else if (itemName == EXAMPLE_NAME_Block2) { itm = ottest::createVoltageSource(EXAMPLE_NAME_Block2); }
	else if (itemName == EXAMPLE_NAME_Block3) { itm = ottest::createDiode(EXAMPLE_NAME_Block3); }
	else
	{
		m_uiComponent->displayMessage("[ERROR] Unknown item: " + itemName + "\n");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	
	itm->setPosition(pos);
	itm->setUid(std::to_string(++ottest::currentBlockUid));
	pckg.addItem(itm);

	

	// Here i Create a buffer Object for generating netlist
	
	CircuitElement element;
	element.setItemName(itemName);
	element.setEditorName(editorName);
	element.setUID(itm->uid());
	if (element.getItemName() == "Resistor")
	{
		std::string temp = std::to_string(ottest::currentResistorUid++);
		std::string resistor = "R";
		element.setNetlistElementName(resistor + temp);
	}
	else if(element.getItemName() == "Diode")
	{
		std::string temp = std::to_string(ottest::currentDiodeID++);
		std::string diode = "D";
		element.setNetlistElementName(diode + temp);
	}
	else
	{
		element.setNetlistElementName("V1");
	}
	
	auto it = mapOfCircuits.find(editorName);
	if (it == mapOfCircuits.end())
	{
		Application::instance()->uiComponent()->displayMessage("No Circuit found");
	}
	else
	{
		it->second.addElement(element.getUID(), element);
	}



	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

	OT_rJSON_createValueObject(pckgObj);
	pckg.addToJsonObject(reqDoc, pckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);

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

	
	for (auto c : pckg.connections())
	{
		Connection connection(c);

		connection.setNodeNumber(std::to_string(ottest::currentNodeNumber++));

		auto it = mapOfCircuits.find(pckg.name());
		if(it == mapOfCircuits.end())
		{
			OT_LOG_E("Circuit not found { \"CircuitName\": \"" + pckg.name() + "\" }");
		}

		else
		{	
			//Here I add the connection to the Origin Element
			it->second.addConnection(connection.originUid(), connection);

			


			//Some Tests
			//std::cout << "Size: " << it->second.getElement(connection.originUid()).getList().size() << std::endl;
			
			//Here I add the connection to the Destination Element
			it->second.addConnection(connection.destUid(), connection);

			//test of print
			
			
			
		}
	}
	
	
	

	

	// Here we would check and store the connection information
	OT_LOG_D("Handling new graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to add connections
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);

	// Add received package to reuest (all connections are allowed)
	OT_rJSON_createValueObject(reqPckgObj);
	pckg.addToJsonObject(reqDoc, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);
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

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);
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
		a1->addItem(ottest::createResistor(EXAMPLE_NAME_BLOCK1)); // In die Funktion kommt wie bei playground getItem bspw.
		a1->addItem(ottest::createVoltageSource(EXAMPLE_NAME_Block2));
		a1->addItem(ottest::createDiode(EXAMPLE_NAME_Block3));
		
		pckg.addCollection(a);
		
		Circuit circuit;
		circuit.setEditorName(pckg.title());
		circuit.setId(pckg.name());
		mapOfCircuits.insert_or_assign(pckg.name(), circuit);



		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		this->getBasicServiceInformation().addToJsonObject(doc, doc);

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
	 Application::instance()->uiComponent()->displayMessage(std::string(output) + "\n");

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
		/*disabled.push_back("Circuit Simulator:Simulate:New Simulation");
		m_uiComponent->setControlsEnabledState(enabled, disabled);*/

	}
	else if (status == 1)
	{
		OT_LOG_E("Something went wrong");
	}


	// Some simulation

	generateNetlist();

	const char* netlistPath = "C:\\Users\\Sebastian\\Desktop\\NGSpice_Dateien_Test\\output.cir";

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

std::string Application::generateNetlist()
{
	//1Möglichkeit: Man könnte nach der Größe der NodenNumber sortieren das heißt dass alle knoten dann innerhalb eines NetlistString sortiert sind z.b statt
	// 2 1 kommt dann immer 1 2 also -> knoten 1 < knoten 2 . Jedoch müsste man für die spoannungsquelle das umgedreht machen für positive ergebnisse

	//2 Möglichkeit: Man sortiert die Liste der Connections für jedes Element aufsteigend der Nodenumbers nach mittels einer Funktion und dann bekommt man die 
	//Connections schon sortiert aus der Liste 


	// First I declare the path to the file
	
	std::ofstream outfile("C:/Users/Sebastian/Desktop/NGSpice_Dateien_Test/output.cir");
	//Here i have two Vectors for incomging and putgoing Connections
	

	std::string Title = "*Test";
	outfile << Title << std::endl;
	

	
	
	// Now I write the informations to the File
	// Here i get the Circuit and the map of Elements
	auto map = mapOfCircuits.find("Circuit")->second.getMapOfElements();
	// Now i iterate through this Map of Elements and save the information
	for (auto it : map)
	{
		
		std::string NodeNumbersString = "";
		std::string elementName_temp = it.second.getItemName();
		std::string netlistElementName = it.second.getNetlistElementName();
		std::string value;
		std::string elmentUID = it.second.getUID();


		//Here i get the Connection List
		auto connectionList = it.second.getList();
		
		if(elementName_temp == "VoltageSource")
		{
			for (auto c = connectionList.rbegin(); c != connectionList.rend(); c++)
			{
				NodeNumbersString += c->getNodeNumber();
				NodeNumbersString += " ";
			}
		}

		else
		{
			for (auto c : connectionList)
			{
				NodeNumbersString += c.getNodeNumber();
				NodeNumbersString += " ";

			}

		}

		

		
		

		//I set default values
		if (elementName_temp == "VoltageSource")
		{
			value = "12V";


		}
		else if(elementName_temp == "Resistor")
		{
			value = "200";
		}
		else
		{
			value = "myDiode";
			std::string diodeModel = ".model myDiode D (IS=1n RS=0.1 N=1)";
			outfile << diodeModel << std::endl;
		}

		//Create the end string 
		std::string NetlistLine = netlistElementName + " " + NodeNumbersString + value;

		outfile << NetlistLine << std::endl;
	}

	

	std::string Properties = ".dc V1 0V 12V 1V";
	outfile << Properties << std::endl;

	std::string control = ".Control";
	outfile << control << std::endl;;

	std::string run = "run";
	outfile << run << std::endl;

	std::string print = "print all";
	outfile << print << std::endl;

	std::string endc = ".endc";
	outfile << endc << std::endl;;

	std::string end = ".end";
	outfile << end << std::endl;


	std::string myString = "Succesfull";
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
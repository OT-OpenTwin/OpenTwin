//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
#include "CircuitElement.h"
//Open Twin Header
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
//Third Party Header
#include <string>
namespace Numbers
{
	static unsigned long long nodeNumber = 0;
	static unsigned long long voltageSourceNetlistNumber = 0;
	static unsigned long long resistorNetlistNumber = 0;
	
}

void NGSpice::clearBufferStructure(std::string name)
{
	this->getMapOfCircuits().find(name)->second.getMapOfElements().clear();

}

void NGSpice::updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	if ( it == Application::instance()->getNGSpice().getMapOfCircuits().end())
	{
		Circuit circuit;
		circuit.setEditorName(editorname);
		circuit.setId(editorname);
		Application::instance()->getNGSpice().getMapOfCircuits().insert_or_assign(editorname, circuit);
	}
	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		CircuitElement element;
		element.setEditorName(editorname);
		element.setItemName(blockEntity->getBlockTitle());
		element.setUID(blockEntity->getEntityID());

		if (blockEntity->getBlockTitle() == "Voltage Source")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitVoltageSource*>(blockEntity.get());
			element.setValue(myElement->getElementType());
			element.setType(myElement->getType());
			element.setFunction(myElement->getFunction());
		}
		else if (blockEntity->getBlockTitle() == "Resistor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
			element.setValue(myElement->getElementType());
		}

		auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
		it->second.addElement(element.getUID(), element);
	}
	
	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		auto connections = blockEntity->getAllConnections();
	
		for (auto connectionID : connections)
		{
			
			auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
			std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(connectionID);
			ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

			Connection conn(connectionCfg);
			conn.setNodeNumber(std::to_string(Numbers::nodeNumber++));

			bool res1 = it->second.addConnection(connectionCfg.getOriginUid(), conn);
			bool res2 = it->second.addConnection(connectionCfg.getDestinationUid(), conn);
			if (res1 == false && res2 == false )
			{
				Numbers::nodeNumber--;
			}			
		}
	}
}


std::string NGSpice::generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);

	EntityPropertiesString* printSettingsProperty = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Print Settings"));
	assert(printSettingsProperty != nullptr);

	std::string simulationType = simulationTypeProperty->getValue();
	std::string printSettings = printSettingsProperty->getValue();
	


	//Here i first create the Title of the Netlist
	std::string TitleLine = "circbyline *Test";

	//ngSpice_Command(const_cast<char*>(TitleLine.c_str()));
	//ngSpice_Command(const_cast<char*>("circbyline R1 0 1 200"));
	////ngSpice_Command(const_cast<char*>("circbyline iin 0 1 AC 1"));
	//ngSpice_Command(const_cast<char*>("circbyline V1 0 1 AC 10 sin(0 1 1k)"));
	//ngSpice_Command(const_cast<char*>("circbyline .ac dec 10 .01 10"));
	//ngSpice_Command(const_cast<char*>("circbyline .Control"));
	//ngSpice_Command(const_cast<char*>("circbyline run"));
	//ngSpice_Command(const_cast<char*>("circbyline asciiplot all"));
	//ngSpice_Command(const_cast<char*>("circbyline .endc"));
	//ngSpice_Command(const_cast<char*>("circbyline .end"));

	//Now i get the SimulationLine depending on which simulation i chose
	std::string simulationLine = "";
	
	if (simulationType == ".dc")
	{
		simulationLine = generateNetlistDCSimulation(solverEntity, allConnectionEntities, allEntitiesByBlockID, editorname);
		
	}
	else if (simulationType == ".TRAN")
	{
		simulationLine = generateNetlistTRANSimulation(solverEntity, allConnectionEntities, allEntitiesByBlockID, editorname);
		
	}
	else
	{
		simulationLine = generateNetlistACSimulation(solverEntity, allConnectionEntities, allEntitiesByBlockID, editorname);
		
	}

	simulationLine = "circbyline " + simulationLine;
	printSettings = "circbyline " + printSettings;
	

	//Here i create and generate all the netlist Elements and their relation to each other line by line

	auto it =Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	 
	for (auto mapOfElements : it->second.getMapOfElements())
	{
		auto element = mapOfElements.second;
		std::string netlistElementName = "";
		std::string netlistLine="circbyline ";
		std::string netlistValue = element.getValue();
		std::string netlistNodeNumbers;
		std::string netlistVoltageSourceType="";

		if (element.getItemName() == "Voltage Source")
		{
			netlistElementName += "V" + std::to_string(++Numbers::voltageSourceNetlistNumber);
			netlistVoltageSourceType = element.getType() + " ";

			//if the voltage source is an AC i need the function for it
			if (netlistVoltageSourceType == "AC ")
			{
				netlistVoltageSourceType += element.getFunction();
			}
			netlistLine += netlistElementName + " ";
			
		}
		else if (element.getItemName() == "Resistor")
		{
			netlistElementName = "R" + std::to_string(++Numbers::resistorNetlistNumber);
			netlistLine += netlistElementName + " ";
		}

		auto connections = element.getList();
		for (auto conn : connections)
		{
			netlistNodeNumbers += conn.getNodeNumber() + " ";
		}

		netlistLine += netlistNodeNumbers;

		if (netlistVoltageSourceType != "")
		{
			netlistLine += netlistVoltageSourceType;
		}
		if (element.getType() != "AC")
		{
			netlistLine += netlistValue;
		}
		
		ngSpice_Command(const_cast<char*>(netlistLine.c_str()));
	}


	

	ngSpice_Command(const_cast<char*>(simulationLine.c_str()));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>(printSettings.c_str()));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));



	return "success";
}

std::string NGSpice::generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname)
{
	EntityPropertiesEntityList* elementProperty = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Element"));

	std::string element = Application::instance()->extractStringAfterDelimiter(elementProperty->getValueName(), '/', 2);
	if (element == "Voltage Source")
	{
		element = "V1";
	}

	std::string simulationLine="";
	std::string type = ".dc";
	EntityPropertiesString* from = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("From"));
	EntityPropertiesString* to = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("To"));
	EntityPropertiesString* step = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Step"));
	simulationLine = type + " " + element + " " + from->getValue() + " " + to->getValue() + " " + step->getValue();
	return simulationLine;
}

std::string NGSpice::generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname)
{
	EntityPropertiesSelection* variation = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Variation"));
	EntityPropertiesString* np = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Number of Points"));
	EntityPropertiesString* fStart = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Starting Frequency"));
	EntityPropertiesString* fEnd = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Final Frequency"));

	std::string simulationLine = "";
	std::string type = ".ac";

	simulationLine = type + " " + variation->getValue() + " " + np->getValue() + " " + fStart->getValue() + " " + fEnd->getValue();

	return simulationLine;
}

std::string NGSpice::generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname)
{
	EntityPropertiesString* duration = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Duration"));
	EntityPropertiesString* timeSteps = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("TimeSteps"));

	std::string simulationLine = "";
	std::string type = ".TRAN";

	simulationLine = type + " " + duration->getValue() + " " + timeSteps->getValue();

	return simulationLine;
}

std::string NGSpice::ngSpice_Initialize(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
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


	
	 updateBufferClasses(allConnectionEntities,allEntitiesByBlockID,editorname);
	 generateNetlist( solverEntity, allConnectionEntities,allEntitiesByBlockID, editorname);

	 Numbers::nodeNumber = 0;
	 Numbers::resistorNetlistNumber = 0;
	 Numbers::voltageSourceNetlistNumber = 0;
	 
	/*char command[1000];
	const char* netlist = "C:/Users/Sebastian/Desktop/NGSpice_Dateien_Test/Test.cir";
	sprintf_s(command, sizeof(command), "source %s", netlist);
	ngSpice_Command(command);*/

	myString = std::to_string(status);

	return myString;
}


//Callback Functions for NGSpice
int NGSpice::MySendCharFunction(char* output, int ident, void* userData)
{
	Application::instance()->uiComponent()->displayMessage(std::string(output) + "\n");

	return 0;
}

int NGSpice::MySendStat(char* outputReturn, int ident, void* userData)
{
	Application::instance()->uiComponent()->displayMessage(std::string(outputReturn) + "\n");


	return 0;
}

int NGSpice::MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{

	OT_LOG_D(std::to_string(exitstatus));

	return 0;

}


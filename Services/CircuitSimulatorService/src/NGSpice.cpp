//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
#include "CircuitElement.h"
#include "SimulationResults.h"

//Open Twin Header
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
#include "EntityBlockCircuitDiode.h"
#include "EntityBlockCircuitVoltageMeter.h"
//Third Party Header
#include <string>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <unordered_set>

//C++ Header
#include <sstream>
namespace Numbers
{
	static unsigned long long nodeNumber = 0;
	static unsigned long long voltageSourceNetlistNumber = 0;
	static unsigned long long resistorNetlistNumber = 0;
	static unsigned long long diodeNetlistNumber = 0;
}

void NGSpice::clearBufferStructure(std::string name)
{
	this->getMapOfCircuits().find(name)->second.getMapOfElements().clear();
	this->getMapOfCircuits().find(name)->second.getMapOfEntityBlcks().clear();
	SimulationResults::getInstance()->getResultMap().clear();
	

}

std::vector<std::string> NGSpice::getNodeNumbersOfVoltageMeter(std::string editorName, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID)
{
	std::vector<std::string> nodeNumberString;
	std::string nodes;
	
	//First i go through all Entities to find the Voltage Meter
	auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorName)->second.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageMeter");

	for (auto& voltageMeter : it->second)
	{
		if (isValidNodeString(nodes))
		{
			nodeNumberString.push_back("v(" + nodes + ")");
			nodes = "";
			continue;
		}

		
		//When found i go through its all connections 
			
		ot::UID voltageMeterUID = voltageMeter->getEntityID();
		auto voltageMeterconnections = voltageMeter->getAllConnections();

		for (auto voltageMeterConnectionID : voltageMeterconnections)
		{
			if (isValidNodeString(nodes))
			{
				nodeNumberString.push_back("v(" + nodes + ")");
				nodes = "";
				continue;
			}

			//Here i get the connectionCfg to earn all information about the connection
			std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(voltageMeterConnectionID);
			ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

			//Now i find out the UID of the other element and the connectorName
			ot::UID connectedElementUID;
			std::string connectorName = "";
			if (connectionCfg.getOriginUid() != voltageMeterUID)
			{
				connectorName = connectionCfg.originConnectable();
				connectedElementUID = connectionCfg.getOriginUid();
			}
			else if(connectionCfg.getDestinationUid() != voltageMeterUID)
			{
				connectorName = connectionCfg.destConnectable();
				connectedElementUID = connectionCfg.getDestinationUid();
			}
				

			// Here i get the connectedElement out of my BufferClass
			auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorName);
			auto netlistElement = it->second.getMapOfElements().find(connectedElementUID);
					
			//Now i go through all connections of the Element and try to find the right one to get the nodeNumber
			for (auto netlistConn : netlistElement->second.getList())
			{
				if (isValidNodeString(nodes))
				{
					
					nodeNumberString.push_back("v(" + nodes + ")");
					nodes = "";
					continue;
				}
					
				if (netlistConn.getNodeNumber() != "voltageMeterConnection")
				{
					if (netlistConn.originConnectable() == connectorName && netlistConn.getOriginUid() == connectedElementUID ||
						netlistConn.destConnectable() == connectorName && netlistConn.getDestinationUid() == connectedElementUID)
					{
						
						size_t position = nodes.find(netlistConn.getNodeNumber());
						if (position != std::string::npos)
						{
							continue;
						}
						else
						{
							size_t pos = nodes.find(",");
							if (pos == std::string::npos)
							{
								nodes += netlistConn.getNodeNumber() + ",";
							}
							else
							{
								nodes += netlistConn.getNodeNumber();
							}
									
						}
					}
				}
				else
				{
					continue;
				}
			}		
		}	
	}

	
	return nodeNumberString;

	//Build a map in which the value is a vector and put in their all elements to find easy the voltage Meter elements and not to go trough all with a for loop

}

bool NGSpice::isValidNodeString(const std::string& input)
{
	std::stringstream ss(input);
	std::string token;
	std::vector<std::string> tokens;

	while (std::getline(ss, token, ',')) {
		tokens.push_back(token);
	}

	if (tokens.size() != 2) {
		return false;
	}

	for (const auto& tok : tokens) {
		for (char c : tok) {
			if (!isdigit(c)) {
				return false;
			}
		}
	}

	return true;
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
		it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
		it->second.addBlockEntity(blockEntity->getClassName(), blockEntity);
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
		else if (blockEntity->getBlockTitle() == "Diode")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitDiode*>(blockEntity.get());
			element.setValue(myElement->getElementType());
		}
		else if (blockEntity->getBlockTitle() == "Voltage Meter")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitVoltageMeter*>(blockEntity.get());
		}

		
		it->second.addElement(element.getUID(), element);
	}

	
	std::map<std::pair<ot::UID, std::string>, std::string> connectionMap;
	

	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		auto connections = blockEntity->getAllConnections();

		// Map used for parallel connections to identify if a connection already exists on the connector and then give the second or ... connection the same nodenumber
		

		for (auto connectionID : connections)
		{
			
			auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
			std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(connectionID);
			ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

			Connection conn(connectionCfg);

			ot::UID destUID;
			if (conn.getOriginUid() == blockEntity->getEntityID())
			{
				destUID = conn.getOriginUid();
			}
			else if (conn.getDestinationUid() == blockEntity->getEntityID())
			{
				destUID = conn.getDestinationUid();
			}

			bool temp = false;
			//If the connection is has the voltage Meter as origin or destination then we dont want to give it a nodeNumber
			if ((allEntitiesByBlockID.at(connectionCfg.getOriginUid())->getBlockTitle() != "Voltage Meter") && (allEntitiesByBlockID.at(connectionCfg.getDestinationUid())->getBlockTitle() != "Voltage Meter"))
			{
				auto connectionKey = std::make_pair(destUID,connectionCfg.destConnectable());

				if (connectionMap.find(connectionKey) != connectionMap.end())
				{
					connectionMap[connectionKey] = connectionMap.at(connectionKey);
					conn.setNodeNumber(connectionMap[connectionKey]);
				}
				else
				{
					connectionMap[connectionKey] = std::to_string(Numbers::nodeNumber++);
					conn.setNodeNumber(connectionMap[connectionKey]);
					temp = true;
				}
			}
			else
			{
				conn.setNodeNumber("voltageMeterConnection");
			}

			bool res1 = it->second.addConnection(connectionCfg.getOriginUid(), conn);
			bool res2 = it->second.addConnection(connectionCfg.getDestinationUid(), conn);
			if (res1 == false && res2 == false && temp == true)
			{
				Numbers::nodeNumber--;
				temp = false;
			}
		}

		
		
		
	}
}


std::string NGSpice::generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	
	


	//Here i first create the Title of the Netlist and send it to NGSpice
	std::string TitleLine = "circbyline *Test";
	ngSpice_Command(const_cast<char*>(TitleLine.c_str()));
	
	
	
	//ngSpice_Command(const_cast<char*>("circbyline R1 0 1 200"));
	//ngSpice_Command(const_cast<char*>("circbyline iin 0 1 AC 1"));
	//ngSpice_Command(const_cast<char*>("circbyline V1 0 1 AC 10 sin(0 1 1k)"));
	//ngSpice_Command(const_cast<char*>("circbyline .ac dec 10 .01 10"));
	//ngSpice_Command(const_cast<char*>("circbyline .Control"));
	//ngSpice_Command(const_cast<char*>("circbyline run"));
	//ngSpice_Command(const_cast<char*>("circbyline asciiplot all"));
	//ngSpice_Command(const_cast<char*>("circbyline .endc"));
	//ngSpice_Command(const_cast<char*>("circbyline .end"));


	

	//As next i create the Circuit Element Netlist Lines by getting the information out of the BufferClasses 

	std::vector<std::string> nodesOfVoltageMeter;

	auto it =Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	 
	for (auto mapOfElements : it->second.getMapOfElements())
	{
		auto element = mapOfElements.second;

		std::string netlistElementName = "";
		std::string netlistLine="circbyline ";
		std::string netlistValue = element.getValue();
		std::string netlistNodeNumbers;
		std::string netlistVoltageSourceType="";
		std::string modelNetlistLine = "circbyline ";
		
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
		else if (element.getItemName() == "Diode")
		{
			netlistElementName = "D" + std::to_string(++Numbers::diodeNetlistNumber);
			netlistLine += netlistElementName + " ";
			modelNetlistLine += ".MODEL D1N4148 D(IS=1e-15)";
		}
		else if (element.getItemName() == "Voltage Meter")
		{
			if (nodesOfVoltageMeter.size() == 0)
			{
				nodesOfVoltageMeter = getNodeNumbersOfVoltageMeter(editorname, allConnectionEntities, allEntitiesByBlockID);
				continue;
			}
			else
			{
				continue;
			}
			
		}
	
		

		//From begin
		auto connections = element.getList();

		/*for (auto conn : connections)
		{
			netlistNodeNumbers += conn.getNodeNumber() + " ";
		}*/

		//From behind
		std::vector<Connection> tempVector(connections.begin(), connections.end());
		std::reverse(tempVector.begin(), tempVector.end());
		std::unordered_set<std::string> temp;
		for (auto conn : tempVector)
		{
			if (conn.getNodeNumber() == "voltageMeterConnection")
			{
				continue;
			}
			else if (temp.find(conn.getNodeNumber()) != temp.end())
			{
				continue;
			}
			netlistNodeNumbers += conn.getNodeNumber() + " ";
			temp.insert(conn.getNodeNumber());
		}

		temp.clear();

		netlistLine += netlistNodeNumbers;

		if (netlistVoltageSourceType != "")
		{
			netlistLine += netlistVoltageSourceType;
		}
		if (element.getType() != "AC")
		{
			netlistLine += netlistValue;
		}


		//Here i send the Lines to NGSpice dll
		ngSpice_Command(const_cast<char*>(netlistLine.c_str()));
		if (modelNetlistLine != "circbyline ")
		{
			ngSpice_Command(const_cast<char*>(modelNetlistLine.c_str()));
		}
	}

	//After i got the TitleLine and the elements which represent my circuit I check which simulation was chosen and create the simlationLine
	
	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);

	/*EntityPropertiesString* printSettingsProperty = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Print Settings"));
	assert(printSettingsProperty != nullptr);*/

	std::string simulationType = simulationTypeProperty->getValue();

	std::string printSettings = "print ";
	for (auto nodes : nodesOfVoltageMeter)
	{
		printSettings += nodes + " ";
	}
	

	
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

	
	//And now i send it to NGSpice in the right order
	//ngSpice_Command(const_cast<char*>("circbyline .probe vd (V1:1, V2:2)"));
	//ngSpice_Command(const_cast<char*>("circbyline V2 2 1 0"));
	ngSpice_Command(const_cast<char*>(simulationLine.c_str()));
	//ngSpice_Command(const_cast<char*>(" circybline E_diff 3 0 2 1 gain = 1"));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	//ngSpice_Command(const_cast<char*>("circbyline probe v(1,2)"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>(printSettings.c_str()));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	//ngSpice_Command(const_cast<char*>(sendData.c_str()));
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
	SendData* datfcn = MySendDataFunction;
	SendInitData* initData = MySendInitDataFunction;
	std::string myString;

	int status = ngSpice_Init(MySendCharFunction, MySendStat, MyControlledExit, MySendDataFunction, MySendInitDataFunction, nullptr, nullptr);

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



int NGSpice::MySendDataFunction(pvecvaluesall vectorsAll, int numStructs, int idNumNGSpiceSharedLib, void* userData)
{
	
	for (int i = 0; i < vectorsAll->veccount; ++i) {
		std::string name = vectorsAll->vecsa[i]->name;
		double value = vectorsAll->vecsa[i]->creal;
		SimulationResults::getInstance()->getResultMap().at(name).push_back(value);
	}
	
	return 0;
}

int NGSpice::MySendInitDataFunction(pvecinfoall vectorInfoAll, int idNumNGSpiceSharedLib, void* userData)
{
	for (int i = 0; i < vectorInfoAll->veccount; i++)
	{
		std::string name = vectorInfoAll->vecs[i]->vecname;
		std::vector<double> values;
		SimulationResults::getInstance()->setVecAmount(vectorInfoAll->veccount);
		if (name != "voltagemeterconnection")
		{
			SimulationResults::getInstance()->getResultMap().insert_or_assign(name, values);
		}
		else
		{
			continue;
		}
		


	}

	return 0;
}



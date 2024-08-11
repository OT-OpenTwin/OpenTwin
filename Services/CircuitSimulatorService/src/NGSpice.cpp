//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
#include "CircuitElements/VoltageSource.h"
#include "CircuitElements/Resistor.h"
#include "CircuitElements/Diode.h"
#include "CircuitElements/Inductor.h"
#include "CircuitElements/Capacitor.h"
#include "CircuitElements/VoltageMeter.h"
#include "CircuitElements/CurrentMeter.h"
#include "SimulationResults.h"


//Open Twin Header
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
#include "EntityBlockCircuitDiode.h"
#include "EntityBlockCircuitVoltageMeter.h"
#include "EntityBlockCircuitCapacitor.h"
#include "EntityBlockCircuitInductor.h"
#include "EntityBlockCircuitCurrentMeter.h"

//Third Party Header
#include <string>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <unordered_set>
#include <boost/functional/hash.hpp>

//C++ Header
#include <sstream>
namespace Numbers
{
	static unsigned long long nodeNumber = 1;
	static unsigned long long RshunNumbers = 0;
}

void NGSpice::clearBufferStructure(std::string name)
{
	this->getMapOfCircuits().find(name)->second.getMapOfElements().clear();
	this->getMapOfCircuits().find(name)->second.getMapOfEntityBlcks().clear();
	SimulationResults::getInstance()->getResultMap().clear();
	

}

void NGSpice::getNodeNumbersOfMeters(std::string editorName, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::vector<std::string>& nodesOfVoltageMeter)
{
	
	std::string nodes;
	
	//First i go through all Entities to find the Voltage Meter
	auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorName)->second.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageMeter");


	for (auto& voltageMeter : it->second)
	{
		if (isValidNodeString(nodes) && voltageMeter->getClassName() == "EntityBlockCircuitVoltageMeter")
		{
			nodesOfVoltageMeter.push_back("(" + nodes + ")");
			nodes = "";
			continue;
		}
		
		
		//When found i go through its all connections 
			
		ot::UID voltageMeterUID = voltageMeter->getEntityID();
		auto voltageMeterconnections = voltageMeter->getAllConnections();

		for (auto voltageMeterConnectionID : voltageMeterconnections)
		{
			if (isValidNodeString(nodes) && voltageMeter->getClassName() == "EntityBlockCircuitVoltageMeter")
			{
				nodesOfVoltageMeter.push_back("(" + nodes + ")");
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
				connectorName = connectionCfg.getOriginConnectable();
				connectedElementUID = connectionCfg.getOriginUid();
			}
			else if (connectionCfg.getDestinationUid() != voltageMeterUID)
			{
				connectorName = connectionCfg.getDestConnectable();
				connectedElementUID = connectionCfg.getDestinationUid();
			}
			
			// Here i get the connectedElement out of my BufferClass
			auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorName);
			auto netlistElement = it->second.getMapOfElements().find(connectedElementUID);
					
			//Now i go through all connections of the Element and try to find the right one to get the nodeNumber
			for (auto netlistConn : netlistElement->second->getList())
			{
				if (isValidNodeString(nodes) && voltageMeter->getClassName() == "EntityBlockCircuitVoltageMeter")
				{
					nodesOfVoltageMeter.push_back("(" + nodes + ")");
					nodes = "";
					continue;
				}
				
					
				if (netlistConn.getNodeNumber() != "voltageMeterConnection")
				{
					if (netlistConn.getOriginConnectable() == connectorName && netlistConn.getOriginUid() == connectedElementUID ||
						netlistConn.getDestConnectable() == connectorName && netlistConn.getDestinationUid() == connectedElementUID)
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

	std::string notInitialized = "NotYet";

	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
		it->second.addBlockEntity(blockEntity->getClassName(), blockEntity);
		

		if (blockEntity->getBlockTitle() == "Voltage Source")
		{

			auto myElement = dynamic_cast<EntityBlockCircuitVoltageSource*>(blockEntity.get());
			
			auto voltageSource = std::make_unique<VoltageSource>(myElement->getVoltage(),myElement->getFunction(),notInitialized,myElement->getAmplitude(),
										myElement->getBlockTitle(),editorname,myElement->getEntityID(),notInitialized);
			
			
			
			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				
				voltageSource->setNetlistName("V1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				voltageSource->setNetlistName("V" + counter);
			}

			if (voltageSource->getFunction() == "PULSE")
			{
				std::string function = "PULSE(";
				std::vector<std::string> parameters = myElement->getPulseParameters();

				for (auto parameter : parameters)
				{
					function += parameter + " ";
				}

				function += ")";

				voltageSource->setFunction(function);
				
			}
			else if (voltageSource->getFunction() == "SIN")
			{
				std::string function = "SIN(";
				std::vector<std::string> parameters = myElement->getSinParameters();
				
				for (auto parameter : parameters)
				{
					function += parameter + " ";
				}

				function += ")";

				voltageSource->setFunction(function);
			}
			else if (voltageSource->getFunction() == "EXP")
			{
				std::string function = "EXP(";
				std::vector<std::string> parameters = myElement->getExpParameters();

				for (auto parameter : parameters)
				{
					function += parameter + " ";
				}

				function += ")";

				voltageSource->setFunction(function);
			}
			ot::UID uid = voltageSource->getUID();
			auto voltageSource_p = voltageSource.release();
			it->second.addElement(uid, voltageSource_p);

		}
		else if (blockEntity->getBlockTitle() == "Resistor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
			auto resistor = std::make_unique<Resistor>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);
			

			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				resistor->setNetlistName("R1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				resistor->setNetlistName("R" + counter);
			}

			ot::UID uid = resistor->getUID();
			auto resistor_p = resistor.release();
			it->second.addElement(uid, resistor_p);
		}
		else if (blockEntity->getBlockTitle() == "Diode")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitDiode*>(blockEntity.get());
			auto diode = std::make_unique<Diode>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);


			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				diode->setNetlistName("D1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				diode->setNetlistName("D" + counter);
			}
			ot::UID uid = diode->getUID();
			auto diode_p = diode.release();
			it->second.addElement(uid, diode_p);
		}
		else if (blockEntity->getBlockTitle() == "Capacitor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitCapacitor*>(blockEntity.get());
			auto capacitor = std::make_unique<Capacitor>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				capacitor->setNetlistName("C1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				capacitor->setNetlistName("C" + counter);
			}

			ot::UID uid = capacitor->getUID();
			auto capacitor_p = capacitor.release();
			it->second.addElement(uid, capacitor_p);
		}
		else if (blockEntity->getBlockTitle() == "Inductor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitInductor*>(blockEntity.get());
			auto inductor = std::make_unique<Inductor>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				inductor->setNetlistName("L1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				inductor->setNetlistName("L" + counter);
			}

			ot::UID uid = inductor->getUID();
			auto inductor_p = inductor.release();
			it->second.addElement(uid, inductor_p);
		}
		else if (blockEntity->getBlockTitle() == "Voltage Meter") {
			auto myElement = dynamic_cast<EntityBlockCircuitVoltageMeter*>(blockEntity.get());
			auto voltMeter = std::make_unique<VoltageMeter>( myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				voltMeter->setNetlistName("VM1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				voltMeter->setNetlistName("VM" + counter);
			}

			ot::UID uid = voltMeter->getUID();
			auto voltMeter_p = voltMeter.release();
			it->second.addElement(uid, voltMeter_p);
		}
		else if (blockEntity->getBlockTitle() == "Current Meter") {
			auto myElement = dynamic_cast<EntityBlockCircuitCurrentMeter*>(blockEntity.get());
			auto currentMeter = std::make_unique<CurrentMeter>(myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			std::string counter = Application::instance()->extractStringAfterDelimiter(myElement->getName(), '_', 1);
			if (counter == "first")
			{
				currentMeter->setNetlistName("VM1");
			}
			else
			{
				int temp = std::stoi(counter);
				temp += 1;
				counter = std::to_string(temp);

				currentMeter->setNetlistName("VM" + counter);
			}

			ot::UID uid = currentMeter->getUID();
			auto currentMeter_p = currentMeter.release();
			it->second.addElement(uid, currentMeter_p);
		}
	
	}

	//std::unordered_map<std::pair<ot::UID, std::string>, std::string, PairHash> connectionNodeNumbers; 
	std::map<std::pair<ot::UID, std::string>, std::string> connectionNodeNumbers;
	//Lieber normale map nehmen 

	// I want to be able to have always the same structure and naming of nodeNumbers in the circuit in all cases the user might build the circuit.
	// Means that the user can start with every element to drop in the scene and start with any connection he want and this will not influence the results of the simulation
	// and the coordination and of the nodenumbers and elements
	// I want always a flow from positive node to negative node 
	// I first start with the voltageSource and give it the connection id = 0 then i will go to the next element at the connection and to the opposite connetor and give it the 
	// next nodenumber. I will take use of traversing graphs with my code of checking parallel connections. I just need a good understandable structure

	// First I get alle the VoltageSources of the Circuit
	//auto vectorVoltageSources = it->second.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageSource");
	//if ( vectorVoltageSources != it->second.getMapOfEntityBlcks().end()) {
	//	
	//	// Now i go through every VoltageSource and get its connections
	//	for (auto voltageSource : vectorVoltageSources->second) {
	//		auto connections = voltageSource->getAllConnections();

	//		for (auto connectionID : connections)
	//		{

	//			std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(connectionID);
	//			ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

	//			//After i got a connection i check if the connection is at the positivePole of my VoltageSource
	//			if (connectionCfg.getDestConnectable() == "positivePole" || connectionCfg.getOriginConnectable() == "positivePole")
	//			{
	//				// If its at positivePole no matter if destination or origin i add it to the map if it does not exist and give it a nodeNumber
	//				// Also i check if its a VoltageMeter connection, when it is one then i ignore it and go on
	//				Connection myConn(connectionCfg);

	//				if ((allEntitiesByBlockID.at(connectionCfg.getOriginUid())->getBlockTitle() != "Voltage Meter") && (allEntitiesByBlockID.at(connectionCfg.getDestinationUid())->getBlockTitle() != "Voltage Meter")) {

	//					auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(),myConn.getDestConnectable() });
	//					if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
	//						myConn.setNodeNumber(connectionWithNodeNumber->second);

	//					}
	//					else {
	//						connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
	//						if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
	//							myConn.setNodeNumber(connectionWithNodeNumber->second);

	//						}
	//						else {

	//							myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
	//							connectionNodeNumbers.insert_or_assign({ myConn.getDestinationUid(), myConn.getDestConnectable() }, myConn.getNodeNumber());
	//							connectionNodeNumbers.insert_or_assign({ myConn.getOriginUid(), myConn.getOriginConnectable() }, myConn.getNodeNumber());
	//						}
	//					}
	//				}

	//				else {
	//					myConn.setNodeNumber("voltageMeterConnection");
	//				}


	//				it->second.addConnection(connectionCfg.getOriginUid(), myConn);
	//				it->second.addConnection(connectionCfg.getDestinationUid(), myConn);
	//			}
	//			else {
	//				continue;
	//			}

	//			// After i added the connection i go to the next Element of this connection and check its connections
	//			 
	//			


	//		}
	//	}

	//}
	


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

			Connection myConn(connectionCfg);

			//1.Methode

			if ((allEntitiesByBlockID.at(connectionCfg.getOriginUid())->getBlockTitle() != "Voltage Meter") && (allEntitiesByBlockID.at(connectionCfg.getDestinationUid())->getBlockTitle() != "Voltage Meter"))
			{
				auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(),myConn.getDestConnectable() });
				if (connectionWithNodeNumber != connectionNodeNumbers.end())
				{
					myConn.setNodeNumber(connectionWithNodeNumber->second);

				}
				else
				{
					connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
					if (connectionWithNodeNumber != connectionNodeNumbers.end())
					{
						myConn.setNodeNumber(connectionWithNodeNumber->second);

					}
					else
					{	

						myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
						connectionNodeNumbers.insert_or_assign({ myConn.getDestinationUid(), myConn.getDestConnectable() }, myConn.getNodeNumber());
						connectionNodeNumbers.insert_or_assign({ myConn.getOriginUid(), myConn.getOriginConnectable() }, myConn.getNodeNumber());
					}
				}
			}
			else
			{
				myConn.setNodeNumber("voltageMeterConnection");
			}


			it->second.addConnection(connectionCfg.getOriginUid(), myConn);
			it->second.addConnection(connectionCfg.getDestinationUid(), myConn);
			
		}

		
		
		
	}
}


std::string NGSpice::generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	
	


	//Here i first create the Title of the Netlist and send it to NGSpice
	std::string TitleLine = "circbyline *Test";
	ngSpice_Command(const_cast<char*>(TitleLine.c_str()));
	/*ngSpice_Command(const_cast<char*>("circbyline V1 1 0 AC 1 0"));
	ngSpice_Command(const_cast<char*>("circbyline R1 1 2 1k"));
	ngSpice_Command(const_cast<char*>("circbyline L1 2 3 100mH"));
	ngSpice_Command(const_cast<char*>("circbyline C1 3 0 1uF"));
	ngSpice_Command(const_cast<char*>("circbyline .ac dec 10 10 100k"));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));*/

	//As next i create the Circuit Element Netlist Lines by getting the information out of the BufferClasses 

	std::vector<std::string> nodesOfVoltageMeter;
	std::vector<std::vector<std::string>> nodesOfCurrentMeter;

	// I need to get the type of Simulation to set then the voltage source if its ac dc or tran
	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);
	std::string simulationType = simulationTypeProperty->getValue();
	std::string voltageSourceType = "";

	auto it =Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	 
	for (const auto& mapOfElements : it->second.getMapOfElements())
	{
		auto circuitElement = mapOfElements.second;

		std::string netlistElementName = "";
		std::string netlistLine="circbyline ";
		std::string netlistValue = "";
		std::string netlistNodeNumbers;
		std::string netlistVoltageSourceType="";
		std::string modelNetlistLine = "circbyline ";
		
		if (circuitElement->type() == "VoltageSource")
		{
			VoltageSource* voltagesource = dynamic_cast<VoltageSource*>(circuitElement);
			netlistElementName = voltagesource->getNetlistName();
			if (simulationType == ".dc")
			{
				voltagesource->setType("DC");
				netlistVoltageSourceType = voltagesource->getType() + " ";
				netlistValue = voltagesource->getValue();
			}
			else if (simulationType == ".ac")
			{
				voltageSourceType = "AC ";
				netlistVoltageSourceType = "DC 0 " + voltageSourceType + voltagesource->getAmplitude();
				
			}
			else
			{
				voltageSourceType = "TRAN";
				netlistVoltageSourceType = voltagesource->getFunction();
			}
			
	
			netlistLine += netlistElementName + " ";
			
		}
		else if (circuitElement->type() == "Resistor")
		{
			Resistor* resistor = dynamic_cast<Resistor*>(circuitElement);

			netlistElementName = resistor->getNetlistName();
			netlistLine += netlistElementName + " ";
			netlistValue = resistor->getResistance();
		}
		else if (circuitElement->type() == "Diode")
		{
			Diode* diode = dynamic_cast<Diode*>(circuitElement);

			netlistElementName = diode->getNetlistName();
			netlistLine += netlistElementName + " ";
			modelNetlistLine += ".MODEL D1N4148 D(IS=1e-15)";
			netlistValue = diode->getValue();
		}
		else if (circuitElement->type() == "VoltageMeter")
		{
			if (nodesOfVoltageMeter.size() == 0)
			{
				getNodeNumbersOfMeters(editorname, allConnectionEntities, allEntitiesByBlockID,nodesOfVoltageMeter);
				// I am doing a continue here becaue i dont want to generate an instance Line for this element
				continue;
			}
			else
			{
				continue;
			}
			
		}
		else if (circuitElement->type() == "CurrentMeter")
		{

			if (nodesOfCurrentMeter.size() == 0)
			{
				
				std::vector<std::string> nodeNumbers;
				std::unordered_set<std::string> temp;

				for (auto conn : circuitElement->getList())
				{
					if (temp.find(conn.getNodeNumber()) != temp.end())
					{
						continue;
					}

					temp.insert(conn.getNodeNumber());
					nodeNumbers.push_back(conn.getNodeNumber());
				}
				nodesOfCurrentMeter.push_back(nodeNumbers);
				
				// Here i do a continue because i dont want to generate an instance line for this element
				continue;
			}
			else
			{
				continue;
			}
		}
		else if (circuitElement->type() == "Capacitor")
		{
			Capacitor* capacitor = dynamic_cast<Capacitor*>(circuitElement);

			netlistElementName = capacitor->getNetlistName();
			netlistLine += netlistElementName + " ";
			netlistValue = capacitor->getCapacity();
		}
		else if (circuitElement->type() == "Inductor")
		{
			Inductor* inductor = dynamic_cast<Inductor*>(circuitElement);

			netlistElementName = inductor->getNetlistName();
			netlistLine += netlistElementName + " ";
			netlistValue = inductor->getInductance();
		}
		
		

		//From behind
		if (circuitElement->type() == "VoltageSource") {
			auto connections = circuitElement->getList();
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
		}
		else {
			std::unordered_set<std::string> temp;
			for (auto conn : circuitElement->getList())	{
				if (conn.getNodeNumber() == "voltageMeterConnection") {
					continue;
				}
				else if (temp.find(conn.getNodeNumber()) != temp.end()) {
					continue;
				}
				netlistNodeNumbers += conn.getNodeNumber() + " ";
				temp.insert(conn.getNodeNumber());

			}
			temp.clear();
		}
		
	

		netlistLine += netlistNodeNumbers;

		if (netlistVoltageSourceType != "")
		{
			netlistLine += netlistVoltageSourceType;
		}

		netlistLine += netlistValue;
		
		if (circuitElement->type() == "VoltageSource")
		{
			voltageSourceType = "";
		}


		//Here i send the instance Lines to NGSpice dll
		ngSpice_Command(const_cast<char*>(netlistLine.c_str()));
		if (modelNetlistLine != "circbyline ")
		{
			ngSpice_Command(const_cast<char*>(modelNetlistLine.c_str()));
		}
	}

	//After i got the TitleLine and the elements which represent my circuit I check which simulation was chosen and create the simlationLine
	
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


	//And now i send it to NGSpice in the right order
	
	//Now i create for every Current Meter a resistor with Zero Ohm to measure the current through it
	std::vector<std::string> nameOfRShunts;
	for (auto nodes : nodesOfCurrentMeter)
	{
		std::string nodeString = "";
		for (auto m_node : nodes)
		{
			nodeString += m_node + " ";
		}
		std::ostringstream oss;
		std::string name = "Rshunt" + Numbers::RshunNumbers++;
		nameOfRShunts.push_back(name);
		oss << "circbyline " << name << " " << nodeString << "0";
		std::string temp = oss.str();
		ngSpice_Command(const_cast<char*>( temp.c_str()));
	}
	

	//Here are my Simulation properties which i send to NGSpice
	ngSpice_Command(const_cast<char*>(simulationLine.c_str()));

	//Now i will do a loop through the nodes of the voltageMeter to get the potential diffirence with probe
	for (auto nodes : nodesOfVoltageMeter)
	{
		std::string probeLine = "circbyline .probe vd" + nodes ; //That is the right syntax
		ngSpice_Command(const_cast<char*>(probeLine.c_str()));
	}
	
	// Here i create a probe for every Shunt resistor
	for (auto name : nameOfRShunts)
	{
		std::ostringstream oss;
		oss << "circbyline .probe I(" << name << ")";
		std::string probeLine = oss.str();
		ngSpice_Command(const_cast<char*>(probeLine.c_str()));
	}

	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
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
	 Numbers::RshunNumbers = 0;

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



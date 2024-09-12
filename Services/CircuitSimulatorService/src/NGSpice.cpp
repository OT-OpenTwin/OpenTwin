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
#include "EntityBlockCircuitGND.h"

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
	auto elements = this->getMapOfCircuits().find(name)->second.getMapOfElements();
	for (auto element : elements) 
	{	
		delete element.second;
		element.second = nullptr;
	}
	this->getMapOfCircuits().find(name)->second.getMapOfEntityBlcks().clear();
	this->getMapOfCircuits().clear();
	this->connectionNodeNumbers.clear();
	this->elementCounters.clear();
	this->customNameToNetlistNameMap.clear();
	Numbers::nodeNumber = 1;
	SimulationResults::getInstance()->getResultMap().clear();
}


void NGSpice::connectionAlgorithmWithGNDElement(std::string startingElement,int counter,ot::UID startingElementUID,ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	counter++;

	// First get all informations that needed
	auto circuitMap = Application::instance()->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = allEntitiesByBlockID.at(elementUID);
	auto connections = element->getAllConnections();
	
	//Check if Element already exists
	if (checkIfElementOrConnectionVisited(visitedElements,elementUID))
	{
		return;
	}
	
	for (auto connection : connections)
	{
		Connection myConn = createConnection(allConnectionEntities, connection);

		//First i check if the connection is connected to GND If yes then i state it with node number 0 
		//I check now if the netlist has gnd Element, if yes i do the whole generation with assuming the GND Element
		

		// Here i check if connection already exists
		if (checkIfElementOrConnectionVisited(visitedElements, connection))
		{
			continue;
		}

		if (checkIfConnectionIsConnectedToGND(myConn.getOriginConnectable()) ||
			checkIfConnectionIsConnectedToGND(myConn.getDestConnectable())) {

			auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
			}
			else {
				connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
				if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
					myConn.setNodeNumber(connectionWithNodeNumber->second);
				}
				else {
					myConn.setNodeNumber("0");
					connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestConnectable() }] = myConn.getNodeNumber();
					connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				}
			}

		}
		else
		{
			auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
			}
			else {
				connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
				if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
					myConn.setNodeNumber(connectionWithNodeNumber->second);
				}
				else {
					myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
					connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestConnectable() }] = myConn.getNodeNumber();
					connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				}
			}
		}
		
		it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		it->second.addConnection(myConn.getDestConnectable(), myConn.getDestinationUid(), myConn);

		// Recursive call to explore the next element
		ot::UID nextElementUID;
		if (myConn.getOriginUid() == elementUID) {
			nextElementUID = myConn.getDestinationUid();
		}
		else {
			nextElementUID = myConn.getOriginUid();
		}
		connectionAlgorithmWithGNDElement(startingElement,counter,startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
	}
}

void NGSpice::connectionAlgorithmWithGNDVoltageSource(std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	counter++;

	// First get all informations that needed
	auto circuitMap = Application::instance()->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = allEntitiesByBlockID.at(elementUID);
	auto connections = element->getAllConnections();

	//Check if Element already exists
	if (checkIfElementOrConnectionVisited(visitedElements, elementUID))
	{
		return;
	}

	for (auto connection : connections) {
		Connection myConn = createConnection(allConnectionEntities, connection);

		// As i always start with the voltageSource i want to start at the positivePole first so i skip the connection on the negativePole for the voltageSource 
		// And i dont want to execute the above if Condition so i constructed a counter for this that only for the voltageSource it is relevant

		if (counter == 1 && elementUID == startingElementUID) {
			if (checkIfConnectionIsConnectedToGndVoltageSource(myConn.getOriginConnectable(), startingElementUID, myConn.getOriginUid()) ||
				checkIfConnectionIsConnectedToGndVoltageSource(myConn.getDestConnectable(), startingElementUID, myConn.getDestinationUid())) {
				continue;
			}
		}

		// Here i check if connection already exists
		if (checkIfElementOrConnectionVisited(visitedElements, connection))
		{
			continue;
		}


		if (checkIfConnectionIsConnectedToGndVoltageSource(myConn.getOriginConnectable(), startingElementUID, myConn.getOriginUid()) ||
			checkIfConnectionIsConnectedToGndVoltageSource(myConn.getDestConnectable(), startingElementUID, myConn.getDestinationUid())) {

			auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
			}
			else {
				connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
				if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
					myConn.setNodeNumber(connectionWithNodeNumber->second);
				}
				else {
					myConn.setNodeNumber("0");
					connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestConnectable() }] = myConn.getNodeNumber();
					connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				}
			}

		}
		else
		{
			auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
			}
			else {
				connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
				if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
					myConn.setNodeNumber(connectionWithNodeNumber->second);
				}
				else {
					myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
					connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestConnectable() }] = myConn.getNodeNumber();
					connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				}
			}
		}

		it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		it->second.addConnection(myConn.getDestConnectable(), myConn.getDestinationUid(), myConn);

		// Recursive call to explore the next element
		ot::UID nextElementUID;
		if (myConn.getOriginUid() == elementUID) {
			nextElementUID = myConn.getDestinationUid();
		}
		else {
			nextElementUID = myConn.getOriginUid();
		}

		connectionAlgorithmWithGNDVoltageSource(startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
	}

}

void NGSpice::connectionAlgorithmWithoutGND(ot::UID elementUID,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	// First get all informations that needed
	auto circuitMap = Application::instance()->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = allEntitiesByBlockID.at(elementUID);
	auto connections = element->getAllConnections();

	//Check if Element already exists
	if (checkIfElementOrConnectionVisited(visitedElements, elementUID))
	{
		return;
	}


	for (auto connection : connections)	{
		Connection myConn = createConnection(allConnectionEntities, connection);

		// Here i check if connection already exists
		if (checkIfElementOrConnectionVisited(visitedElements, connection))
		{
			continue;
		}

		auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestConnectable() });
		if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
			myConn.setNodeNumber(connectionWithNodeNumber->second);
		}
		else {
			connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
			}
			else {
				myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
			}
		}

		it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		it->second.addConnection(myConn.getDestConnectable(), myConn.getDestinationUid(), myConn);

		// Recursive call to explore the next element
		ot::UID nextElementUID;
		if (myConn.getOriginUid() == elementUID) {
			nextElementUID = myConn.getDestinationUid();
		}
		else {
			nextElementUID = myConn.getOriginUid();
		}
		connectionAlgorithmWithoutGND(nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
	}

}

bool NGSpice::checkIfElementOrConnectionVisited(std::set<ot::UID>& visitedElements, ot::UID elementOrConnectionUID)
{
	if (visitedElements.find(elementOrConnectionUID) != visitedElements.end()) {
		return true; // Already visited this element, avoid infinite loop
	}
	else
	{
		visitedElements.insert(elementOrConnectionUID); // Mark this element as visited
		return false;
	}
	
}

Connection NGSpice::createConnection(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,ot::UID connection)
{
	std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(connection);
	ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();
	Connection myConn(connectionCfg);
	return myConn;
}

bool NGSpice::checkIfConnectionIsConnectedToGND(std::string pole)
{
	if ( pole == m_gndPole)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

bool NGSpice::checkIfConnectionIsConnectedToGndVoltageSource(std::string pole, ot::UID voltageSourceUID, ot::UID elementUID)
{
	if (elementUID == voltageSourceUID && pole == "negativePole")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool NGSpice::checkIfConnectionIsConnectedToVoltageMeter(std::string blockTitle)
{
	if (blockTitle != m_voltageMeterTitle)
	{
		return true;
	}
	else
	{
		return false;
	}
	
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
			
			voltageSource->setCustomName(myElement->getNameOnly());
			voltageSource->setNetlistName(assignElementID("V"));

			//Add customName and netlistName to Map
			if (!addToCustomNameToNetlistMap(voltageSource->getCustomName(), voltageSource->getNetlistName()))
			{
				OT_LOG_E("customName and netlistName could not be added to map");
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
			
			resistor->setCustomName(myElement->getNameOnly());
			resistor->setNetlistName(assignElementID("R"));

			//Add customName and netlistName to Map
			if (!addToCustomNameToNetlistMap(resistor->getCustomName(), resistor->getNetlistName()))
			{
				OT_LOG_E("customName and netlistName could not be added to map");
			}

			ot::UID uid = resistor->getUID();
			auto resistor_p = resistor.release();
			it->second.addElement(uid, resistor_p);
		}
		else if (blockEntity->getBlockTitle() == "Diode")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitDiode*>(blockEntity.get());
			auto diode = std::make_unique<Diode>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);
			
			diode->setCustomName(myElement->getNameOnly());
			diode->setNetlistName(assignElementID("D"));

			//Add customName and netlistName to Map
			if (!addToCustomNameToNetlistMap(diode->getCustomName(), diode->getNetlistName()))
			{
				OT_LOG_E("customName and netlistName could not be added to map");
			}

			ot::UID uid = diode->getUID();
			auto diode_p = diode.release();
			it->second.addElement(uid, diode_p);
		}
		else if (blockEntity->getBlockTitle() == "Capacitor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitCapacitor*>(blockEntity.get());
			auto capacitor = std::make_unique<Capacitor>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);
			
			capacitor->setCustomName(myElement->getNameOnly());
			capacitor->setNetlistName(assignElementID("C"));

			//Add customName and netlistName to Map
			if (!addToCustomNameToNetlistMap(capacitor->getCustomName(), capacitor->getNetlistName()))
			{
				OT_LOG_E("customName and netlistName could not be added to map");
			}

			ot::UID uid = capacitor->getUID();
			auto capacitor_p = capacitor.release();
			it->second.addElement(uid, capacitor_p);
		}
		else if (blockEntity->getBlockTitle() == "Inductor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitInductor*>(blockEntity.get());
			auto inductor = std::make_unique<Inductor>(myElement->getElementType(), myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);
			
			inductor->setCustomName(myElement->getNameOnly());
			inductor->setNetlistName(assignElementID("L"));

			//Add customName and netlistName to Map
			if (!addToCustomNameToNetlistMap(inductor->getCustomName(), inductor->getNetlistName()))
			{
				OT_LOG_E("customName and netlistName could not be added to map");
			}

			ot::UID uid = inductor->getUID();
			auto inductor_p = inductor.release();
			it->second.addElement(uid, inductor_p);
		}
		else if (blockEntity->getBlockTitle() == "Voltage Meter") {
			auto myElement = dynamic_cast<EntityBlockCircuitVoltageMeter*>(blockEntity.get());
			auto voltMeter = std::make_unique<VoltageMeter>( myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);
			
			voltMeter->setCustomName(myElement->getNameOnly());
			voltMeter->setNetlistName(assignElementID("VM"));

			ot::UID uid = voltMeter->getUID();
			auto voltMeter_p = voltMeter.release();
			it->second.addElement(uid, voltMeter_p);
		}
		else if (blockEntity->getBlockTitle() == "Current Meter") {
			auto myElement = dynamic_cast<EntityBlockCircuitCurrentMeter*>(blockEntity.get());
			auto currentMeter = std::make_unique<CurrentMeter>(myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			currentMeter->setCustomName(myElement->getNameOnly());
			currentMeter->setNetlistName(assignElementID("CM"));
			ot::UID uid = currentMeter->getUID();
			auto currentMeter_p = currentMeter.release();
			it->second.addElement(uid, currentMeter_p);
		}
		/*else if (blockEntity->getClassName() == "EntityBlockCircuitGND") {
			auto myElement = dynamic_cast<EntityBlockCircuitGND*>(blockEntity.get());
			auto currentMeter = std::make_unique<CurrentMeter>(myElement->getBlockTitle(), editorname, myElement->getEntityID(), notInitialized);

			currentMeter->setNetlistName(myElement->getNameOnly());

			ot::UID uid = currentMeter->getUID();
			auto currentMeter_p = currentMeter.release();
			it->second.addElement(uid, currentMeter_p);
		}*/
	
	}

	

	// I want to be able to have always the same structure and naming of nodeNumbers in the circuit in all cases the user might build the circuit.
	// Means that the user can start with every element to drop in the scene and start with any connection he want and this will not influence the results of the simulation
	// and the coordination and of the nodenumbers and elements
	// I want always a flow from positive node to negative node 
	// I first start with the voltageSource and give it the connection id = 0 then i will go to the next element at the connection and to the opposite connetor and give it the 
	// next nodenumber. I will take use of traversing graphs with my code of checking parallel connections. I just need a good understandable structure

	// First I get all the VoltageSources of the Circuit
	auto vectorGND = it->second.getMapOfEntityBlcks().find("EntityBlockCircuitGND");
	if (vectorGND != it->second.getMapOfEntityBlcks().end()) {
		std::set<ot::UID> visitedElements; // Initialize visited set

		for (auto GNDElement : vectorGND->second) {
			ot::UID elementUID = GNDElement->getEntityID();
			int counter = 0;
			connectionAlgorithmWithGNDElement( GNDElement->getClassName(), counter, elementUID, elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
		}

	}
	else
	{
		auto vectorVoltageSource = it->second.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageSource");
		if (vectorVoltageSource == it->second.getMapOfEntityBlcks().end())
		{
			OT_LOG_E("No VoltageSource found at connection Algorithm");
		}
		std::set<ot::UID> visitedElements; // Initialize visited set

		for (auto voltageSource : vectorVoltageSource->second) {
			auto temp = dynamic_cast<EntityBlockCircuitVoltageSource*>(voltageSource.get());

			ot::UID elementUID = voltageSource->getEntityID();
			if (temp->getGND() == false)
			{
				connectionAlgorithmWithoutGND(elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
			}
			else
			{
				int counter = 0;
				connectionAlgorithmWithGNDVoltageSource(voltageSource->getClassName(), counter, elementUID, elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
			}
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

	std::vector<std::vector<std::string>> nodesOfVoltageMeter;
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
				
				/*getNodeNumbersOfMeters(editorname, allConnectionEntities, allEntitiesByBlockID,nodesOfVoltageMeter);*/

				std::vector<std::string> nodeNumbers;
				std::unordered_set<std::string> temp;
				auto connections = circuitElement->getList();

				// Verarbeite zuerst die positivePole Verbindung
				if (connections.find("positivePole") != connections.end())
				{
					auto& positiveConn = connections.at("positivePole");

					if (temp.find(positiveConn.getNodeNumber()) == temp.end())
					{
						temp.insert(positiveConn.getNodeNumber());
						nodeNumbers.push_back(positiveConn.getNodeNumber());
					}
				}

				// Verarbeite danach die negativePole Verbindung
				if (connections.find("negativePole") != connections.end())
				{
					auto& negativeConn = connections.at("negativePole");

					if (temp.find(negativeConn.getNodeNumber()) == temp.end())
					{
						temp.insert(negativeConn.getNodeNumber());
						nodeNumbers.push_back(negativeConn.getNodeNumber());
					}
				}
				nodesOfVoltageMeter.push_back(nodeNumbers);
				// I am doing a continue here becaue i dont want to generate an instance Line for this element
				continue;		
		}
		else if (circuitElement->type() == "CurrentMeter")
		{
			
				std::vector<std::string> nodeNumbers;
				std::unordered_set<std::string> temp;
				auto connections = circuitElement->getList();

				// Verarbeite zuerst die positivePole Verbindung
				if (connections.find("positivePole") != connections.end())
				{
					auto& positiveConn = connections.at("positivePole");

					if (temp.find(positiveConn.getNodeNumber()) == temp.end())
					{
						temp.insert(positiveConn.getNodeNumber());
						nodeNumbers.push_back(positiveConn.getNodeNumber());
					}
				}

				// Verarbeite danach die negativePole Verbindung
				if (connections.find("negativePole") != connections.end())
				{
					auto& negativeConn = connections.at("negativePole");

					if (temp.find(negativeConn.getNodeNumber()) == temp.end())
					{
						temp.insert(negativeConn.getNodeNumber());
						nodeNumbers.push_back(negativeConn.getNodeNumber());
					}
				}

				nodesOfCurrentMeter.push_back(nodeNumbers);

				// Hier �berspringen wir die Instanzlinien-Generierung f�r dieses Element
				continue;
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
		
		

		//insert connectionNodeNumbers into string
		
		std::unordered_set<std::string> temp;
		auto connections = circuitElement->getList();
			
			if (connections.find("positivePole") != connections.end()) {
				auto& positiveConn = connections.at("positivePole");

				if (positiveConn.getNodeNumber() != "voltageMeterConnection" && temp.find(positiveConn.getNodeNumber()) == temp.end()) {
					netlistNodeNumbers += positiveConn.getNodeNumber() + " ";
					temp.insert(positiveConn.getNodeNumber());
				}
			}

			if (connections.find("negativePole") != connections.end()) {
				auto& negativeConn = connections.at("negativePole");

				if (negativeConn.getNodeNumber() != "voltageMeterConnection" && temp.find(negativeConn.getNodeNumber()) == temp.end()) {
					netlistNodeNumbers += negativeConn.getNodeNumber() + " ";
					temp.insert(negativeConn.getNodeNumber());
				}
			}


		
		temp.clear();
		
		
		
	

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
	int rshuntCounter = 1; // Initialisiere den Z�hler

	for (auto nodes : nodesOfCurrentMeter)
	{
		std::string nodeString = "";
		for (auto m_node : nodes)
		{
			nodeString += m_node + " ";
		}

		std::ostringstream oss;
		std::string name = "Rshunt" + std::to_string(rshuntCounter++); // Erh�he den Z�hler nach jedem Schritt
		nameOfRShunts.push_back(name);

		oss << "circbyline " << name << " " << nodeString << "0";
		std::string temp = oss.str();
		ngSpice_Command(const_cast<char*>(temp.c_str()));
	}
	

	//Here are my Simulation properties which i send to NGSpice
	ngSpice_Command(const_cast<char*>(simulationLine.c_str()));

	//Now i will do a loop through the nodes of the voltageMeter to get the potential diffirence with probe
	for (auto nodes : nodesOfVoltageMeter)
	{
		std::string nodeString = "("; // Beginne den String mit einer �ffnenden Klammer
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			nodeString += nodes[i];
			if (i < nodes.size() - 1)
			{
				nodeString += ","; // F�ge ein Komma und ein Leerzeichen zwischen den Nodes hinzu
			}
		}
		nodeString += ")"; // Schlie�e den String mit einer schlie�enden Klammer ab

		std::ostringstream oss;
		oss << "circbyline .probe vd" << nodeString;
		std::string probeLine = oss.str();
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
	//ngSpice_Command(const_cast<char*>("circbyline unset askquit"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	//ngSpice_Command(const_cast<char*>("circbyline quit"));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));
	//ngSpice_Command(const_cast<char*>("unset askquit"));
	//ngSpice_Command(const_cast<char*>("quit 0"));
	

	return "success";
	
}

std::string NGSpice::generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname)
{
	EntityPropertiesEntityList* elementProperty = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Element"));
	
	std::string element = Application::instance()->extractStringAfterDelimiter(elementProperty->getValueName(), '/', 2);
	std::string netlistName = getNetlistNameOfMap(element);
	
	

	std::string simulationLine="";
	std::string type = ".dc";
	EntityPropertiesString* from = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("From"));
	EntityPropertiesString* to = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("To"));
	EntityPropertiesString* step = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Step"));
	simulationLine = type + " " + netlistName + " " + from->getValue() + " " + to->getValue() + " " + step->getValue();
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

bool NGSpice::addToCustomNameToNetlistMap(const std::string& customName, const std::string& netlistName) {
	
	if (customNameToNetlistNameMap.find(customName) != customNameToNetlistNameMap.end()) {
		OT_LOG_E("The custom name:" + customName + "already exists");
		return false;
	}

	customNameToNetlistNameMap[customName] = netlistName;
	return true;
}

std::string NGSpice::assignElementID(const std::string& elementType) {
	elementCounters[elementType]++;
	return elementType + std::to_string(elementCounters[elementType]);
}

std::string NGSpice::getNetlistNameOfMap(const std::string& customName) const
{
	auto it = customNameToNetlistNameMap.find(customName);
	if (it != customNameToNetlistNameMap.end()) {
		return it->second; 
	}
	else {
		OT_LOG_E("Netlist name in customNameToNetlistNameMap not found!");
		return "failed"; 
	}
}

//NGSpice* NGSpice::getInstance() {
//	if (!instance) {
//		instance = new NGSpice();
//	}
//	return instance;
//}



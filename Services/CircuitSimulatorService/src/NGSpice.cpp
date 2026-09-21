// @otlicense
// File: NGSpice.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
#include "CircuitElements/TransmissionLine.h"
#include "SimulationResults.h"
#include "BlockEntityHandler.h"
#include "SimulationStrategy.h"

//Open Twin Header
#include "OTBlockEntities//EntityBlockConnection.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitGND.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitDiode.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitInductor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitResistor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitCapacitor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitVoltageMeter.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitCurrentMeter.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitVoltageSource.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitTransmissionLine.h"

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
#include <cmath>
#include <stdexcept>
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
	this->m_elementNamingRegistry.reset();
	Numbers::nodeNumber = 1;
	//SimulationResults::getInstance()->getResultMap().clear();
	/*ngSpice_Command(const_cast<char*>("show"));*/
	//ngSpice_Command(const_cast<char*>("reset"));
	
	//ngSpice_Init(MySendCharFunction, MySendStat, MyControlledExit, MySendDataFunction, MySendInitDataFunction, nullptr, nullptr);
}

std::shared_ptr<EntityFileText> NGSpice::getModelEntity(const std::string& _folderName, std::string _modelName) {
	BlockEntityHandler blockHandler;
	std::shared_ptr<EntityFileText> circuitModelEntity = blockHandler.getCircuitModel(_folderName,_modelName);
	
	if (circuitModelEntity != nullptr) {
		return circuitModelEntity;
	}

	OT_LOG_E("No CircuitModelEntity found with name: " + _modelName);
	return nullptr;
}

std::string NGSpice::getCircuitModelType(std::shared_ptr<EntityFileText> _circuitModelEntity) {
	if (_circuitModelEntity != nullptr) {
		auto propertyBase = _circuitModelEntity->getProperties().getProperty("ModelType");
		if (propertyBase) {
			auto circuitModelType = dynamic_cast<EntityPropertiesString*>(propertyBase);
			if (circuitModelType) {
				return circuitModelType->getValue();
			}
		}
		return "";
	}

	OT_LOG_E("No Circuit model type found: EntityFileText is null");
	return "";
}

std::string NGSpice::getCircuitModelText(std::shared_ptr<EntityFileText> _circuitModelEntity) {
	if (_circuitModelEntity != nullptr) {
		const std::vector<char>& data = _circuitModelEntity->getDataEntity()->getData();
		std::string modelText(data.begin(), data.end());
		return modelText;
	}

	OT_LOG_E("No Circuit model type found: " + _circuitModelEntity->getClassName() + " is null");
	return "";

}

std::vector<std::string> NGSpice::convertToCircByLine(const std::string& lines) {
	std::istringstream stream(lines);
	std::string line;
	std::vector<std::string> circLines;

	while (std::getline(stream, line)) {
		if (line.empty()) continue; // skip emtpy line
		circLines.push_back("circbyline " + line);
	}

	return circLines;
}

void NGSpice::connectionAlgorithmWithGNDElement(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string startingElement,int counter,ot::UID startingElementUID,ot::UID elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	counter++;

	// First get all informations that needed
	auto appInstance = Application::instance();
	auto circuitMap = appInstance->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = getEntityBlock(allEntitiesByBlockID, elementUID);
	if (element == nullptr) {
		return;
	}
	//auto connections = element->getAllConnections();
	auto connections = getConnections(_connectionBlockMap, elementUID);
	if (connections.empty()) {
		return;
	}

	//Check if Element already exists
	if (checkIfElementOrConnectionVisited(visitedElements,elementUID))
	{
		return;
	}
	
	for (auto connection : connections)
	{
		Connection myConn = createConnection(allConnectionEntities, connection);
		

		//I always start with GND Element and give the Connection the nodeNumber 0 to ensure that the following connections of the connector get the right node number
		if (counter == 1 && startingElement == "EntityBlockCircuitGND")
		{
			if (checkIfConnectionIsConnectedToGND(myConn.getOriginConnectable()) ||
				checkIfConnectionIsConnectedToGND(myConn.getDestinationConnectable())) {

				myConn.setNodeNumber("0");
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				it->second.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

			}
		}

		// Check the case if the connection is connected to a connector 
		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(),'/',2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID() ||
			appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{
			
			ot::UID nextElementUID;
			if (myConn.getOriginUid() == elementUID) {
				nextElementUID = myConn.getDestinationUid();
			}
			else {
				nextElementUID = myConn.getOriginUid();
			}

			// Now i deal with the connectors means that i give all connections of the connector the same nodeNumber and search if the connector is connected to another connector
			//Meaning i go through and give all in row placed connectors the same nodeNumbers
			handleWithConnectors(_connectionBlockMap, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

			
			connectionAlgorithmWithGNDElement(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
			
		}
		else
		{

			// Here i check if connection already exists
			if (checkIfElementOrConnectionVisited(visitedElements, connection))
			{
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == elementUID) {
					nextElementUID = myConn.getDestinationUid();
				}
				else {
					nextElementUID = myConn.getOriginUid();
				}

				connectionAlgorithmWithGNDElement(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

			}
			else {
				setNodeNumbers(myConn);

				it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				it->second.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

				// Recursive call to explore the next element
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == elementUID) {
					nextElementUID = myConn.getDestinationUid();
				}
				else {
					nextElementUID = myConn.getOriginUid();
				}
				connectionAlgorithmWithGNDElement(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
			}

			
		}
		
	}

	return;
}

void NGSpice::connectionAlgorithmWithGNDVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	counter++;

	// First get all informations that needed
	auto appInstance = Application::instance();
	auto circuitMap = appInstance->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = getEntityBlock(allEntitiesByBlockID, elementUID);
	if (element == nullptr) {
		return;
	}
	//auto connections = element->getAllConnections();
	auto connections = getConnections(_connectionBlockMap, elementUID);
	if (connections.empty()) {
		return;
	}

	//Check if Element already exists
	if (checkIfElementOrConnectionVisited(visitedElements, elementUID))
	{
		return;
	}

	for (auto connection : connections) {
		Connection myConn = createConnection(allConnectionEntities, connection);

		// As i always start with the voltageSource i want to start at the positivePole first so i skip the connection on the negativePole for the voltageSource 
		// And i dont want to execute the above if Condition so i constructed a counter for this that only for the voltageSource it is relevant

			if (counter == 1 && startingElement == "EntityBlockCircuitVoltageSource"){

			if (checkIfConnectionIsConnectedToGndVoltageSource(myConn.getOriginConnectable(), startingElementUID, myConn.getOriginUid()) ||
				checkIfConnectionIsConnectedToGndVoltageSource(myConn.getDestinationConnectable(), startingElementUID, myConn.getDestinationUid())) {

				myConn.setNodeNumber("0");
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				it->second.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);				
			}
		}

		// Check the case if the connection is connected to a connector 
		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID() ||
			appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{

			ot::UID nextElementUID;
			if (myConn.getOriginUid() == elementUID) {
				nextElementUID = myConn.getDestinationUid();
			}
			else {
				nextElementUID = myConn.getOriginUid();
			}

			// Now i deal with the connectors means that i give all connections of the connector the same nodeNumber and search if the connector is connected to another connector
			//Meaning i go through and give all in row placed connectors the same nodeNumbers
			handleWithConnectors(_connectionBlockMap, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);


			connectionAlgorithmWithGNDElement(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

		}
		else
		{

			// Here i check if connection already exists
			if (checkIfElementOrConnectionVisited(visitedElements, connection))
			{
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == elementUID) {
					nextElementUID = myConn.getDestinationUid();
				}
				else {
					nextElementUID = myConn.getOriginUid();
				}

				connectionAlgorithmWithGNDVoltageSource(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

			}
			else {
				setNodeNumbers(myConn);

				it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				it->second.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

				// Recursive call to explore the next element
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == elementUID) {
					nextElementUID = myConn.getDestinationUid();
				}
				else {
					nextElementUID = myConn.getOriginUid();
				}
				connectionAlgorithmWithGNDVoltageSource(_connectionBlockMap, startingElement, counter, startingElementUID, nextElementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

			}
		}
	}

	return;

}

void NGSpice::handleWithConnectors(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	auto appInstance = Application::instance();
	auto circuitMap = appInstance->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = getEntityBlock(allEntitiesByBlockID, elementUID);
	if (element == nullptr) {
		return;
	}
	//auto connections = element->getAllConnections();
	auto connections = getConnections(_connectionBlockMap, elementUID);
	if (connections.empty()) {
		return;
	}

	for (auto connection : connections)
	{
		Connection myConn = createConnection(allConnectionEntities, connection);
		if (checkIfElementOrConnectionVisited(visitedElements, connection))
		{
			continue;
		}

		setNodeNumbers(myConn);

		it->second.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		it->second.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID())
		{
			handleWithConnectors(_connectionBlockMap, myConn.getDestinationUid(), allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
		}
		else if (appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{
			handleWithConnectors(_connectionBlockMap, myConn.getOriginUid(), allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);
		}
	}

	return;
	
}

void NGSpice::setNodeNumbers(Connection& myConn)
{
	if (checkIfConnectionIsConnectedToGND(myConn.getOriginConnectable()) ||
		checkIfConnectionIsConnectedToGND(myConn.getDestinationConnectable()))
	{
		auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestinationConnectable() });
		if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
			myConn.setNodeNumber(connectionWithNodeNumber->second);
			connectionNodeNumbers[{myConn.getOriginUid(), myConn.getOriginConnectable()}] = myConn.getNodeNumber();
		}
		else {
			connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
				connectionNodeNumbers[{myConn.getDestinationUid(), myConn.getDestinationConnectable()}] = myConn.getNodeNumber();
			}
			else {
				myConn.setNodeNumber("0");
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
			}
		}
	}
	else {
		auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestinationConnectable() });
		if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
			myConn.setNodeNumber(connectionWithNodeNumber->second);
			connectionNodeNumbers[{myConn.getOriginUid(), myConn.getOriginConnectable()}] = myConn.getNodeNumber();
		}
		else {
			connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
				connectionNodeNumbers[{myConn.getDestinationUid(), myConn.getDestinationConnectable()}] = myConn.getNodeNumber();
			}
			else {
				myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
			}
		}
	}
}

void NGSpice::setNodeNumbersWithGNDVoltageSource(Connection& myConn,ot::UID startingElementUID)
{
	if (checkIfConnectionIsConnectedToGndVoltageSource(myConn.getOriginConnectable(), startingElementUID, myConn.getOriginUid()) ||
		checkIfConnectionIsConnectedToGndVoltageSource(myConn.getDestinationConnectable(), startingElementUID, myConn.getDestinationUid())) {

		auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestinationConnectable() });
		if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
			myConn.setNodeNumber(connectionWithNodeNumber->second);
			connectionNodeNumbers[{myConn.getOriginUid(), myConn.getOriginConnectable()}] = myConn.getNodeNumber();
		}
		else {
			connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
				connectionNodeNumbers[{myConn.getDestinationUid(), myConn.getDestinationConnectable()}] = myConn.getNodeNumber();
			}
			else {
				myConn.setNodeNumber("0");
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
			}
		}

	}
	else
	{
		auto connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getDestinationUid(), myConn.getDestinationConnectable() });
		if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
			myConn.setNodeNumber(connectionWithNodeNumber->second);
			connectionNodeNumbers[{myConn.getOriginUid(), myConn.getOriginConnectable()}] = myConn.getNodeNumber();
		}
		else {
			connectionWithNodeNumber = connectionNodeNumbers.find({ myConn.getOriginUid(), myConn.getOriginConnectable() });
			if (connectionWithNodeNumber != connectionNodeNumbers.end()) {
				myConn.setNodeNumber(connectionWithNodeNumber->second);
				connectionNodeNumbers[{myConn.getDestinationUid(), myConn.getDestinationConnectable()}] = myConn.getNodeNumber();
			}
			else {
				myConn.setNodeNumber(std::to_string(Numbers::nodeNumber++));
				connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
			}
		}
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

Connection NGSpice::createConnection(std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities,ot::UID connection)
{
	auto it = allConnectionEntities.find(connection);
	if (it != allConnectionEntities.end()) {
		ot::GraphicsConnectionCfg connectionCfg = it->second->getConnectionCfg();
		Connection myConn(connectionCfg);
		return myConn;
	}
	else {
		OT_LOG_E("Connection does not exist - EntityID: " + connection);
		return Connection();
	}
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

void NGSpice::setNodeNumbersOfVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements)
{
	// First get all informations that needed
	auto circuitMap = Application::instance()->getNGSpice().getMapOfCircuits();
	auto it = circuitMap.find(editorname);
	auto element = getEntityBlock(allEntitiesByBlockID, elementUID);
	if (element == nullptr) {
		return;
	}
	//auto connections = element->getAllConnections();
	auto connections = getConnections(_connectionBlockMap, elementUID);
	if (connections.empty()) {
		return;
	}

	std::vector<Connection> connectionsToBeSet;
	for (auto connection : connections)
	{
		Connection myConn = createConnection(allConnectionEntities, connection);
		connectionsToBeSet.push_back(myConn);
	}




	for (auto myConn = connectionsToBeSet.begin(); myConn != connectionsToBeSet.end(); ++myConn)
	{
		if (checkIfConnectionIsConnectedToGndVoltageSource(myConn->getOriginConnectable(), startingElementUID, myConn->getOriginUid()) ||
			checkIfConnectionIsConnectedToGndVoltageSource(myConn->getDestinationConnectable(), startingElementUID, myConn->getDestinationUid()))
		{
			myConn->setNodeNumber("0");
			connectionNodeNumbers[{ myConn->getDestinationUid(), myConn->getDestinationConnectable() }] = myConn->getNodeNumber();
			connectionNodeNumbers[{ myConn->getOriginUid(), myConn->getOriginConnectable() }] = myConn->getNodeNumber();

			it->second.addConnection(myConn->getOriginConnectable(), myConn->getOriginUid(), *myConn);
			it->second.addConnection(myConn->getDestinationConnectable(), myConn->getDestinationUid(), *myConn);
		}
	}



	return;
}



void NGSpice::updateBufferClasses(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{


	auto [it, inserted] = Application::instance()->getNGSpice().getMapOfCircuits().try_emplace(editorname);
	if (inserted)
	{
		it->second.setEditorName(editorname);
		it->second.setId(editorname);
	}

	Circuit& circuit = it->second;

	for (auto& [blockID, blockEntity] : allEntitiesByBlockID)
	{
		
		circuit.addBlockEntity(blockEntity->getClassName(), blockEntity);
		
		CircuitElement* element = CircuitElement::createFromClassName(blockEntity->getClassName());
		if (!element)
		{
			continue;
		}

		auto elementPtr = std::unique_ptr<CircuitElement>(element);
		elementPtr->initFromEntity(blockEntity, editorname);

		std::string netlistName = m_elementNamingRegistry.registerElement(blockEntity->getNameOnly(), elementPtr->getNetlistPrefix());
		elementPtr->setNetlistName(netlistName);
		elementPtr->setCustomName(blockEntity->getNameOnly());

		auto* circuitEntity = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
		if (circuitEntity)
		{
			std::string circuitModel = circuitEntity->getCircuitModel();
			circuitModel = Application::instance()->extractStringAfterDelimiter(circuitModel, '/', 2);
			elementPtr->setModel(circuitModel);
			elementPtr->setFolderName(circuitEntity->getFolderName());
		}

		ot::UID uid = blockEntity->getEntityID();
		auto element_p = elementPtr.release();
		circuit.addElement(uid, element_p);

	}

	

// I want to be able to have always the same structure and naming of nodeNumbers in the circuit in all cases the user might build the circuit.
// Means that the user can start with every element to drop in the scene and start with any connection he want and this will not influence the results of the simulation
// and the coordination and of the nodenumbers and elements
// I want always a flow from positive node to negative node 
// I first start with the voltageSource and give it the connection id = 0 then i will go to the next element at the connection and to the opposite connetor
// and give it the  next nodenumber. I will take use of traversing graphs with my code of checking parallel connections. I just need a good understandable structure


// First I get all the VoltageSources of the Circuit
	auto vectorGND = circuit.getMapOfEntityBlcks().find("EntityBlockCircuitGND");
	if (vectorGND != circuit.getMapOfEntityBlcks().end()) {
		std::set<ot::UID> visitedElements; // Initialize visited set

		for (auto GNDElement : vectorGND->second) {
			ot::UID elementUID = GNDElement->getEntityID();
			int counter = 0;
			connectionAlgorithmWithGNDElement(_connectionBlockMap, GNDElement->getClassName(), counter, elementUID, elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

		}

	}
	else
	{
		auto vectorVoltageSource = circuit.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageSource");
		if (vectorVoltageSource == circuit.getMapOfEntityBlcks().end())
		{
			OT_LOG_E("No VoltageSource found at connection Algorithm");
			return;
		}
		std::set<ot::UID> visitedElements; // Initialize visited set

		for (auto voltageSource : vectorVoltageSource->second) {
			ot::UID elementUID = voltageSource->getEntityID();	
			int counter = 0;

			//First set the GND connections of the VoltageSource meaning that i first want to find the connection with 0 and set it to it
			setNodeNumbersOfVoltageSource(_connectionBlockMap, voltageSource->getClassName(), counter, elementUID, elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

			// now i go thorugh all the other connections
			connectionAlgorithmWithGNDVoltageSource(_connectionBlockMap, voltageSource->getClassName(), counter, elementUID, elementUID, allConnectionEntities, allEntitiesByBlockID, editorname, visitedElements);

		}
	}
	
	return;
}



std::list<std::string> NGSpice::generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	
	
	//Here i first create the Title of the Netlist and send it to NGSpice
	std::list<std::string> _netlist;
	std::string TitleLine = "circbyline *Test";
	_netlist.push_back(const_cast<char*>(TitleLine.c_str()));




	//As next i create the Circuit Element Netlist Lines by getting the information out of the BufferClasses 

	std::vector<std::vector<std::string>> nodesOfVoltageMeter;
	std::vector<std::vector<std::string>> nodesOfCurrentMeter;
	std::vector<std::string> namesOfCurrentMeter;
	std::vector<std::string> nameOfRShunts;
	std::unordered_set<std::string> usedModels;
	int rshuntCounter = 1; // Initialize the counter
	// I need to get the type of Simulation to set then the voltage source if its ac dc or tran
	EntityPropertiesSelection* simulationTypeProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Simulation Type"));
	assert(simulationTypeProperty != nullptr);
	std::string simulationType = simulationTypeProperty->getValue();

	// Create the appropriate simulation strategy based on the simulation type
	auto strategy = createSimulationStrategy(simulationType);
	if (!strategy)
	{
		OT_LOG_E("Failed to create simulation strategy for type: " + simulationType);
		_netlist.clear();
		return _netlist;
	}

	auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	 
	for (const auto& mapOfElements : it->second.getMapOfElements())
	{
		auto circuitElement = mapOfElements.second;

		std::string netlistElementName = "";
		std::string netlistLine="circbyline ";
		std::string netlistValue = "";
		std::string netlistNodeNumbers;
		std::string netlistVoltageSourceType="";
		std::vector<std::string> modelNetlistLine;
		std::string modelType = "";
		
		//Check if circuitElement has a model
		if (circuitElement->getModel() != "failed") {
			// First we get the entity
			std::shared_ptr<EntityFileText> circuitModelEntity = getModelEntity(circuitElement->getFolderName(),circuitElement->getModel());

			// Now we need to get the model type (subcircuit or model)
			modelType = getCircuitModelType(circuitModelEntity);
			
			auto it = usedModels.find(circuitElement->getModel());
			if (it == usedModels.end()) {
				// As last step we need the model text
				modelNetlistLine = convertToCircByLine(getCircuitModelText(circuitModelEntity));
				usedModels.insert(circuitElement->getModel());
			}
			
		}
		else {
			circuitElement->setModel("");
		}



		if (circuitElement->type() == "VoltageSource")
		{
			VoltageSource* voltagesource = dynamic_cast<VoltageSource*>(circuitElement);
			netlistVoltageSourceType = strategy->getVoltageSourceNetlistType(voltagesource);

			if (modelType.empty()) {
				netlistElementName = voltagesource->getNetlistName();
				netlistValue = voltagesource->getValue();
			}
			else {
				if (modelType == m_subcktType) {
					netlistElementName = m_elementNamingRegistry.generateNextId("X");
				}
				else {
					netlistElementName = voltagesource->getNetlistName();
				}

				netlistValue = voltagesource->getModel();
			}

			netlistLine += netlistElementName + " ";
		}
		else if (circuitElement->type() == "Resistor")
		{
			Resistor* resistor = dynamic_cast<Resistor*>(circuitElement);

			
			if (modelType.empty()) {
				netlistElementName = resistor->getNetlistName();
				netlistValue = resistor->getResistance();
			}
			else {
				if (modelType == m_subcktType) {
					netlistElementName = m_elementNamingRegistry.generateNextId("X");
				}
				else {
					netlistElementName = resistor->getNetlistName();
				}

				netlistValue = resistor->getModel();
			}

			netlistLine += netlistElementName + " ";
		}
		else if (circuitElement->type() == "Diode")
		{
			Diode* diode = dynamic_cast<Diode*>(circuitElement);

			// Check if .model or .subckt
			if (modelType == m_subcktType) {
				netlistElementName = m_elementNamingRegistry.generateNextId("X");
			}
			else {
				netlistElementName = diode->getNetlistName();
			}

			netlistValue = diode->getValue() + " " + diode->getModel();
			netlistLine += netlistElementName + " ";
			
		}
		else if (circuitElement->type() == "VoltageMeter")
		{
				
				/*getNodeNumbersOfMeters(editorname, allConnectionEntities, allEntitiesByBlockID,nodesOfVoltageMeter);*/

				std::vector<std::string> nodeNumbers;
				std::unordered_set<std::string> temp;
				auto connections = circuitElement->getList();

				// First process the positivePole connection
				if (connections.find("positivePole") != connections.end())
				{
					auto& positiveConn = connections.at("positivePole");

					if (temp.find(positiveConn.getNodeNumber()) == temp.end())
					{
						temp.insert(positiveConn.getNodeNumber());
						nodeNumbers.push_back(positiveConn.getNodeNumber());
					}
				}

				// process the negative connection
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
				// I am doing a continue here because i dont want to generate an instance Line for this element
				continue;		
		}
		else if (circuitElement->type() == "CurrentMeter")
		{
				CurrentMeter* currentMeter = dynamic_cast<CurrentMeter*>(circuitElement);
				namesOfCurrentMeter.push_back(currentMeter->getCustomName());
				std::string name = "Rshunt" + std::to_string(rshuntCounter++); // Increase counter for next Rshunt
				nameOfRShunts.push_back(name);
				m_elementNamingRegistry.registerMapping(currentMeter->getCustomName(), ElementNamingRegistry::toLowercase(name));
				std::vector<std::string> nodeNumbers;
				std::unordered_set<std::string> temp;
				auto connections = circuitElement->getList();

				// First process the positivePole connection
				if (connections.find("positivePole") != connections.end())
				{
					auto& positiveConn = connections.at("positivePole");

					if (temp.find(positiveConn.getNodeNumber()) == temp.end())
					{
						temp.insert(positiveConn.getNodeNumber());
						nodeNumbers.push_back(positiveConn.getNodeNumber());
					}
				}

				// process the negative connection
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

				// Now we skip the instance line generation
				continue;
		}
		else if (circuitElement->type() == "Capacitor")
		{
			Capacitor* capacitor = dynamic_cast<Capacitor*>(circuitElement);

			
			if (modelType.empty()) {
				netlistElementName = capacitor->getNetlistName();
				netlistValue = capacitor->getCapacity();
			}
			else {
				if (modelType == m_subcktType) {
					m_elementNamingRegistry.generateNextId("X");
				}
				else {
					netlistElementName = capacitor->getNetlistName();
				}

				netlistValue = capacitor->getModel();
			}
			netlistLine += netlistElementName + " ";
		}
		else if (circuitElement->type() == "Inductor")
		{
			Inductor* inductor = dynamic_cast<Inductor*>(circuitElement);

			if (modelType.empty()) {
				netlistElementName = inductor->getNetlistName();
				netlistValue = inductor->getInductance();
			}
			else {
				if (modelType == m_subcktType) {
					m_elementNamingRegistry.generateNextId("X");
				}
				else {
					netlistElementName = inductor->getNetlistName();
				}

				netlistValue = inductor->getModel();
			}
			netlistLine += netlistElementName + " ";
		}
		else if (circuitElement->type() == "TransmissionLine") 
		{
			TransmissionLine* transmissionLine = dynamic_cast<TransmissionLine*>(circuitElement);

			if (modelType.empty()) {
				netlistElementName = transmissionLine->getNetlistName();
				netlistValue = transmissionLine->getImpedance() + " " + transmissionLine->getTransmissionDelay();
			}
			else {
				if (modelType == m_subcktType) {
					m_elementNamingRegistry.generateNextId("X");
				}
				else {
					netlistElementName = transmissionLine->getNetlistName();
				}

				netlistValue = transmissionLine->getModel();
			}
			netlistLine += netlistElementName + " ";
		}
		
		
		//insert connectionNodeNumbers into string
		
		std::unordered_set<std::string> temp;
		auto connections = circuitElement->getList();

		if (circuitElement->type() == "TransmissionLine") 
		{
			//Here check positive1/neg1 and pos2/neg2 need some work !!!
			if (connections.find("PositivePole1") != connections.end()) {
				auto& positiveConn = connections.at("PositivePole1");

				if (positiveConn.getNodeNumber() != "voltageMeterConnection" ) {
					netlistNodeNumbers += positiveConn.getNodeNumber() + " ";
					temp.insert(positiveConn.getNodeNumber());
				}
			}

			if (connections.find("PositivePole2") != connections.end()) {
				auto& positiveConn = connections.at("PositivePole2");

				if (positiveConn.getNodeNumber() != "voltageMeterConnection") {
					netlistNodeNumbers += positiveConn.getNodeNumber() + " ";
					temp.insert(positiveConn.getNodeNumber());
				}
			}

			if (connections.find("NegativePole1") != connections.end()) {
				auto& negativeConn = connections.at("NegativePole1");

				if (negativeConn.getNodeNumber() != "voltageMeterConnection") {
					netlistNodeNumbers += negativeConn.getNodeNumber() + " ";
					temp.insert(negativeConn.getNodeNumber());
				}
			}
			
			if (connections.find("NegativePole2") != connections.end()) {
				auto& negativeConn = connections.at("NegativePole2");

				if (negativeConn.getNodeNumber() != "voltageMeterConnection") {
					netlistNodeNumbers += negativeConn.getNodeNumber() + " ";
					temp.insert(negativeConn.getNodeNumber());
				}
			}
		}
		else {
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
		}
			
	


		
		temp.clear();
		
		
		
	

		netlistLine += netlistNodeNumbers;

		if (netlistVoltageSourceType != "")
		{
			netlistLine += netlistVoltageSourceType;
		}

		netlistLine += netlistValue;


		//Here i put the netlist instance line into my _netlist
		_netlist.push_back(const_cast<char*>(netlistLine.c_str()));
		
		// If we have a model included into the element
		if (!modelNetlistLine.empty())
		{
			for (auto line : modelNetlistLine){
				_netlist.push_back(const_cast<char*>(line.c_str()));
			}
			
			
		}
	}

	//After i got the TitleLine and the elements which represent my circuit I check which simulation was chosen and create the simlationLine
	
	std::string simulationLine = strategy->generateSimulationLine(solverEntity, m_elementNamingRegistry);

	if (simulationLine == "failed")
	{
	
		OT_LOG_E("Failed at creating Simulation Line");
		_netlist.clear();
		return _netlist;
	}
	simulationLine = "circbyline " + simulationLine;


	//And now i send it to NGSpice in the right order
	//Now i create for every Current Meter a resistor with Zero Ohm to measure the current through it

	std::vector<std::string> tempVecOfShunts;
	for (auto nodes : nodesOfCurrentMeter)
	{
		std::string nodeString = "";
		for (auto m_node : nodes)
		{
			nodeString += m_node + " ";
		}

		std::ostringstream oss;
		std::string name = nameOfRShunts.front();
		oss << "circbyline " << name << " " << nodeString << "0";
		tempVecOfShunts.push_back(name);
		nameOfRShunts.erase(nameOfRShunts.begin());
		std::string temp = oss.str();
		_netlist.push_back(const_cast<char*>(temp.c_str()));
		
	}

	//Here are my Simulation properties which i send to NGSpice
	_netlist.push_back(const_cast<char*>(simulationLine.c_str()));
	

	//Now i will do a loop through the nodes of the voltageMeter to get the potential diffirence with probe
	for (auto nodes : nodesOfVoltageMeter)
	{
		std::string nodeString = "("; // Begin the string an opening parenthesis
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			nodeString += nodes[i];
			if (i < nodes.size() - 1)
			{
				nodeString += ","; // Add comma and space between node numbers
			}
		}
		nodeString += ")"; // Close the parenthesis

		std::ostringstream oss;
		oss << "circbyline .probe vd" << nodeString;
		std::string probeLine = oss.str();
		_netlist.push_back(const_cast<char*>(probeLine.c_str()));
	}

	
	// Here i create a probe for every Shunt resistor
	for (auto name : tempVecOfShunts)
	{
		std::ostringstream oss;
		oss << "circbyline .probe I(" << name << ")";
		std::string probeLine = oss.str();
		_netlist.push_back(const_cast<char*>(probeLine.c_str()));
	}

	_netlist.push_back(const_cast<char*>("circbyline .Control"));
	_netlist.push_back(const_cast<char*>("circbyline run"));
	_netlist.push_back(const_cast<char*>("circbyline echo \"Simulation Completed!\""));
	_netlist.push_back(const_cast<char*>("circbyline .endc"));
	_netlist.push_back(const_cast<char*>("circbyline .end"));

	return _netlist;
}



std::list<std::string> NGSpice::ngSpice_Initialize(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	
	updateBufferClasses(_connectionBlockMap ,allConnectionEntities, allEntitiesByBlockID, editorname);

	std::list<std::string> temp =  generateNetlist( solverEntity, allConnectionEntities,allEntitiesByBlockID, editorname);
	
	Numbers::RshunNumbers = 0;
	
	return temp;

}

std::shared_ptr<ot::EntityBlock> NGSpice::getEntityBlock(std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const ot::UID& _uid) const {
	auto it = _allEntitiesByBlockID.find(_uid);
	if (it != _allEntitiesByBlockID.end()) {
		return it->second;
	}
	OT_LOG_E("Block not found - EntityID: " + std::to_string(_uid));
	return nullptr;
}

ot::UIDList NGSpice::getConnections(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, const ot::UID& _uid) const {
	auto it = _connectionBlockMap.find(_uid);
	if (it != _connectionBlockMap.end()) {
		return it->second;
	}
	OT_LOG_E("Connection not found - EntityID: " + std::to_string(_uid));
	return ot::UIDList{};
}




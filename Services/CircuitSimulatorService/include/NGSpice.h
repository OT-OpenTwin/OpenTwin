// @otlicense
// File: NGSpice.h
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

#pragma once
//Service Header
#include <Circuit.h>
#include "Connection.h"
//Open Twin Header
#include "EntityBlock.h"
#include "EntityBlockConnection.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityFileText.h"

//ThirdPartyHeader
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <string.h>
#include <map>


class NGSpice
{
public:
	
	std::map<std::string, Circuit> mapOfCircuits;
	std::map<std::pair<ot::UID, std::string>, std::string> connectionNodeNumbers;
	std::map<std::string, std::string> customNameToNetlistNameMap;
	std::map<std::string, std::string> netlistNameToCustomNameMap;
	std::map<std::string, int> elementCounters;



	//Simulation Functions
	void updateBufferClasses(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	std::list<std::string> generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string editorname);
	std::string generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::list<std::string> ngSpice_Initialize(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	void clearBufferStructure(std::string name);

	// CircuitModel functions
	std::shared_ptr<EntityFileText> getModelEntity(const std::string& _folderName,std::string _modelName);
	std::string getCircuitModelType(std::shared_ptr<EntityFileText> _circuitModelEntity);
	std::string getCircuitModelText(std::shared_ptr<EntityFileText> _circuitModelEntity);
	std::vector<std::string> convertToCircByLine(const std::string& lines);

	//Connection Algorithm functions
	void connectionAlgorithmWithGNDElement(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string startingElement,int counter,ot::UID startingElementUID,ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	void connectionAlgorithmWithGNDVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	void handleWithConnectors(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	void setNodeNumbers(Connection& myConn);
	void setNodeNumbersWithGNDVoltageSource(Connection& myConn, ot::UID startingElementUID);
	bool checkIfElementOrConnectionVisited(std::set<ot::UID>& visitedElements, ot::UID elementUID);
	Connection createConnection(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,ot::UID connection);
	bool checkIfConnectionIsConnectedToGND(std::string pole);
	bool checkIfConnectionIsConnectedToGndVoltageSource(std::string pole, ot::UID voltageSourceUID, ot::UID elementUID);
	bool checkIfConnectionIsConnectedToVoltageMeter( std::string blockTitle);
	void setNodeNumbersOfVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	
	
	//Setter
	bool addToCustomNameToNetlistMap(const std::string&, const std::string&);
	bool addToNetlistNameToCustomMap(const std::string&, const std::string&);
	std::string assignElementID(const std::string& elementType);
	std::string to_lowercase(const std::string& str);


	//Getter
	std::map<std::string, Circuit>& getMapOfCircuits() { return mapOfCircuits; }
	std::string const getVoltMeterConnectionName() const { return m_voltMeterConnection; }
	std::map<std::string, std::string>& getMapOfCustomToNetlistName() { return this->customNameToNetlistNameMap; }
	std::map<std::string, int>& getMapOfElementCounters() { return this->elementCounters; }
	std::string getNetlistNameOfMap(const std::string& customName) const;
	std::shared_ptr<EntityBlock> getEntityBlock(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allEntitiesByBlockID, const ot::UID& _uid) const;
	ot::UIDList getConnections(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, const ot::UID& _uid) const;

private:
	const std::string m_voltMeterConnection = "voltageMeterConnection";
	const std::string m_voltageMeterTitle = "Voltage Meter";
	const std::string m_positivePole = "positivePole";
	const std::string m_gndPole = "GNDPole";
	const std::string m_modelType = "MODEL";
	const std::string m_subcktType = "SUBCKT";
	
	
};
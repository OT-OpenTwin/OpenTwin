#pragma once
//Service Header
#include <Circuit.h>
#include "Connection.h"
//Open Twin Header
#include "EntityBlock.h"
#include "EntityBlockConnection.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"


//ThirdPartyHeader
#include <ngspice/sharedspice.h>
//#include <ngspice/ngspice.h>
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
	std::map<std::string, int> elementCounters;

	//Simulation Functions
	void updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	std::string generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string editorname);
	std::string generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string ngSpice_Initialize(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	void clearBufferStructure(std::string name); 
	

	//Connection Algorithm functions
	void connectionAlgorithmWithGNDElement(std::string startingElement,int counter,ot::UID startingElementUID,ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	void connectionAlgorithmWithGNDVoltageSource(std::string startingElement, int counter, ot::UID startingElementUID, ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	void connectionAlgorithmWithoutGND(ot::UID elementUID,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	bool checkIfElementOrConnectionVisited(std::set<ot::UID>& visitedElements, ot::UID elementUID);
	Connection createConnection(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,ot::UID connection);
	bool checkIfConnectionIsConnectedToGND(std::string pole);
	bool checkIfConnectionIsConnectedToGndVoltageSource(std::string pole, ot::UID voltageSourceUID, ot::UID elementUID);
	bool checkIfConnectionIsConnectedToVoltageMeter( std::string blockTitle);

	//Callback functions from NGSpice
	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);
	static int MySendDataFunction(pvecvaluesall, int, int, void*);
	static int MySendInitDataFunction(pvecinfoall, int, void*);
	
	//Setter
	bool addToCustomNameToNetlistMap(const std::string&, const std::string&);
	std::string assignElementID(const std::string& elementType);

	//Getter
	std::map<std::string, Circuit>& getMapOfCircuits() { return mapOfCircuits; }
	std::string const getVoltMeterConnectionName() const { return m_voltMeterConnection; }
	std::map<std::string, std::string>& getMapOfCustomToNetlistName() { return this->customNameToNetlistNameMap; }
	std::map<std::string, int>& getMapOfElementCounters() { return this->elementCounters; }
	std::string getNetlistNameOfMap(const std::string& customName) const;
	


	

private:
	const std::string m_voltMeterConnection = "voltageMeterConnection";
	const std::string m_voltageMeterTitle = "Voltage Meter";
	const std::string m_positivePole = "positivePole";
	const std::string m_gndPole = "GNDPole"; 

	
};
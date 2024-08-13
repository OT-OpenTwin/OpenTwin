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
	


	void updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	std::string generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string editorname);
	std::string generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string ngSpice_Initialize(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	void clearBufferStructure(std::string name); 
	void getNodeNumbersOfMeters(std::string ,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::vector<std::string>&);
	bool isValidNodeString(const std::string& input);
	void connectionAlgorithm(int counter,ot::UID voltageSource,ot::UID elementUID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID, std::string editorname, std::set<ot::UID>& visitedElements);
	

	//Callback functions from NGSpice
	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);
	static int MySendDataFunction(pvecvaluesall, int, int, void*);
	static int MySendInitDataFunction(pvecinfoall, int, void*);
	

	//Getter
	std::map<std::string, Circuit>& getMapOfCircuits() { return mapOfCircuits; }

private:
	
	
};
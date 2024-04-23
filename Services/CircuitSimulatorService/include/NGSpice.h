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

	


	void updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	std::string generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string editorname);
	std::string generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string ngSpice_Initialize(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	void clearBufferStructure(std::string name); 
	std::string getNodeNumbersOfVoltageMeter(std::string ,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&);
	bool isValidNodeString(const std::string& input);
	//Result Curve Functions
	

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
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

	
	/*void setConnectionNodeNumbers(std::map<std::string, std::shared_ptr<EntityBlock>>&);
	std::string getBlockEntityTitleByUID(std::string UID, std::map<std::string, std::shared_ptr<EntityBlock>>&);*/

	void updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	std::string generateNetlist(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string editorname);
	std::string generateNetlistDCSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistACSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string generateNetlistTRANSimulation(EntityBase* solverEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>, std::map<ot::UID, std::shared_ptr<EntityBlock>>&, std::string editorname);
	std::string ngSpice_Initialize(EntityBase* solverEntity,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>,std::map<ot::UID, std::shared_ptr<EntityBlock>>&,std::string);
	void clearBufferStructure(std::string name);

	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);

	//Getter
	/*std::multimap<std::string, Connection>& getMapOfConnections() { return mapOfConnections; }*/
	std::map<std::string, Circuit>& getMapOfCircuits() { return mapOfCircuits; }
	//Setter
	/*void addConnection(std::string key, const Connection& obj);*/


private:
	
	/*std::multimap<std::string,Connection> mapOfConnections;*/
};
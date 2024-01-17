#pragma once
//Service Header
#include <Circuit.h>
#include "Connection.h"
//Open Twin Header
#include "EntityBlock.h"


//ThirdPartyHeader
#include <ngspice/sharedspice.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <string.h>

class NGSpice
{
public:
	std::map<std::string, Circuit> mapOfCircuits;

	void addElementConnection(const std::string&, const Connection&);
	void addResistorConnection(const std::string&, const Connection&);
	void setConnectionNodeNumbers(std::map<std::string, std::shared_ptr<EntityBlock>>&);

	std::string generateNetlist(std::map<std::string, std::shared_ptr<EntityBlock>>&);
	std::string ngSpice_Initialize(std::map<std::string, std::shared_ptr<EntityBlock>>&);

	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);

	//Getter
	std::map<std::string, Connection>& getElementConnections() { return ElementConnections; }
	std::map<std::string, Connection>& getResistorConnections() { return ResistorConnections; }

	//Setter
	void setElementConnections(std::map<std::string, Connection>& a) { ElementConnections = a; }
	void setResistorConnections(std::map<std::string, Connection>& a) { ResistorConnections = a; }


private:
	std::map<std::string , Connection> ElementConnections;
	std::map<std::string, Connection> ResistorConnections;
};
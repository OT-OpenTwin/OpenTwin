#pragma once
//Service Header
#include <Circuit.h>

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

	std::string generateNetlist(std::map<std::string, std::shared_ptr<EntityBlock>>&);
	std::string ngSpice_Initialize(std::map<std::string, std::shared_ptr<EntityBlock>>&);

	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);
};
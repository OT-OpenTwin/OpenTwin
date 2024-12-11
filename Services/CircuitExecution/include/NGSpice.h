#pragma once
//ThirdPartyHeader
#include <ngspice/sharedspice.h>

//OpenTwin Header
#include "OTCore/Logger.h"
class NGSpice {
public:
	NGSpice();
	//Callback functions from NGSpice
	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);
	static int MySendDataFunction(pvecvaluesall, int, int, void*);
	static int MySendInitDataFunction(pvecinfoall, int, void*);

	void runSimulation(std::list<std::string>& _netlist);

private:
	void initializeCallbacks();
	void intializeNGSpice();
};
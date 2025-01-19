#pragma once
//ThirdPartyHeader
#include <ngspice/sharedspice.h>

//OpenTwin Header
#include "OTCore/Logger.h"

//Service Header
#include "ConnectionManager.h"
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
	void initializeCallbacks();
	void intializeNGSpice();

private:

	static double calculateMagnitude(double real, double imag);
};
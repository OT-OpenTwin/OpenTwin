#pragma once

// ThirdParty header
#include <ngspice/sharedspice.h>

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/OTClassHelper.h"

// Service header
#include "ConnectionManager.h"

class NGSpice {
	OT_DECL_NODEFAULT(NGSpice)
public:
	static void runNGSpice(std::list<std::string> _netlist);

private:
	// Callback functions from NGSpice

	static int MySendCharFunction(char*, int, void*);
	static int MySendStat(char*, int, void*);
	static int MyControlledExit(int, bool imidiate, bool quitexit, int, void*);
	static int MySendDataFunction(pvecvaluesall, int, int, void*);
	static int MySendInitDataFunction(pvecinfoall, int, void*);

	// Starting Functions

	static void init(std::list<std::string> _netlist);
	static void runSimulation(std::list<std::string>& _netlist);
	static void initializeCallbacks();
	static void intializeNGSpice();

	static double calculateMagnitude(double real, double imag);

};
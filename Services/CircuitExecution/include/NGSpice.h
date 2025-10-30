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
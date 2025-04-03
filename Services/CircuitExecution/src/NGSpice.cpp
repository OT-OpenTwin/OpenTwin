//Service Header
#include "NGSpice.h"
#include "SimulationResults.h"
#include "ConnectionManager.h"
#include "Application.h"

// C++ Header
#include <thread>


void NGSpice::runNGSpice(std::list<std::string> _netlist) {
	std::thread workerThread(&NGSpice::init, _netlist);
	workerThread.detach();
}


// Callback Functions for NGSpice
int NGSpice::MySendCharFunction(char* output, int ident, void* userData) {

	std::string callback = std::string(output);

	if (callback.rfind("stdout",0) == 0) {
		
		const std::string stdoutPrefix = "stdout";
		callback.erase(0, stdoutPrefix.size());

		if(callback.find("Simulation Completed!") != std::string::npos)
		{
			//The results are being pushed in the callbacks to SimulationResults map
			//Application::getInstance()->getConnectionManager()->sendBackResults(SimulationResults::getInstance()->getResultMap());
			/*OT_LOG_W("Sended Back INIT Results and Thread is ended");*/

		}
		SimulationResults::getInstance()->triggerCallback("Message", callback);
	}
	else if (callback.rfind("stderr",0) == 0) {
		
		const std::string sterrPrefix = "stderr";
		callback.erase(0, sterrPrefix.size());
		SimulationResults::getInstance()->triggerCallback("Error", callback);
	}
	
#ifndef _DEBUG
	OT_LOG_D(std::string(output));
#endif // !_DEBUG

	
	
	return 0;
}

int NGSpice::MySendStat(char* outputReturn, int ident, void* userData) {
	
	SimulationResults::getInstance()->triggerCallback("Message", std::string(outputReturn));
#ifndef _DEBUG
	OT_LOG_D(std::string(outputReturn));
#endif // !_DEBUG


	return 0;
}

int NGSpice::MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata) {
	
	if (immediate || (exitstatus >= 1 && !quitexit)) {
		SimulationResults::getInstance()->triggerCallback("UnrecoverableError", "NGSpice fatal error, check Circuit and try again");
		
	}
	else if (quitexit && immediate) {
		SimulationResults::getInstance()->triggerCallback("UnrecoverableError", "NGSpice fata error, check Circuit and try again");
	}
	else if (exitstatus == 1 && !immediate && quitexit) {
		SimulationResults::getInstance()->triggerCallback("Message", "Simulation ended normally without error");
	}
	else {
		SimulationResults::getInstance()->triggerCallback("UnrecoverableError", "Unexpected exit condition, restarting ngspice");
	}

	OT_LOG_E(std::to_string(immediate));
	OT_LOG_E(std::to_string(quitexit));
	OT_LOG_E(std::to_string(exitstatus));

	SimulationResults::getInstance()->getResultMap().clear();
	return 0;
}



int NGSpice::MySendDataFunction(pvecvaluesall vectorsAll, int numStructs, int idNumNGSpiceSharedLib, void* userData) {

	for (int i = 0; i < vectorsAll->veccount; ++i) {
		std::string name = vectorsAll->vecsa[i]->name;

		double value = 0;
		if (vectorsAll->vecsa[i]->is_complex) {
			value = calculateMagnitude(vectorsAll->vecsa[i]->creal, vectorsAll->vecsa[i]->cimag);
		}
		else {
			value = vectorsAll->vecsa[i]->creal;
		}

		SimulationResults::getInstance()->getResultMap().at(name).push_back(value);
	}

	return 0;


}

int NGSpice::MySendInitDataFunction(pvecinfoall vectorInfoAll, int idNumNGSpiceSharedLib, void* userData) {

	for (int i = 0; i < vectorInfoAll->veccount; i++) {
		std::string name = vectorInfoAll->vecs[i]->vecname;
		std::vector<double> values;
		SimulationResults::getInstance()->setVecAmount(vectorInfoAll->veccount);
		if (name != "voltagemeterconnection") {
			SimulationResults::getInstance()->getResultMap().insert_or_assign(name, values);
		}
		else {
			continue;
		}


	}

	return 0;
	
}



void NGSpice::init(std::list<std::string> _netlist) {
	//Initialize Callbacks of NGSpice
	initializeCallbacks();

	//Intialize NGSpice
	intializeNGSpice();

	
	//Executing run from NGSpice
	runSimulation(_netlist);

	Application::getInstance()->getConnectionManager()->sendBackResults(SimulationResults::getInstance()->getResultMap());
}

void NGSpice::runSimulation(std::list<std::string>& _netlist) {
	for (const std::string& command : _netlist) {
		ngSpice_Command(const_cast<char*>(command.c_str()));
	}
}

void NGSpice::initializeCallbacks() {
	SendChar* printfcn = MySendCharFunction;
	SendStat* statfcn = MySendStat;
	ControlledExit* ngexit = MyControlledExit;
	SendData* datfcn = MySendDataFunction;
	SendInitData* initData = MySendInitDataFunction;
}

void NGSpice::intializeNGSpice() {
	int status =ngSpice_Init(MySendCharFunction, MySendStat, MyControlledExit, MySendDataFunction, MySendInitDataFunction, nullptr, nullptr);
	if (status == 0) {
		OT_LOG_D("Worked");
	}
	else if (status == 1) {
		OT_LOG_E("Something went wrong");
	}
}


double NGSpice::calculateMagnitude(double real, double imag) {
	return sqrt(real * real + imag * imag);
}

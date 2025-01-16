//Service Header
#include "NGSpice.h"
#include "SimulationResults.h"
#include "ConnectionManager.h"

NGSpice::NGSpice() {
	//Initialize Callbacks of NGSpice
	initializeCallbacks();

	//Intialize NGSpice
	intializeNGSpice();
	


}

//Callback Functions for NGSpice
int NGSpice::MySendCharFunction(char* output, int ident, void* userData) {

	SimulationResults::getInstance()->triggerCallback("Message", std::string(output));
	
	OT_LOG_D(std::string(output));
	
	return 0;
}

int NGSpice::MySendStat(char* outputReturn, int ident, void* userData) {
	
	SimulationResults::getInstance()->triggerCallback("Message", std::string(outputReturn));
	OT_LOG_D(std::string(outputReturn));

	return 0;
}

int NGSpice::MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata) {
	

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

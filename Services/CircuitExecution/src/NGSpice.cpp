//Service Header
#include "NGSpice.h"



NGSpice::NGSpice() {
	//Initialize Callbacks of NGSpice
	initializeCallbacks();

	//Intialize NGSpice
	intializeNGSpice();
	


}

//Callback Functions for NGSpice
int NGSpice::MySendCharFunction(char* output, int ident, void* userData) {

	OT_LOG_D(std::string(output));

	return 0;
}

int NGSpice::MySendStat(char* outputReturn, int ident, void* userData) {
	
	OT_LOG_D(std::string(outputReturn));

	return 0;
}

int NGSpice::MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata) {

	OT_LOG_E(std::to_string(exitstatus));

	return 0;
}



int NGSpice::MySendDataFunction(pvecvaluesall vectorsAll, int numStructs, int idNumNGSpiceSharedLib, void* userData) {



	return 0;
}

int NGSpice::MySendInitDataFunction(pvecinfoall vectorInfoAll, int idNumNGSpiceSharedLib, void* userData) {


	
	return 0;
}

void NGSpice::runSimulation(std::list<std::string>& _netlist) {
	OT_LOG_D("Run Simulation");
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

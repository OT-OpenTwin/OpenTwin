// @otlicense

#pragma once
#ifndef KRIGING_CLASS

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <Python.h>
#include <numpy/arrayobject.h>

#include "KrigingParams.h"
#include "IBase.h"
#include "DataSet.h"

using namespace std;
using std::string;

class Kriging : protected IBase
{
private:
	KrigingParams params;
	DataSet ds;
	string modelStr;

public:
	Kriging(const std::string scriptName);
	Kriging(const std::string scriptName, const KrigingParams params);
	~Kriging();

	void setParameters(const KrigingParams params);
	KrigingParams getParameters();
	void setDataSet(DataSet ds);
	DataSet getDataSet();
	void setModelStr(const string model);
	string getModelStr();

	void initializePython();

	//void train(const char* scriptName, const char* funcName);
	int predict();

	// Inherited via ScriptBase
	virtual int setDataset(std::list<std::vector<double>> d) override;
	virtual int train() override;
	virtual int predict(std::list<std::vector<double>> d) override;
	virtual void getModel() override;


	int testing();

	int dsTest();

	std::string outputs;

};


#endif
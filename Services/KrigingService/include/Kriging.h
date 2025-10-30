// @otlicense
// File: Kriging.h
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
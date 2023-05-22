#pragma once
#include "PythonAPI.h"
#include <vector>
#include <string>
#include "OpenTwinCore/Variable.h"
#include "PythonWrapper.h"

class PythonAPI
{
public:
	PythonAPI();
	void InterpreteString(const std::string& programm, ot::VariableBundle& variables);
	void InterpreteString(std::vector<std::string>& programms, std::vector<ot::VariableBundle>& variables);
	
private:
	PythonWrapper wrapper;
};




#pragma once
#include "PythonAPI.h"
#include "PythonWrapper.h"
#include "EntityParameterizedDataSource.h"

class PythonAPI
{
public:
	PythonAPI();
	/*Execute()*/

private:
	PythonWrapper _wrapper;
	std::map<std::string, std::string> _serviceByServiceName;
	
};



struct ServicePythonModule
{
	std::map<ot::UID, EntityParameterizedDataSource*> allImportedScripts;
};

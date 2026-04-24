#pragma once
#include "OTCore/Python/PythonParameter.h"
#include <map>
#include <list>
#include "OTCore/Variable/Variable.h"
#include "OTCore/CoreAPIExport.h"
class OT_CORE_API_EXPORT PyhonParameterBuilderGeneric
{
public: 
	static std::string getTypeName() { return "Generic"; }
	static PythonParameter create(const std::string& _entityName, const std::map < std::string, std::list<ot::Variable>>& _parameter);
	static std::map < std::string, std::list<ot::Variable>> extract(const PythonParameter& _parameter);
};

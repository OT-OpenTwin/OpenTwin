#pragma once
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/Python/PythonParameter.h"
#include <string>
#include "OTCore/CoreAPIExport.h"
class OT_CORE_API_EXPORT PyhonParameterBuilderValueComparisons
{
public:
	static std::string getTypeName() { return "ValueComp"; }
	static PythonParameter create(const std::string& _entityName, const std::list<ot::ValueComparisonDescription>& _valueComp);
	static std::list<ot::ValueComparisonDescription> extract(const PythonParameter& _parameter);
};

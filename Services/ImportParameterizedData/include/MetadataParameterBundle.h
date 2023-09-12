#pragma once
#include "MetadataParameter.h"
#include <set>

class MetadataParameterBundle
{
public:
	void AddMetadataParameter(MetadataParameter& parameter) { _parameter.push_back(parameter); };
	
	std::set<std::string> GetAllParameterAbbreviations();
	std::list<MetadataParameter>& getParameter() { return _parameter; };
private:
	std::set<std::string> _allParameterAbbreviations;
	std::list<MetadataParameter> _parameter;
};

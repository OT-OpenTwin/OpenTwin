#include "MetadataParameterBundle.h"
#include <assert.h>

std::set<std::string> MetadataParameterBundle::GetAllParameterAbbreviations()
{
	if (_allParameterAbbreviations.size() == 0)
	{
		for (const auto& param : _parameter)
		{
			_allParameterAbbreviations.insert(param.parameterAbbreviation);
		}
	}
	
	return _allParameterAbbreviations;
}

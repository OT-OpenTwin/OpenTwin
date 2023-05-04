#include "MetadataParameterBundle.h"
#include <assert.h>

std::set<std::string> MetadataParameterBundle::GetAllParameterAbbreviations()
{
	if (_allParameterAbbreviations.size() == 0)
	{
		for (const auto& param : _stringParameter)
		{
			_allParameterAbbreviations.insert(param.parameterAbbreviation);
		}
		for (const auto& param : _doubleParameter)
		{
			_allParameterAbbreviations.insert(param.parameterAbbreviation);
		}
		for (const auto& param : _int32Parameter)
		{
			_allParameterAbbreviations.insert(param.parameterAbbreviation);
		}
		for (const auto& param : _int64Parameter)
		{
			_allParameterAbbreviations.insert(param.parameterAbbreviation);
		}
	}
	
	return _allParameterAbbreviations;
}

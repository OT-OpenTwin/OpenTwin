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

std::list<int32_t> MetadataParameterBundle::GetParameterValueIndices(int32_t quantityIndex)
{
	std::list<int32_t> paramValueIndices;
	for (const std::string abbrev : GetAllParameterAbbreviations())
	{
		bool paramerterFound = false;
		for (const auto& param : _stringParameter)
		{
			if (param.parameterAbbreviation == abbrev)
			{
				paramValueIndices.push_back(param.valueIndices.find(param.selectedValues[quantityIndex])->second);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : _doubleParameter)
		{
			if (param.parameterAbbreviation == abbrev)
			{
				paramValueIndices.push_back(param.valueIndices.find(param.selectedValues[quantityIndex])->second);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : _int32Parameter)
		{
			if (param.parameterAbbreviation == abbrev)
			{
				paramValueIndices.push_back(param.valueIndices.find(param.selectedValues[quantityIndex])->second);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : _int64Parameter)
		{
			if (param.parameterAbbreviation == abbrev)
			{
				paramValueIndices.push_back(param.valueIndices.find(param.selectedValues[quantityIndex])->second);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
	}

	return paramValueIndices;
}
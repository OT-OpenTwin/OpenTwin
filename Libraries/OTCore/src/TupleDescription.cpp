#include "OTCore/Tuple/TupleDescription.h"
#include "OTCore/String.h"
#include <algorithm>

std::vector<std::string> TupleDescription::getTupleElementNames(const std::string& _formatName) const
{
	return m_tupleElementNamesByFormatName.find(_formatName) != m_tupleElementNamesByFormatName.end() ? m_tupleElementNamesByFormatName.at(_formatName) : std::vector<std::string>();
}

std::vector<std::string> TupleDescription::getUnitCombinations(const std::string& _formatName) const
{
	return m_unitCombinationsByFormatName.find(_formatName) != m_unitCombinationsByFormatName.end() ? m_unitCombinationsByFormatName.at(_formatName) : std::vector<std::string>();
}

std::string TupleDescription::createCombinedUnitsString(const std::vector<std::string>& _units) const
{
	std::string combinedUnitsString;
	for(const std::string& unit : _units)
	{
		if(unit.empty())
		{
			combinedUnitsString += "\"\"";
		}
		else
		{
			combinedUnitsString += unit;
		}
		combinedUnitsString += ",";
	}
	if(!combinedUnitsString.empty())
	{
		combinedUnitsString.pop_back(); // Remove the trailing comma.
	}
	return combinedUnitsString;
}

std::vector<std::string> TupleDescription::separateCombinedUnitString(const std::string& _combinedUnitString) 
{
	std::list<std::string> units =	ot::String::split(_combinedUnitString, ",");
	
	std::replace(units.begin(), units.end(), std::string("\"\""), std::string(""));
	return std::vector<std::string>{units.begin(), units.end()};
}


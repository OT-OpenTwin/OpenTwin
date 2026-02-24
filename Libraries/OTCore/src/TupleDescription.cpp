#include "OTCore/Tuple/TupleDescription.h"

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
		if(_units.empty())
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


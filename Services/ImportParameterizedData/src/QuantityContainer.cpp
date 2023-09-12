#include "QuantityContainer.h"


QuantityContainer::QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection)
	: _parameterValueIndices(parameterValueIndices), _isFlatCollection(isFlatCollection), _quantityIndex(quantityIndex)
{

	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaType", "QuantityContainer"));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaVersion", 1));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("MSMD", msmdIndex));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("Quantity", _quantityIndex));
	auto paramValIndex = parameterValueIndices.begin();
	for (const std::string& paramAbbrev : parameterAbbreviations)
	{
		_mongoDocument.append(bsoncxx::builder::basic::kvp(paramAbbrev, *paramValIndex));
		paramValIndex++;
	}
}

void QuantityContainer::AddValue(ot::Variable& value)
{
	_converter(_mongoDocument, value, "Value");
	_values.push_back(value);
}

bool QuantityContainer::ParameterValueIndicesAndQuantityIndexAreMatching(const std::list<int32_t>& parameterValueIndices, int32_t quantityIndex)
{
	auto givenParameterIndexValue = _parameterValueIndices.begin();
	if (quantityIndex != _quantityIndex)
	{
		return false;
	}
	bool match = true;
	for (const auto& parameterValueIndex : parameterValueIndices)
	{
		if (parameterValueIndex != *givenParameterIndexValue)
		{
			match = false;
			break;
		}
		givenParameterIndexValue++;
	}

	return match;
}

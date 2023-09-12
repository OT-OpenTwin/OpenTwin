#include "QuantityContainer.h"
#include "VariableToBSONConverter.h"

QuantityContainer::QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection)
	: _isFlatCollection(isFlatCollection)
{
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaType", "QuantityContainer"));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaVersion", 1));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("MSMD", msmdIndex));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("Quantity", quantityIndex));
	auto paramValIndex = parameterValueIndices.begin();

	for (const std::string& paramAbbrev : parameterAbbreviations)
	{
		_mongoDocument.append(bsoncxx::builder::basic::kvp(paramAbbrev, *paramValIndex));
		paramValIndex++;
	}
}

void QuantityContainer::AddValue(ot::Variable& value)
{
	if (_isFlatCollection && _values.size() > 0)
	{
		throw std::exception("Quantity document is defined for a flat collection and cannot hold more than one value.");
	}
	_values.push_back(value);
}

const bsoncxx::builder::basic::document& QuantityContainer::getMongoDocument()
{
	VariableToBSONConverter converter;
	if (_isFlatCollection)
	{
		converter(_mongoDocument, *_values.begin(), "Values");
	}
	else
	{
		auto valueArray = bsoncxx::builder::basic::array();
		for (auto& value : _values)
		{
			converter(valueArray, value);
		}
		_mongoDocument.append(bsoncxx::builder::basic::kvp("Values", valueArray));
	}
	return _mongoDocument;
}


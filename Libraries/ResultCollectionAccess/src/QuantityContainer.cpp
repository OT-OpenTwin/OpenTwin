#include "QuantityContainer.h"
#include "VariableToBSONConverter.h"
#include "MetadataQuantity.h"
#include "MetadataSeries.h"

QuantityContainer::QuantityContainer(int32_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, int32_t quantityIndex)
{
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaType", "QuantityContainer"));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaVersion", 1));
	_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataSeries::getFieldName(), seriesIndex));
	_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataQuantity::getFieldName(), quantityIndex));
	
	VariableToBSONConverter converter;
	auto parameterValue =parameterValues.begin();
	for (const auto& parameterAbbreviation : parameterAbbreviations)
	{
		converter(_mongoDocument,*parameterValue++,parameterAbbreviation);
	}

}

QuantityContainer::QuantityContainer(QuantityContainer&& other)
{
	_values = std::move(other._values);
	_mongoDocument = std::move(other._mongoDocument);
}


QuantityContainer QuantityContainer::operator=(QuantityContainer&& other)
{
	return QuantityContainer(std::move(other));
}

QuantityContainer::~QuantityContainer()
{
}

void QuantityContainer::AddValue(const ot::Variable& value)
{
	_values.push_back(value);
}

bsoncxx::builder::basic::document& QuantityContainer::getMongoDocument()
{
	VariableToBSONConverter converter;
	if (_values.size() == 1)
	{
		converter(_mongoDocument, *_values.begin(), QuantityContainer::getFieldName());
	}
	else
	{
		auto valueArray = bsoncxx::builder::basic::array();
		for (auto& value : _values)
		{
			converter(valueArray, value);
		}
		_mongoDocument.append(bsoncxx::builder::basic::kvp(QuantityContainer::getFieldName(), valueArray));
	}
	return _mongoDocument;
}


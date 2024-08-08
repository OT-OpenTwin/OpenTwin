#include "QuantityDescription.h"

void QuantityDescription::addValueDescription(const std::string _valueName, const std::string _valueDataType, const std::string _valueUnit)
{
	MetadataQuantityValueDescription valueDescription;
	valueDescription.quantityValueName = _valueName;
	valueDescription.unit = _valueUnit;
	valueDescription.dataTypeName = _valueDataType;
	m_metadataQuantity.valueDescriptions.push_back(std::move(valueDescription));
}

ot::UID QuantityDescription::getFirstValueQuantityIndex() const
{
	auto& descriptions = m_metadataQuantity.valueDescriptions;
	PRE(descriptions.size() >= 1 && descriptions.begin()->quantityIndex != 0);
	return descriptions.begin()->quantityIndex;
}

ot::UID QuantityDescription::getSecondValueQuantityIndex() const
{
	auto& descriptions = m_metadataQuantity.valueDescriptions;
	PRE(descriptions.size() >= 2 && (descriptions.begin()++)->quantityIndex != 0);
	auto secondDescription = descriptions.begin()++;
	return secondDescription->quantityIndex;
}

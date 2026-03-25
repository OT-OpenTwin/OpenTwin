#include "OTCore/QueryDescription/DataLakeQueryCfg.h"

void DataLakeQueryCfg::setSeriesLabel(const std::string& _seriesLabel)
{
	m_seriesLabel = _seriesLabel;
}

void DataLakeQueryCfg::setCollectionName(const std::string& _collectionName)
{
	m_collectionName = _collectionName;
}
void DataLakeQueryCfg::setValueDescriptionParameters(const std::list<ot::ValueComparisonDescription>& _valueCompares)
{
	m_valueDescriptionsParameters = _valueCompares;
}
void DataLakeQueryCfg::setValueDescriptionQuantities(const ot::ValueComparisonDescription& _valueCompares)
{
	m_valueDescriptionsQuantities = _valueCompares;
}
void DataLakeQueryCfg::setValueDescriptionSeriesMD(const std::list<ot::ValueComparisonDescription>& _valueCompares)
{
	m_valueDescriptionsSeriesMD = _valueCompares;
}

const std::list<ot::ValueComparisonDescription>& DataLakeQueryCfg::getValueDescriptionParameters() const
{
	return m_valueDescriptionsParameters;
}

const ot::ValueComparisonDescription& DataLakeQueryCfg::getValueDescriptionQuantities() const
{
	return m_valueDescriptionsQuantities;
}

const std::list<ot::ValueComparisonDescription>& DataLakeQueryCfg::getValueDescriptionSeriesMD() const
{
	return m_valueDescriptionsSeriesMD;
}

void DataLakeQueryCfg::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	ot::JsonArray parameters;
	for (auto& parameter : m_valueDescriptionsParameters)
	{
		ot::JsonObject entry;
		parameter.addToJsonObject(entry, _allocator);
		parameters.PushBack(entry, _allocator);
	}
	_jsonObject.AddMember("Parameters", parameters, _allocator);

	ot::JsonArray seriesMDs;
	for (auto& seriesMD: m_valueDescriptionsSeriesMD)
	{
		ot::JsonObject entry;
		seriesMD.addToJsonObject(entry, _allocator);
		seriesMDs.PushBack(entry, _allocator);
	}
	_jsonObject.AddMember("SeriesMD", seriesMDs, _allocator);

	ot::JsonObject quantity;
	m_valueDescriptionsQuantities.addToJsonObject(quantity, _allocator);
	_jsonObject.AddMember("Quantities", quantity, _allocator);

	_jsonObject.AddMember("SeriesLabel", ot::JsonString(m_seriesLabel, _allocator), _allocator);
	_jsonObject.AddMember("CollectionName", ot::JsonString(m_collectionName, _allocator), _allocator);
	

}

void DataLakeQueryCfg::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	ot::ConstJsonArray parameters = ot::json::getArray(_jsonObject, "Parameters");
	for (uint32_t i = 0; i < parameters.Size(); i++)
	{
		ot::ValueComparisonDescription valueDescription;
		valueDescription.setFromJsonObject(ot::json::getObject(parameters, i));
		m_valueDescriptionsParameters.push_back(valueDescription);
	}

	auto quantities = ot::json::getObject(_jsonObject, "Quantities");
	m_valueDescriptionsQuantities.setFromJsonObject(quantities);
	
	ot::ConstJsonArray seriesMD = ot::json::getArray(_jsonObject, "SeriesMD");
	for (uint32_t i = 0; i < seriesMD.Size(); i++)
	{
		ot::ValueComparisonDescription valueDescription;
		valueDescription.setFromJsonObject(ot::json::getObject(seriesMD, i));
		m_valueDescriptionsSeriesMD.push_back(valueDescription);
	}

	m_seriesLabel = ot::json::getString(_jsonObject, "SeriesLabel");
	m_collectionName = ot::json::getString(_jsonObject, "CollectionName");
}

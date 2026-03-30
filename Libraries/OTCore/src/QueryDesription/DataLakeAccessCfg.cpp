// @otlicense

// OpenTwin header
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
using namespace ot;

ot::DataLakeAccessCfg::DataLakeAccessCfg(const DataLakeAccessCfg& _other)
{
	m_collectionToQueryMap = _other.m_collectionToQueryMap;
	
	for (auto& entry : _other.m_seriesMetadataMap)
	{
		ot::JsonDocument newDoc;
		newDoc.CopyFrom(entry.second, newDoc.GetAllocator());
		m_seriesMetadataMap.insert({ entry.first,std::move(newDoc) });
	}
	m_collectionNameBase = _other.m_collectionNameBase;
	
	m_fieldDecodersParameter = _other.m_fieldDecodersParameter;
	m_fieldDecodersQuantity = _other.m_fieldDecodersQuantity;
	m_fieldDecodersSeries = _other.m_fieldDecodersSeries;

	m_valueTransformerByFieldKeyParameter = _other.m_valueTransformerByFieldKeyParameter;
	m_valueTransformerByFieldKeyQuantity = _other.m_valueTransformerByFieldKeyQuantity;
}

ot::DataLakeAccessCfg& ot::DataLakeAccessCfg::operator=(const DataLakeAccessCfg& _other)
{
	m_collectionToQueryMap = _other.m_collectionToQueryMap;

	for (auto& entry : _other.m_seriesMetadataMap)
	{
		ot::JsonDocument newDoc;
		newDoc.CopyFrom(entry.second, newDoc.GetAllocator());
		m_seriesMetadataMap.insert({ entry.first,std::move(newDoc) });
	}
	m_collectionNameBase = _other.m_collectionNameBase;

	m_fieldDecodersParameter = _other.m_fieldDecodersParameter;
	m_fieldDecodersQuantity = _other.m_fieldDecodersQuantity;
	m_fieldDecodersSeries = _other.m_fieldDecodersSeries;

	m_valueTransformerByFieldKeyParameter = _other.m_valueTransformerByFieldKeyParameter;
	m_valueTransformerByFieldKeyQuantity = _other.m_valueTransformerByFieldKeyQuantity;
	return *this;
}

void ot::DataLakeAccessCfg::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("CollectionNameBase", ot::JsonString(m_collectionNameBase, _allocator), _allocator);
	JsonArray collectionQueryArr;
	for (const auto& it : m_collectionToQueryMap)
	{
		JsonObject kvp;
		kvp.AddMember("Collection", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Query", JsonString(it.second, _allocator), _allocator);
		collectionQueryArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("CollectionQueryMap", std::move(collectionQueryArr), _allocator);

	JsonArray seriesMetadataArr;
	for (const auto& it : m_seriesMetadataMap)
	{
		JsonObject kvp;
		kvp.AddMember("Series", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Data", JsonValue(it.second, _allocator), _allocator);
		seriesMetadataArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("SeriesMetadataMap", std::move(seriesMetadataArr), _allocator);
	

	JsonArray fieldDecoderParamArr;
	for (const auto& it : m_fieldDecodersParameter)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Decoder", JsonObject(it.second, _allocator), _allocator);
		fieldDecoderParamArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("FieldDecodersParam", std::move(fieldDecoderParamArr), _allocator);

	JsonArray fieldDecoderQuantArr;
	for (const auto& it : m_fieldDecodersQuantity)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Decoder", JsonObject(it.second, _allocator), _allocator);
		fieldDecoderQuantArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("FieldDecodersQuant", std::move(fieldDecoderQuantArr), _allocator);

	JsonArray fieldDecoderSeriesArr;
	for (const auto& it : m_fieldDecodersSeries)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Decoder", JsonObject(it.second, _allocator), _allocator);
		fieldDecoderSeriesArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("FieldDecodersSer", std::move(fieldDecoderSeriesArr), _allocator);

	JsonArray valueTransformerParamArr;
	for (const auto& it : m_valueTransformerByFieldKeyParameter)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Transformer", JsonObject(it.second, _allocator), _allocator);
		valueTransformerParamArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("ValueTransformersParam", std::move(valueTransformerParamArr), _allocator);

	JsonArray valueTransformerQuant;
	for (const auto& it : m_valueTransformerByFieldKeyQuantity)
	{
		JsonArray transformer;
		for (auto& processor : it.second)
		{
			ot::JsonObject entry;
			processor.addToJsonObject(entry, _allocator);
			transformer.PushBack(entry, _allocator);
		}


		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Transformer", transformer, _allocator);
		valueTransformerQuant.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("ValueTransformersQuant", std::move(valueTransformerQuant), _allocator);
}

void ot::DataLakeAccessCfg::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_collectionNameBase = ot::json::getString(_jsonObject, "CollectionNameBase");
	
	m_collectionToQueryMap.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "CollectionQueryMap"))
	{
		std::string collection = json::getString(kvp, "Collection");
		std::string query = json::getString(kvp, "Query");
		m_collectionToQueryMap.emplace(std::move(collection), std::move(query));
	}

	m_seriesMetadataMap.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "SeriesMetadataMap"))
	{
		std::string series = json::getString(kvp, "Series");
		JsonDocument metadata;
		metadata.CopyFrom(kvp["Data"], metadata.GetAllocator());
		m_seriesMetadataMap.emplace(std::move(series), std::move(metadata));
	}

	m_fieldDecodersParameter.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecodersParam"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecodersParameter.emplace(fieldKey, decoder);
		m_fieldDecodersParameterByLabel.insert_or_assign(decoder.getLabel(), &m_fieldDecodersParameter.find(fieldKey)->second);
	}

	m_fieldDecodersQuantity.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecodersQuant"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecodersQuantity.emplace(fieldKey, decoder);
		m_fieldDecodersQuantityByLabel.insert_or_assign(decoder.getLabel(), &m_fieldDecodersQuantity.find(fieldKey)->second);
	}

	m_fieldDecodersSeries.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecodersSer"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecodersSeries.emplace(fieldKey, decoder);
		m_fieldDecodersSeriesByLabel.insert_or_assign(decoder.getLabel(), &m_fieldDecodersSeries.find(fieldKey)->second);
	}
	
	m_valueTransformerByFieldKeyParameter.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "ValueTransformersParam"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		ValueProcessing transformer(json::getObject(kvp, "Transformer"));
		m_valueTransformerByFieldKeyParameter.emplace(std::move(fieldKey), std::move(transformer));
	}

	m_valueTransformerByFieldKeyQuantity.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "ValueTransformersQuant"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		auto processors = 	json::getArray(_jsonObject, "Transformer");
		std::list<ValueProcessing> procList;
		for (uint32_t i = 0 ; i < processors.Size(); i++)
		{
			ValueProcessing transformer(json::getObject(processors, i));
			procList.push_back(transformer);
		}
		m_valueTransformerByFieldKeyQuantity.emplace(std::move(fieldKey), std::move(procList));
	}
}

const std::map<std::string, std::string>& ot::DataLakeAccessCfg::getQueriesByCollection() const
{
	 return m_collectionToQueryMap; 
}
void ot::DataLakeAccessCfg::addCollection(const std::string& _collection, const std::string& _query)
{
	 m_collectionToQueryMap.insert_or_assign(_collection, _query); 
}
void ot::DataLakeAccessCfg::addCollection(const std::string& _collection, std::string&& _query)
{
	 m_collectionToQueryMap.insert_or_assign(_collection, std::move(_query));
}
bool ot::DataLakeAccessCfg::hasCollection(const std::string& _collection) const
{
	 return m_collectionToQueryMap.find(_collection) != m_collectionToQueryMap.end(); 
}
const std::string& DataLakeAccessCfg::getQueryForCollection(const std::string& _collection) const
{
	return m_collectionToQueryMap.find(_collection)->second;
}

void ot::DataLakeAccessCfg::addSeriesMetadata(const std::string& _series, JsonDocument&& _metadata)
{
	 m_seriesMetadataMap.insert_or_assign(_series, std::move(_metadata)); 
}

bool ot::DataLakeAccessCfg::hasSeriesMetadata(const std::string& _series) const
{
	 return m_seriesMetadataMap.find(_series) != m_seriesMetadataMap.end();
}

JsonDocument& ot::DataLakeAccessCfg::getSeriesMetadata(const std::string& _series)
{
	 return m_seriesMetadataMap.at(_series); 
}

const JsonDocument& ot::DataLakeAccessCfg::getSeriesMetadata(const std::string& _series) const
{
	 return m_seriesMetadataMap.at(_series);
}

void ot::DataLakeAccessCfg::addFieldDecoderQuantity(const ot::DataPointDecoder& _decoder)
{
	m_fieldDecodersQuantity.insert_or_assign(_decoder.getFieldName(), _decoder);
	auto temp = &m_fieldDecodersQuantity.find(_decoder.getFieldName())->second;
	m_fieldDecodersParameterByLabel.insert_or_assign(_decoder.getLabel(), temp);
}

std::optional<ot::DataPointDecoder> ot::DataLakeAccessCfg::getFieldDecoderQuantity(const std::string& _fieldKey) const
{
	auto decoder = m_fieldDecodersQuantity.find(_fieldKey);
	if (decoder != m_fieldDecodersQuantity.end())
	{
		return decoder->second;
	}
	else
	{
		return std::nullopt;
	}

}

std::optional<DataPointDecoder> ot::DataLakeAccessCfg::getFieldDecoderQuantityByLabel(const std::string& _label) const
{
	auto decoder = m_fieldDecodersQuantityByLabel.find(_label);
	if (decoder != m_fieldDecodersQuantityByLabel.end())
	{
		return *decoder->second;
	}
	else
	{
		return std::nullopt;
	}
}

size_t ot::DataLakeAccessCfg::getNumberOfFieldDecoderParameter() const
{
	 return m_fieldDecodersParameter.size(); 
}

void ot::DataLakeAccessCfg::addFieldDecoderParameter(const DataPointDecoder& _decoder)
{
	m_fieldDecodersParameter.insert_or_assign(_decoder.getFieldName(), _decoder);
	m_fieldDecodersParameterByLabel.insert_or_assign(_decoder.getLabel(), &m_fieldDecodersParameter.find(_decoder.getFieldName())->second);
}

std::optional<ot::DataPointDecoder> ot::DataLakeAccessCfg::getFieldDecoderParameter(const std::string& _fieldKey) const
{
	auto decoder = m_fieldDecodersParameter.find(_fieldKey);
	if (decoder != m_fieldDecodersParameter.end())
	{
		return decoder->second;
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<DataPointDecoder> ot::DataLakeAccessCfg::getFieldDecoderParameterByLabel(const std::string& _label) const
{
	auto decoder = m_fieldDecodersParameterByLabel.find(_label);
	if (decoder != m_fieldDecodersParameterByLabel.end())
	{
		return *decoder->second;
	}
	else
	{
		return std::nullopt;
	}
}

void ot::DataLakeAccessCfg::addFieldDecoderSeries(const DataPointDecoder& _decoder)
{
	m_fieldDecodersSeries.insert_or_assign(_decoder.getFieldName(), _decoder);
	m_fieldDecodersSeriesByLabel.insert_or_assign(_decoder.getLabel(), &m_fieldDecodersSeries.find(_decoder.getFieldName())->second);

}

std::optional<ot::DataPointDecoder> ot::DataLakeAccessCfg::getFieldDecoderSeries(const std::string& _fieldKey) const
{
	auto decoder = m_fieldDecodersSeries.find(_fieldKey);
	if (decoder != m_fieldDecodersSeries.end())
	{
		return decoder->second;
	}
	else
	{
		return std::nullopt;
	}
}

const std::map<std::string, ot::DataPointDecoder>& ot::DataLakeAccessCfg::getAllFieldDecoderParameter() const
{
	return m_fieldDecodersParameter;
}

const std::map<std::string, DataPointDecoder*>& ot::DataLakeAccessCfg::getAllFieldDecoderParameterByLabel() const
{
	return m_fieldDecodersParameterByLabel;
}

const std::map<std::string, DataPointDecoder*>& ot::DataLakeAccessCfg::getAllFieldDecoderSeriesByLabel() const
{
	return m_fieldDecodersSeriesByLabel;
}

ot::ValueProcessing& ot::DataLakeAccessCfg::addParameterValueTransformer(const std::string& _fieldKey, const ValueProcessing& _valueTransformer)
{
	 return m_valueTransformerByFieldKeyParameter.insert_or_assign(_fieldKey, _valueTransformer).first->second;
}

ot::ValueProcessing& ot::DataLakeAccessCfg::addParameterValueTransformer(const std::string& _fieldKey, ValueProcessing&& _valueTransformer)
{
	 return m_valueTransformerByFieldKeyParameter.insert_or_assign(_fieldKey, std::move(_valueTransformer)).first->second;
}

bool ot::DataLakeAccessCfg::hasParameterValueTransformer(const std::string& _fieldKey) const
{
	 return m_valueTransformerByFieldKeyParameter.find(_fieldKey) != m_valueTransformerByFieldKeyParameter.end();
}

ot::ValueProcessing& ot::DataLakeAccessCfg::getParameterValueTransformer(const std::string& _fieldKey)
{
	 return m_valueTransformerByFieldKeyParameter.at(_fieldKey);
}

const ot::ValueProcessing& ot::DataLakeAccessCfg::getParameterValueTransformer(const std::string& _fieldKey) const
{
	 return m_valueTransformerByFieldKeyParameter.at(_fieldKey); 
}

void ot::DataLakeAccessCfg::addQuantityValueTransformer(const std::string& _fieldKey, const std::list<ValueProcessing>& _valueTransformers)
{
	m_valueTransformerByFieldKeyQuantity.insert_or_assign(_fieldKey, _valueTransformers);
}

std::list<ot::ValueProcessing> ot::DataLakeAccessCfg::getQuantityValueTransformer(const std::string& _key) const
{
	return m_valueTransformerByFieldKeyQuantity.at(_key);
}

void ot::DataLakeAccessCfg::setCollectionName(const std::string& _collectionName)
{
	m_collectionNameBase = _collectionName; 
}

const std::string& ot::DataLakeAccessCfg::getCollectionName() const
{
	 return m_collectionNameBase;
}

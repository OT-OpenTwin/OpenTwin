// @otlicense

// OpenTwin header
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"

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
		m_fieldDecodersParameter.emplace(std::move(fieldKey), std::move(decoder));
	}

	m_fieldDecodersQuantity.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecodersQuant"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecodersQuantity.emplace(std::move(fieldKey), std::move(decoder));
	}

	m_fieldDecodersSeries.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecodersSer"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecodersSeries.emplace(std::move(fieldKey), std::move(decoder));
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
// @otlicense

// OpenTwin header
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"

void ot::DataLakeAccessCfg::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
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
	
	_jsonObject.AddMember("Projection", JsonString(m_projection, _allocator), _allocator);

	JsonArray fieldDecoderArr;
	for (const auto& it : m_fieldDecoders)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Decoder", JsonObject(it.second, _allocator), _allocator);
		fieldDecoderArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("FieldDecoders", std::move(fieldDecoderArr), _allocator);

	JsonArray valueTransformerArr;
	for (const auto& it : m_valueTransformerByFieldKey)
	{
		JsonObject kvp;
		kvp.AddMember("Field", JsonString(it.first, _allocator), _allocator);
		kvp.AddMember("Transformer", JsonObject(it.second, _allocator), _allocator);
		valueTransformerArr.PushBack(std::move(kvp), _allocator);
	}
	_jsonObject.AddMember("ValueTransformers", std::move(valueTransformerArr), _allocator);
}

void ot::DataLakeAccessCfg::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
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

	m_projection = json::getString(_jsonObject, "Projection");

	m_fieldDecoders.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "FieldDecoders"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		DataPointDecoder decoder(json::getObject(kvp, "Decoder"));
		m_fieldDecoders.emplace(std::move(fieldKey), std::move(decoder));
	}

	m_valueTransformerByFieldKey.clear();
	for (const ConstJsonObject& kvp : json::getObjectList(_jsonObject, "ValueTransformers"))
	{
		std::string fieldKey = json::getString(kvp, "Field");
		ValueProcessing transformer(json::getObject(kvp, "Transformer"));
		m_valueTransformerByFieldKey.emplace(std::move(fieldKey), std::move(transformer));
	}
}
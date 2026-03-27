#include "OTDataStorage/DataLakeHelper.h"
#include "OTDataStorage/DataLakeAPI.h"
#include <bsoncxx/json.hpp>

#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"

const std::string DataLakeHelper::m_resultDataField = "Data";

ot::JsonDocument DataLakeHelper::executeQuery(const ot::DataLakeAccessCfg& _config, mongocxx::options::find _options)
{
	ot::JsonDocument result;
	ot::JsonArray data;

	auto startTime = std::chrono::high_resolution_clock::now();
	const std::string& collectionName = _config.getCollectionName();
	for (auto queryByCollection : _config.getQueriesByCollection())
	{
		
		const std::string& collectionEnding = queryByCollection.first;
		BsonViewOrValue query = bsoncxx::from_json(queryByCollection.second);

		if (collectionEnding.empty())
		{
			throw std::exception("No collection name provided");
		}
		DataStorageAPI::DataLakeAPI dataLakeAPI(collectionName, collectionEnding);

		DataStorageAPI::DataStorageResponse dbResponse = dataLakeAPI.searchInDataLakePartition(query, _options);

		if (dbResponse.getSuccess())
		{
			const std::string queryResponse = dbResponse.getResult();
			ot::JsonDocument doc;
			doc.fromJson(queryResponse);
			if (doc.HasMember("Documents"))
			{
				auto& allMongoDocuments = doc["Documents"];
				ot::json::mergeArrays(data, allMongoDocuments, result.GetAllocator());
			}
		}
	}

	result.AddMember(ot::JsonString(m_resultDataField, result.GetAllocator()), data, result.GetAllocator());
	ot::JsonDocument clearTextResult = createClearTextResult(_config,result);

	return clearTextResult;
}

void DataLakeHelper::createDefaultIndexes(DataStorageAPI::DataLakeAPI& _dataLakeAPI)
{
	std::vector<std::string> defaultIndexes = { "Quantity" ,"Series" };
	bsoncxx::builder::basic::document index{};
	for (const std::string& indexName : defaultIndexes)
	{
		index.append(bsoncxx::builder::basic::kvp(indexName, 1));
	}
	_dataLakeAPI.getCollection().create_index(index.view());
}

void DataLakeHelper::createDefaultIndexes(const std::string& _collectionName)
{
	DataStorageAPI::DataLakeAPI results(_collectionName + getResultCollectionEnding());
	createDefaultIndexes(results);
	DataStorageAPI::DataLakeAPI transformed(_collectionName + getTransformedCollectionEnding());
	createDefaultIndexes(transformed);
}

ot::JsonDocument DataLakeHelper::createClearTextResult(const ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults)
{
	ot::ConstJsonArray encodedEntries = ot::json::getArray(_databaseResults, m_resultDataField);
	ot::JsonDocument clearTextDoc;
	ot::JsonArray clearTextEntries;

	uint32_t numberOfDocuments = encodedEntries.Size();
	//Missing: unit transformations

	for (uint32_t i = 0; i < numberOfDocuments; i++)
	{
		auto singleMongoDocument = ot::json::getObject(encodedEntries, i);
		ot::JsonObject clearTextEntry;

		for (const auto& parameterDecoderByKey : _config.getAllFieldDecoderParameter())
		{
			const std::string& mongoDBFieldName = parameterDecoderByKey.first;
			const std::string& label = parameterDecoderByKey.second.getLabel();
			if (singleMongoDocument.HasMember(mongoDBFieldName.c_str()))
			{
				ot::JsonValue value(singleMongoDocument[mongoDBFieldName.c_str()], clearTextDoc.GetAllocator());
				ot::JsonString newKey(label, clearTextDoc.GetAllocator());
				clearTextEntry.AddMember(std::move(newKey), std::move(value), clearTextDoc.GetAllocator());
			}
		}

		std::string quantityID = std::to_string(singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetInt64());
		ot::JsonString quantityName(MetadataQuantity::getFieldName().c_str(), clearTextDoc.GetAllocator());
		auto dataPointDecoderQu = _config.getFieldDecoderQuantity(quantityID);
		if (!dataPointDecoderQu.has_value())
		{
			assert(false);
		}

		std::string label = dataPointDecoderQu.value().getFieldName();
		clearTextEntry.AddMember(std::move(quantityName), ot::JsonString(label, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());

		ot::JsonValue quantityValue(singleMongoDocument["Values"], clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(ot::JsonString("Values", clearTextDoc.GetAllocator()), quantityValue, clearTextDoc.GetAllocator());

		std::string seriesID = std::to_string(singleMongoDocument[MetadataSeries::getFieldName().c_str()].GetInt64());
		auto dataPointDecoderSer = _config.getFieldDecoderQuantity(seriesID);
		if (!dataPointDecoderSer.has_value())
		{
			assert(false);
		}
		label = dataPointDecoderSer.value().getLabel();
		ot::JsonString newKey(MetadataSeries::getFieldName(), clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(std::move(newKey), ot::JsonString(label, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());

		clearTextEntries.PushBack(clearTextEntry, clearTextDoc.GetAllocator());
	}
	clearTextDoc.AddMember(ot::JsonString(m_resultDataField, clearTextDoc.GetAllocator()), clearTextEntries, clearTextDoc.GetAllocator());
	return clearTextDoc;
}

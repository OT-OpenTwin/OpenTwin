#include "OTDataStorage/DataLakeHelper.h"
#include "OTDataStorage/DataLakeAPI.h"
#include <bsoncxx/json.hpp>

#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTDataStorage/CumulativeLogMessage.h"
#include "OTCore/JSON/JSONHelper.h"
const std::string DataLakeHelper::m_resultDataField = "Data";

ot::JsonDocument DataLakeHelper::executeQuery(const ot::DataLakeAccessCfg& _config, mongocxx::options::find _options, std::string& _log)
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
	ot::JsonDocument clearTextResult = createClearTextResult(_config,result, _log);

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

ot::JsonDocument DataLakeHelper::createClearTextResult(const ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults, std::string& _log)
{
	ot::ConstJsonArray encodedEntries = ot::json::getArray(_databaseResults, m_resultDataField);
	ot::JsonDocument clearTextDoc;
	ot::JsonArray clearTextEntries;

	uint32_t numberOfDocuments = encodedEntries.Size();
	CumulativeLogMessage cumulativeMessage;
	const std::string logPrefixDataModelInconsistency = "Data model inconsistency: ";
	ot::JSONToVariableConverter jsonToVariableConverter;
	ot::VariableToJSONConverter variableToJSONConverter;

	//Now we iterate over all documents and transform them into clear text entries. We also perform value transformations and check for invalidities between the extracted data and the metadata.
	for (uint32_t i = 0; i < numberOfDocuments; i++)
	{
		auto singleMongoDocument = ot::json::getObject(encodedEntries, i);
		ot::JsonObject clearTextEntry;

		for (const auto& parameterDecoderByKey : _config.getAllFieldDecoderParameter())
		{
			const std::string& mongoDBFieldName = parameterDecoderByKey.first;
			const std::string& label = parameterDecoderByKey.second.getLabel();
			const std::vector<std::string> units = parameterDecoderByKey.second.getTupleInstance().getTupleUnits();
			if (singleMongoDocument.HasMember(mongoDBFieldName.c_str()))
			{
				// First we convert the parameter UID into a readable label.
				ot::JsonString newKey(label, clearTextDoc.GetAllocator());
				
				//Now we do value transformations if necessary.
				ot::JsonValue value(singleMongoDocument[mongoDBFieldName.c_str()], clearTextDoc.GetAllocator());
				if (units.size() > 1)
				{
					bool typeIsConsistent = ot::json::isOfType(value, units.front());
					if (!typeIsConsistent)
					{
						cumulativeMessage.addLineToMessage(logPrefixDataModelInconsistency + "Value for field '" + label + "' has type '" + ot::json::getTypeName(value) + "' which is inconsistent with expected type '" + units.front());
					}
				}
				if (_config.hasParameterValueTransformer(mongoDBFieldName))
				{
					const ot::ValueProcessing& valueProcessing =_config.getParameterValueTransformer(mongoDBFieldName);
					if (!valueProcessing.isEmpty())
					{
						ot::Variable temp =	jsonToVariableConverter(value);
						ot::Variable transformed = valueProcessing.execute(temp);
						ot::JsonValue transformedEntry = variableToJSONConverter(transformed, clearTextDoc.GetAllocator());
						clearTextEntry.AddMember(std::move(newKey), std::move(transformedEntry), clearTextDoc.GetAllocator());
					}
					else
					{
						clearTextEntry.AddMember(std::move(newKey), std::move(value), clearTextDoc.GetAllocator());
					}
				}
				else
				{
					assert(false); // There should always be a value transformer.
					clearTextEntry.AddMember(std::move(newKey), std::move(value), clearTextDoc.GetAllocator());
				}	
			}
		}
		// Now we transform the quantity UID into a reable name.
		std::string quantityID = std::to_string(singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetInt64());
		ot::JsonString quantityName(MetadataQuantity::getFieldName().c_str(), clearTextDoc.GetAllocator());
		auto dataPointDecoderQu = _config.getFieldDecoderQuantity(quantityID);
		if (!dataPointDecoderQu.has_value())
		{
			assert(false);
		}
		std::string quantityLabel = dataPointDecoderQu.value().getLabel();
		clearTextEntry.AddMember(std::move(quantityName), ot::JsonString(quantityLabel, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());


		const size_t dataDimension = dataPointDecoderQu.value().getDimension().size();

		if (dataDimension == 1)
		{
			ot::JsonValue quantityValue (singleMongoDocument["Values"], clearTextDoc.GetAllocator());
			// Next is the quantity value
			const auto& quantityTupleInstance =	dataPointDecoderQu.value().getTupleInstance();
			
			// First we check if the expected value type was returned.
			std::string typeName = "";
			if (quantityTupleInstance.isSingle())
			{
				typeName = quantityTupleInstance.getTupleElementDataTypes().front();
			}
			else
			{
				typeName = quantityTupleInstance.getTupleTypeName();

			}
			const bool typeConsistent = ot::json::isOfType(quantityValue, typeName);
			if (!typeConsistent)
			{
				cumulativeMessage.addLineToMessage(logPrefixDataModelInconsistency + "Value for field '" + quantityLabel + "' has type '" + ot::json::getTypeName(quantityValue) + "' which is inconsistent with expected type '" + typeName);
			}

			//Now we do the value transformations.
			const std::list<ot::ValueProcessing>& valueProcessing = _config.getQuantityValueTransformer(quantityID);
			if (quantityTupleInstance.isSingle())
			{
				// Multiple value processings are only there for tuples
				const ot::ValueProcessing& singleEntryProcessing = valueProcessing.front();
				if (singleEntryProcessing.isEmpty())
				{
					clearTextEntry.AddMember(ot::JsonString("Values", clearTextDoc.GetAllocator()), std::move(quantityValue), clearTextDoc.GetAllocator());
				}
				else
				{
					ot::Variable temp = jsonToVariableConverter(quantityValue);
					ot::Variable transformed = singleEntryProcessing.execute(temp);
					ot::JsonValue transformedEntry = variableToJSONConverter(transformed, clearTextDoc.GetAllocator());
					clearTextEntry.AddMember(ot::JsonString("Values", clearTextDoc.GetAllocator()), std::move(transformedEntry), clearTextDoc.GetAllocator());
				}
			}
			else
			{
				// Iterate over the tuple elements and perform a value transformation on each of them.
				//Currently only complex numbers. 
				assert(quantityTupleInstance.getTupleTypeName() == ot::ComplexNumbers::getTypeName());
				ot::Variable temp = jsonToVariableConverter(quantityValue);
				assert(temp.isComplex());
				std::complex<double> complexValue = temp.getComplex();
				double firstValue = complexValue.real();
				double secondValue = complexValue.imag();

				auto& firstProcessing =	valueProcessing.front();
				if (!firstProcessing.isEmpty())
				{
					ot::Variable var = firstProcessing.execute(firstValue);
					firstValue = var.getDouble();
				}
				auto& secondProcessing = valueProcessing.back();
				if (!secondProcessing.isEmpty())
				{
					ot::Variable var = secondProcessing.execute(secondValue);
					secondValue = var.getDouble();
				}
				std::complex<double> transformedComplex(firstValue, secondValue);
				ot::JsonValue transformedEntry = variableToJSONConverter(ot::Variable(transformedComplex), clearTextDoc.GetAllocator());
				clearTextEntry.AddMember(ot::JsonString("Values", clearTextDoc.GetAllocator()), transformedEntry, clearTextDoc.GetAllocator());
			}
		}
		else
		{
			assert(false); // Currently matrices are not supported.
		}

		// Lastly we transform the series UID into a readable name.
		std::string seriesID = std::to_string(singleMongoDocument[MetadataSeries::getFieldName().c_str()].GetInt64());
		auto dataPointDecoderSer = _config.getFieldDecoderSeries(seriesID);
		if (!dataPointDecoderSer.has_value())
		{
			assert(false);
		}
		const std::string seriesLabel = dataPointDecoderSer.value().getLabel();
		ot::JsonString newKey(MetadataSeries::getFieldName(), clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(std::move(newKey), ot::JsonString(seriesLabel, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());

		clearTextEntries.PushBack(clearTextEntry, clearTextDoc.GetAllocator());
	}
	clearTextDoc.AddMember(ot::JsonString(m_resultDataField, clearTextDoc.GetAllocator()), clearTextEntries, clearTextDoc.GetAllocator());

	_log = cumulativeMessage.getMessage();
	return clearTextDoc;
}

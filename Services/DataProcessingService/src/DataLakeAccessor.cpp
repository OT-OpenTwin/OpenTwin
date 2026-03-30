#include "DataLakeAccessor.h"
#include "Application.h"
#include "QueryDescriptionBuilder.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTDataStorage/DataLakeAPI.h"

#include "OTResultDataAccess/QuantityContainer.h"
#include "OTCore/Tuple/TupleFactory.h"
#include <tuple>
#include "ValueProcessingChainBuilder.h"
#include "OTCore/DataFilter/RegexHelper.h"
#include "OTCore/JSON/JSONVectoriser.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTDataStorage/DataLakeHelper.h"
#include "OTCore/EntityName.h"

void DataLakeAccessor::accessPartition(const std::string& _collectionName)
{
	m_collectionName = _collectionName;
	m_queryDescriptionsParameters.clear();
	m_queryDescriptionsQuantities.clear();
	m_queryDescriptionsSeries.clear();
	if (m_resultCollectionMetadataAccess != nullptr)
	{
		delete m_resultCollectionMetadataAccess;
		m_resultCollectionMetadataAccess = nullptr;
	}
	
	bool isCrossCollection = Application::instance()->getCollectionName() != _collectionName;
	m_resultCollectionMetadataAccess = new ResultCollectionMetadataAccess(_collectionName, Application::instance()->getModelComponent(), isCrossCollection);
}

void DataLakeAccessor::createQueryDescriptionsSeries(const std::list<ot::ValueComparisonDescription>& _valueComparisons, const std::string& _seriesLabel)
{
	if (m_resultCollectionMetadataAccess == nullptr)
	{
		throw std::exception(("Failed to access metadata of collection: " + m_collectionName).c_str());
	}
	const MetadataSeries* series = nullptr;

	//First we search the series by name, or if it failes we assume a regex expression and conduct another search.
	std::list<const MetadataSeries*> matchingSeries;
	if (_seriesLabel != "")
	{
		series = m_resultCollectionMetadataAccess->findMetadataSeries(_seriesLabel);
		if (series != nullptr)
		{
			matchingSeries.push_back(series);
		}
		else
		{
			std::list<std::string> allSeriesLabels = m_resultCollectionMetadataAccess->listAllSeriesNames();
			RegexHelper::applyRegexFilter(allSeriesLabels, _seriesLabel);
			for (const std::string& seriesName : allSeriesLabels)
			{
				const MetadataSeries* series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesName);
				assert(series != nullptr);
				matchingSeries.push_back(series);
			}
		}
	}
	else
	{
		//If no label is selected, we take all series in consideration to allow a query that aligns with the current model state.
		const std::list<MetadataSeries>& allSeries = m_resultCollectionMetadataAccess->getMetadataCampaign().getSeriesMetadata();
		for (const MetadataSeries& series : allSeries)
		{
			matchingSeries.push_back(&series);
		}
	}

	//Now we apply metadata filter and sort out the list of series
	auto oneSeriesIt = matchingSeries.begin();
	while (oneSeriesIt != matchingSeries.end())
	{
		const ot::JsonDocument& metadata = (*oneSeriesIt)->getMetadata();
		bool match = true;
		for (auto& metadataQuery : _valueComparisons)
		{
			if (metadataQuery.getComparator() != "" && metadataQuery.getName() != "" && metadataQuery.getValue() != "")
			{
				const std::string fieldName = metadataQuery.getName();
				const ot::JsonValue& fieldValue = ot::JSONVectoriser::getValue(metadata, fieldName);
				const std::string temp = ot::json::toJson(fieldValue);
				if (fieldValue.IsString())
				{
					if (metadataQuery.getComparator() != "=")
					{
						throw std::exception(("Comparison of strings are only possible with an equality check. Field: " + metadataQuery.getName()).c_str());
					}
					else
					{
						const std::string actualSeriesMetadataValue = fieldValue.GetString();
						match &= actualSeriesMetadataValue == metadataQuery.getValue();
					}
				}
				else if (!fieldValue.IsObject() && !fieldValue.IsArray())
				{
					match &= compare(metadataQuery, fieldValue);
				}
				else
				{
					throw std::exception(("Metadata query targets an object, which is not yet supported. Field: " + metadataQuery.getName()).c_str());
				}
			}
			else
			{
				//OT_LOG_W("Skipping metadata query since no comparator was selected. Field: " + metadataQuery.getName());
			}
			if (!match)
			{
				break;
			}
		}

		if (match)
		{
			//The series matches the metadata query. Check the next metadata query.
			oneSeriesIt++;
		}
		else
		{
			oneSeriesIt = matchingSeries.erase(oneSeriesIt);
		}

	}

	std::list<ot::QueryDescription> seriesQueries;
	for (const MetadataSeries* series : matchingSeries)
	{
		ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(series);
		seriesQueries.push_back(queryDescription);
	}

	m_queryDescriptionsSeries = seriesQueries;
}

void DataLakeAccessor::createQueryDescriptionsParameter(const std::list<ot::ValueComparisonDescription>& _valueComparisons)
{
	if (m_resultCollectionMetadataAccess == nullptr)
	{
		throw std::exception(("Failed to access metadata of collection: " + m_collectionName).c_str());
	}
	const auto& parametersByLabel = m_resultCollectionMetadataAccess->getMetadataCampaign().getMetadataParameterByLabel();
	std::list<ot::QueryDescription> allParameterQueries;
	for (const ot::ValueComparisonDescription& query : _valueComparisons)
	{
		if (!query.getName().empty() && !query.getComparator().empty() && !query.getValue().empty())
		{
			const std::string parameterLabel = query.getName();
			auto parameterByLabel = parametersByLabel.find(parameterLabel);
			if (parameterByLabel == parametersByLabel.end())
			{
				OT_LOG_E("Trying to query for: " + parameterLabel + " which was not found in the campaign.");
				assert(0);
			}
			else
			{
				ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(query, parameterByLabel->second);
				allParameterQueries.push_back(queryDescription);
			}
		}
	}
	addValueDescriptionsParameters(allParameterQueries);
}

void DataLakeAccessor::createQueryDescriptionQuantity(const ot::ValueComparisonDescription& _valueComparisons)
{
	if (m_resultCollectionMetadataAccess == nullptr)
	{
		throw std::exception(("Failed to access metadata of collection: " + m_collectionName).c_str());
	}
	if (_valueComparisons.getName() != "" && _valueComparisons.getComparator() != "" && _valueComparisons.getValue() != "" || _valueComparisons.getName() != "" && _valueComparisons.getComparator() == "")
	{
		//First we try to find the corresponding metadata
		std::list<const MetadataQuantity*> viableQuantities;
		const auto selectedQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(_valueComparisons.getName());
		if (selectedQuantity != nullptr)
		{
			viableQuantities.push_back(selectedQuantity);
		}
		else
		{
			// The selected quantity name may be a regex expression
			auto allQuantityLabel = m_resultCollectionMetadataAccess->listAllQuantityLabels();
			RegexHelper::applyRegexFilter(allQuantityLabel, _valueComparisons.getName());
			for (const std::string& viableQuantityLabel : allQuantityLabel)
			{
				auto viableQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(viableQuantityLabel);
				viableQuantities.push_back(viableQuantity);
			}
		}

		if (viableQuantities.size() == 0)
		{
			throw std::exception("Given quantity label expression does not match any of the possible options.");
		}
		else
		{
			std::list<ot::QueryDescription> allQuantityQueries;
			ot::UIDList relatedParameterUIDs;
			for (const MetadataQuantity* viableQuantity : viableQuantities)
			{
				relatedParameterUIDs.insert(relatedParameterUIDs.begin(), viableQuantity->dependingParameterIds.begin(), viableQuantity->dependingParameterIds.end());
				ot::ValueComparisonDescription comparisonDescription = _valueComparisons;
				comparisonDescription.setName(viableQuantity->quantityName);
				ot::TupleInstance tupleInstance = _valueComparisons.getTupleInstance();
				tupleInstance.setTupleTypeName(viableQuantity->m_tupleDescription.getTupleTypeName());
				comparisonDescription.setTupleInstance(tupleInstance);

				ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(comparisonDescription, viableQuantity);
				allQuantityQueries.push_back(queryDescription);
			}
			m_queryDescriptionsQuantities = allQuantityQueries;

			std::list<ot::QueryDescription> allRelatedParameterQueries;
			relatedParameterUIDs.sort();
			relatedParameterUIDs.unique();
			for (ot::UID parameterUID : relatedParameterUIDs)
			{
				const MetadataParameter* relatedParameter = m_resultCollectionMetadataAccess->findMetadataParameter(parameterUID);
				ot::ValueComparisonDescription comparisonDescription;
				ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(comparisonDescription, relatedParameter);
				allRelatedParameterQueries.push_back(queryDescription);
			}
			addValueDescriptionsParameters(allRelatedParameterQueries);
		}
	}
	else
	{
		//Maybe a log of some kind ?
	}
}

void DataLakeAccessor::addValueDescriptionsParameters(const std::list<ot::QueryDescription>& _queryDescriptions)
{
	for (const ot::QueryDescription& queryDescription : _queryDescriptions)
	{
		bool exists = false;
		for (const ot::QueryDescription& alreadyStoredDescription : m_queryDescriptionsParameters)
		{
			if (queryDescription.getQueryTargetDescription().getMongoDBFieldName() == alreadyStoredDescription.getQueryTargetDescription().getMongoDBFieldName())
			{
				exists = true;
				break;
			}
		}
		if (!exists)
		{
			m_queryDescriptionsParameters.push_back(queryDescription);
		}
	}
}

ot::JsonDocument DataLakeAccessor::executeQuery(mongocxx::options::find _options)
{
	if (m_collectionName.empty())
	{
		throw std::exception("No collection name provided");
	}

	BsonViewOrValue resultCollectionQuery, transformedCollectionQuery;
	createQueries(resultCollectionQuery,transformedCollectionQuery);
	
	const std::string debugQuery = bsoncxx::to_json(resultCollectionQuery.view());

	ot::JsonDocument result;
	ot::JsonArray data;

	auto startTime = std::chrono::high_resolution_clock::now();
	if (!resultCollectionQuery.view().empty())
	{
		DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName);	
		DataStorageAPI::DataStorageResponse dbResponse = resultCollectionAccess.searchInDataLakePartition(resultCollectionQuery, _options);

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

	if (!transformedCollectionQuery.view().empty())
	{
		DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName, m_transformedCollectionEnding);
		DataStorageAPI::DataStorageResponse dbResponse = resultCollectionAccess.searchInDataLakePartition(transformedCollectionQuery, _options);

		if (dbResponse.getSuccess())
		{
			const std::string queryResponse = dbResponse.getResult();
			ot::JsonDocument doc;
			doc.fromJson(queryResponse);
			if (doc.HasMember("Documents"))
			{
				auto& allMongoDocuments = doc["Documents"];
				size_t numberOfReturnedDocs = allMongoDocuments.Size();
				ot::json::mergeArrays(data, allMongoDocuments, result.GetAllocator());
				size_t temp = data.Size();

			}
		}
	}

	result.AddMember(ot::JsonString(m_resultDataField,result.GetAllocator()), data, result.GetAllocator());
	ot::JsonDocument clearTextResult = createClearTextResult(result);

	return clearTextResult;
}

ot::DataLakeAccessCfg DataLakeAccessor::createConfig()
{
	if (m_queryDescriptionsQuantities.size() == 0)
	{
		//"No quantity to query selected."
		return ot::DataLakeAccessCfg();
	}

	if (m_collectionName.empty())
	{
		throw std::exception("No collection name provided");
	}
	
	if (m_resultCollectionMetadataAccess == nullptr)
	{
		throw std::exception(("Failed to access metadata of collection: " + m_collectionName).c_str());
	}

	//DataLakeHelper::createDefaultIndexes(m_collectionName);

	BsonViewOrValue resultCollectionQuery, transformedCollectionQuery;
	createQueries(resultCollectionQuery, transformedCollectionQuery);

	ot::DataLakeAccessCfg config;
	if (!resultCollectionQuery.view().empty())
	{
		
		config.addCollection(m_resultCollectionEnding,bsoncxx::to_json(resultCollectionQuery));
	}
	if (!transformedCollectionQuery.view().empty())
	{
		config.addCollection(m_transformedCollectionEnding, bsoncxx::to_json(transformedCollectionQuery));
	}
	config.setCollectionName(m_collectionName);

	std::list<std::string> seriesNames = m_resultCollectionMetadataAccess->listAllSeriesNames();
	for (const std::string& seriesName : seriesNames)
	{
		const MetadataSeries* series =	m_resultCollectionMetadataAccess->findMetadataSeries(seriesName);
		const ot::JsonDocument& metadata =	series->getMetadata();
		ot::JsonDocument doc;
		doc.CopyFrom(metadata, doc.GetAllocator());
		config.addSeriesMetadata(std::to_string(series->getSeriesIndex()), std::move( doc));
	}

	for (auto& parameterQueryDescription : m_queryDescriptionsParameters)
	{
		ot::DataPointDecoder dataPointDecoder;
		dataPointDecoder.setFieldName(parameterQueryDescription.getQueryTargetDescription().getMongoDBFieldName());
		dataPointDecoder.setLabel(parameterQueryDescription.getQueryTargetDescription().getLabel());
		dataPointDecoder.setDimension(parameterQueryDescription.getQueryTargetDescription().getDimensions());
		dataPointDecoder.setTupleInstance(parameterQueryDescription.getQueryTargetDescription().getTupleInstance());
		config.addFieldDecoderParameter(dataPointDecoder);
	}
	for (auto& quantityQueryDescription : m_queryDescriptionsQuantities)
	{
		ot::DataPointDecoder dataPointDecoder;
		dataPointDecoder.setFieldName(quantityQueryDescription.getQueryTargetDescription().getMongoDBFieldName());
		dataPointDecoder.setLabel(quantityQueryDescription.getQueryTargetDescription().getLabel());
		dataPointDecoder.setDimension(quantityQueryDescription.getQueryTargetDescription().getDimensions());
		dataPointDecoder.setTupleInstance(quantityQueryDescription.getQueryTargetDescription().getTupleInstance());
		config.addFieldDecoderQuantity(dataPointDecoder);
	}
	for (auto& seriesQueryDescription : m_queryDescriptionsSeries)
	{
		ot::DataPointDecoder dataPointDecoder;
		dataPointDecoder.setFieldName(seriesQueryDescription.getValueComparisonDescription().getValue());
		std::string label = m_resultCollectionMetadataAccess->findMetadataSeries(std::stoull(seriesQueryDescription.getValueComparisonDescription().getValue()))->getName();
		label = ot::EntityName::getSubName(label).value();
		dataPointDecoder.setLabel(label);
		dataPointDecoder.setDimension(seriesQueryDescription.getQueryTargetDescription().getDimensions());
		dataPointDecoder.setTupleInstance(seriesQueryDescription.getQueryTargetDescription().getTupleInstance());
		config.addFieldDecoderSeries(dataPointDecoder);
	}

	for (auto& valueTransformer : m_inverseParameterTransformationsByFieldKey)
	{
		config.addParameterValueTransformer(valueTransformer.first,valueTransformer.second);
	}
	for (auto& valueTransformer : m_inverseQuantityTransformationsByFieldKey)
	{
		config.addQuantityValueTransformer(valueTransformer.first, valueTransformer.second);
	}

	return config;
}

ot::JsonDocument DataLakeAccessor::executeQuery(ot::DataLakeAccessCfg& _config, mongocxx::options::find _options)
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
		

		DataStorageAPI::DataLakeAPI resultCollectionAccess(collectionName, collectionEnding);
		DataStorageAPI::DataStorageResponse dbResponse = resultCollectionAccess.searchInDataLakePartition(query, _options);

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
	ot::JsonDocument clearTextResult = createClearTextResult(result);

	return clearTextResult;
}

void DataLakeAccessor::createQueries(BsonViewOrValue& _resultCollectionQuery, BsonViewOrValue& _transformedCollectionQuery)
{
	BsonViewOrValue seriesQuery = generateSeriesQuery();
	std::list<BsonViewOrValue> parameterQueries = generateParameterQueries();
	
	BsonViewOrValue resultCollectionQuantityQuery;
	BsonViewOrValue transformedCollectionQuantityQuery;
	generateQuantityQueries(resultCollectionQuantityQuery, transformedCollectionQuantityQuery);
	
	std::list<BsonViewOrValue> allQueriesBase;
	if (!seriesQuery.view().empty())
	{
		allQueriesBase.push_back(std::move(seriesQuery));
	}
	allQueriesBase.insert(allQueriesBase.end(), parameterQueries.begin(), parameterQueries.end());

	AdvancedQueryBuilder builder;
	if (!transformedCollectionQuantityQuery.view().empty())
	{
		std::list<BsonViewOrValue> allQueries = allQueriesBase;
		allQueries.push_back(transformedCollectionQuantityQuery);
		_transformedCollectionQuery = builder.connectWithAND(std::move(allQueries));
		const std::string temp = bsoncxx::to_json(_transformedCollectionQuery);
	}
	
	if (!resultCollectionQuantityQuery.view().empty())
	{
		std::list<BsonViewOrValue> allQueries = allQueriesBase;
		allQueries.push_back(resultCollectionQuantityQuery);
		_resultCollectionQuery = builder.connectWithAND(std::move(allQueries));
		const std::string temp = bsoncxx::to_json(_resultCollectionQuery);

	}
}

bool DataLakeAccessor::transformationNecessary(const ot::TupleInstance& _storedFormat, const ot::TupleInstance& _queryFormat)
{
	if (!(_storedFormat == _queryFormat))
	{
		if (_storedFormat.getTupleFormatName() != _queryFormat.getTupleFormatName())
		{
			return true; // E.g. polar
		}
		else
		{
			return false; //Actually, here we would need to define a scaling
		}
	}
	else
	{
		return false;
	}
}

bool DataLakeAccessor::alreadyStoredTransformation(const ot::QueryDescription& _queryDescription)
{
	DataStorageAPI::DataLakeAPI transformedCollectionAccess(m_collectionName, m_transformedCollectionEnding);

	const std::string fieldValue = _queryDescription.getQueryTargetDescription().getMongoDBFieldName();
	ot::TupleInstance instance;
	instance.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName() });
	ot::ValueComparisonDescription quantitySelection(MetadataQuantity::getFieldName(), "=", fieldValue, instance);
	AdvancedQueryBuilder builder;
	BsonViewOrValue queryForQuantity = builder.createComparison(quantitySelection);
	
	int64_t countInTransformedCollection = transformedCollectionAccess.countInDataLakePartition(queryForQuantity);
	if (countInTransformedCollection != 0)
	{
		DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName);
		int64_t countInResultCollection = resultCollectionAccess.countInDataLakePartition(queryForQuantity);
		if (countInResultCollection != countInTransformedCollection)
		{
			//Here we may have additional documents of the searched quantity in the collection. Better to do a clean reset and start anew.
			transformedCollectionAccess.getCollection().delete_many(queryForQuantity);
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void DataLakeAccessor::storeTransformation(const ot::QueryDescription& _queryDescription)
{
	// First we gather the data from the result collection
	const std::string fieldValue = _queryDescription.getQueryTargetDescription().getMongoDBFieldName();
	ot::TupleInstance instance;
	instance.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName() });
	ot::ValueComparisonDescription quantitySelection(MetadataQuantity::getFieldName(), "=", fieldValue, instance);
	AdvancedQueryBuilder builder;
	BsonViewOrValue queryForQuantity = builder.createComparison(quantitySelection);
	
	DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName);
	mongocxx::options::find options;
	auto response =	resultCollectionAccess.searchInDataLakePartition(queryForQuantity,options);
	if (response.getSuccess())
	{
		const std::string queryResponse = response.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allMongoDocuments = ot::json::getArray(doc, "Documents");

		const uint32_t numberOfDocuments = allMongoDocuments.Size();
		if (numberOfDocuments == 0)
		{
			throw std::exception("Query returned nothing.\n");
		}

		// Now we assemble a transformation function to change the format and turn the data into an SI base unit.
		const ot::QueryTargetDescription& storedDataDescr = _queryDescription.getQueryTargetDescription();
		const ot::ValueComparisonDescription& queryDescription =	_queryDescription.getValueComparisonDescription();

		// Transformation into SI base unit
		ValueProcessingChainBuilder valueProcessingChainBuilder;
		const auto& units = storedDataDescr.getTupleInstance().getTupleUnits();
		std::vector<ot::ValueProcessing> toSIConversions;
		toSIConversions.reserve(units.size());
		for (const std::string& unit : units)
		{
			ot::ValueProcessing processing = valueProcessingChainBuilder.buildToSIChain(unit);
			toSIConversions.push_back(std::move(processing));
		}
		std::function<std::pair<double, double>(double, double)> transformToSI;
		if (toSIConversions[0].isEmpty() && toSIConversions[1].isEmpty())
		{
			transformToSI = [](double _first, double _second)->std::pair<double, double>
				{
					return { _first, _second };
				};
		}
		else if (!toSIConversions[0].isEmpty() && !toSIConversions[1].isEmpty())
		{
			transformToSI = [&toSIConversions](double _first, double _second)->std::pair<double, double>
				{
					_first = toSIConversions[0].execute(_first).getDouble();
					_second= toSIConversions[1].execute(_second).getDouble();
					return { _first, _second };
				};
		}
		else if(!toSIConversions[0].isEmpty())
		{
			transformToSI = [&toSIConversions](double _first, double _second)->std::pair<double, double>
				{
					_first = toSIConversions[0].execute(_first).getDouble();
					return { _first, _second };
				};
		}
		else
		{
			assert(!toSIConversions[1].isEmpty());
			transformToSI = [&toSIConversions](double _first, double _second)->std::pair<double, double>
				{
					_second = toSIConversions[1].execute(_second).getDouble();
					return { _first, _second };
				};
		}

		// Now comes the tuple format transformation.
		const std::string storedFormatName = storedDataDescr.getTupleInstance().getTupleFormatName();
		const std::string queryFormatName = queryDescription.getTupleInstance().getTupleFormatName();
		
		const std::string storedTupleType = storedDataDescr.getTupleInstance().getTupleTypeName();
		ot::TupleDescription* tupleDescription = TupleFactory::create(storedTupleType);
		ot::TupleDescriptionComplex* complexDescr = dynamic_cast<ot::TupleDescriptionComplex*>(tupleDescription);
		std::function<std::pair<double, double>(double, double)> transformFormat;

		if (queryFormatName == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian))
		{
			assert(storedTupleType == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar));
			transformFormat =
				[](double _first, double _second) -> std::pair<double, double>
				{
					std::complex<double> transformed = ot::ComplexNumberConversion::polarToCartesian(_first, _second);
					return { transformed.real(), transformed.imag() };
				};
		}
		else
		{
			assert(storedTupleType == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian));
			transformFormat =
				[](double _first, double _second) -> std::pair<double, double>
				{
					std::complex<double> reIm(_first, _second);

					auto transformed = ot::ComplexNumberConversion::cartesianToPolar(reIm);
					return { transformed.m_firstValue, transformed.m_secondValue };
				};
		}


		ot::JsonDocument dataDoc;
		
		DataStorageAPI::DataLakeAPI transformationCollectionAccess(m_collectionName, m_transformedCollectionEnding);
		for (uint32_t i = 0; i < numberOfDocuments; i++)
		{
			ot::ConstJsonObject singleMongoDocument = ot::json::getObject(allMongoDocuments, i);

			ot::ConstJsonArray complexValue = ot::json::getArray(singleMongoDocument, QuantityContainer::getFieldName());
			double first = ot::json::getDouble(complexValue, 0);
			double second = ot::json::getDouble(complexValue, 1);
			std::pair<double, double> inSIBaseUnit = transformToSI(first, second);
			std::pair<double,double> transFormedValues = transformFormat(inSIBaseUnit.first, inSIBaseUnit.second);
			bsoncxx::builder::basic::array values;
			values.append(transFormedValues.first);
			values.append(transFormedValues.second);

			bsoncxx::document::value parsed = bsoncxx::from_json(ot::json::toJson(singleMongoDocument));
			bsoncxx::builder::basic::document transformedDocument{};

			for (auto&& elem : parsed.view()) {
				if (elem.key() == QuantityContainer::getFieldName()) 
				{
					// Replace "x" with the double array y
					transformedDocument.append(bsoncxx::builder::basic::kvp(QuantityContainer::getFieldName(), values));
				}
				else {
					// Copy all other fields unchanged
					transformedDocument.append(bsoncxx::builder::basic::kvp(elem.key(), elem.get_value()));
				}
			}
			transformationCollectionAccess.insertDocumentToDataLakePartition(transformedDocument, false, true);
		}
		transformationCollectionAccess.flushQueuedData();
	}
}

ot::JsonDocument DataLakeAccessor::createClearTextResult(const ot::JsonDocument& _databaseResults)
{
	ot::ConstJsonArray encodedEntries =	ot::json::getArray(_databaseResults, m_resultDataField);
	ot::JsonDocument clearTextDoc;
	ot::JsonArray clearTextEntries;

	auto mongoKeyToLabelQuantity = createFieldKeyLabelLookup(m_queryDescriptionsQuantities);
	auto mongoKeyToLabelSeries = createFieldKeyLabelLookup(m_queryDescriptionsSeries);
	uint32_t numberOfDocuments = encodedEntries.Size();
	//Missing: unit transformations
	for (uint32_t i = 0; i < numberOfDocuments; i++)
	{
		auto singleMongoDocument = ot::json::getObject(encodedEntries, i);
		ot::JsonObject clearTextEntry;
		for (const ot::QueryDescription& parameter : m_queryDescriptionsParameters)
		{
			const std::string& mongoDBFieldName =  parameter.getQueryTargetDescription().getMongoDBFieldName();
			const std::string& label = parameter.getQueryTargetDescription().getLabel();
			if (singleMongoDocument.HasMember(mongoDBFieldName.c_str()))
			{
				ot::JsonValue value(singleMongoDocument[mongoDBFieldName.c_str()], clearTextDoc.GetAllocator());
				ot::JsonString newKey(label, clearTextDoc.GetAllocator());
				clearTextEntry.AddMember(std::move(newKey), std::move(value), clearTextDoc.GetAllocator());
			}
		}

		std::string quantityID = std::to_string(singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetInt64());
		ot::JsonString quantityName(MetadataQuantity::getFieldName().c_str(), clearTextDoc.GetAllocator());
		auto quantityLabelByMongoField =	mongoKeyToLabelQuantity.find(quantityID);
		const std::string label = quantityLabelByMongoField->second;
		clearTextEntry.AddMember(std::move(quantityName), ot::JsonString(label, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());
		
		ot::JsonValue quantityValue(singleMongoDocument[QuantityContainer::getFieldName().c_str()], clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(ot::JsonString(QuantityContainer::getFieldName().c_str(), clearTextDoc.GetAllocator()), quantityValue, clearTextDoc.GetAllocator());
					
		std::string seriesID = std::to_string(singleMongoDocument[MetadataSeries::getFieldName().c_str()].GetInt64());
		auto seriesLabelByMongoField = mongoKeyToLabelSeries.find(seriesID);
		ot::JsonString newKey(MetadataSeries::getFieldName(), clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(std::move(newKey), ot::JsonString(label, clearTextDoc.GetAllocator()), clearTextDoc.GetAllocator());
		
		clearTextEntries.PushBack(clearTextEntry, clearTextDoc.GetAllocator());
	}
	clearTextDoc.AddMember(ot::JsonString(m_resultDataField, clearTextDoc.GetAllocator()), clearTextEntries, clearTextDoc.GetAllocator());
	return clearTextDoc;
}

ot::JsonDocument DataLakeAccessor::createClearTextResult(ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults)
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

		ot::JsonValue quantityValue(singleMongoDocument[QuantityContainer::getFieldName().c_str()], clearTextDoc.GetAllocator());
		clearTextEntry.AddMember(ot::JsonString(QuantityContainer::getFieldName().c_str(), clearTextDoc.GetAllocator()), quantityValue, clearTextDoc.GetAllocator());

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

std::map<std::string, std::string> DataLakeAccessor::createFieldKeyLabelLookup(const std::list<ot::QueryDescription>& _queryDescriptions)
{
	std::map<std::string, std::string> lookUp;
	for (const ot::QueryDescription& _queryDescription : _queryDescriptions)
	{
		const std::string mongoDBKey = _queryDescription.getQueryTargetDescription().getMongoDBFieldName();
		const std::string fieldLabel = _queryDescription.getQueryTargetDescription().getLabel();
		lookUp[mongoDBKey] = fieldLabel;
	}

	return lookUp;
}

BsonViewOrValue DataLakeAccessor::generateSeriesQuery()
{
	AdvancedQueryBuilder builder;

	std::list<BsonViewOrValue> comparisons;

	for (ot::QueryDescription& queryDescription : m_queryDescriptionsSeries)
	{
		ot::ValueComparisonDescription selectedQuery = queryDescription.getValueComparisonDescription();
		if (!selectedQuery.getComparator().empty() && !selectedQuery.getValue().empty() && !selectedQuery.getName().empty())
		{
			BsonViewOrValue comparison = builder.createComparison(selectedQuery);
			comparisons.push_back(comparison);
			const std::string debugQuery = bsoncxx::to_json(comparison);
		}
	}
	
	BsonViewOrValue seriesQuery;
	if (comparisons.size() > 1)
	{
		seriesQuery = builder.connectWithOR(std::move(comparisons));
	}
	else if (comparisons.size() == 1)
	{

		seriesQuery = std::move(comparisons.front());
	}
	const std::string debugQuery = bsoncxx::to_json(seriesQuery);
	return seriesQuery;
}

std::list<BsonViewOrValue> DataLakeAccessor::generateParameterQueries()
{
	std::list<BsonViewOrValue> comparisons;
	for (ot::QueryDescription& queryDescription : m_queryDescriptionsParameters)
	{
		std::optional<BsonViewOrValue> comparison = generateComparisonConsideringUnits(queryDescription);
		if (comparison.has_value())
		{
			comparisons.push_back(comparison.value());
			const std::string debugQuery = bsoncxx::to_json(comparison.value());
		}
	}

	return comparisons;
}

void DataLakeAccessor::generateQuantityQueries(BsonViewOrValue& _resultCollectionQuery, BsonViewOrValue& _transformedCollectionQuery)
{
	AdvancedQueryBuilder builder;
	std::list<BsonViewOrValue> resultCollectionQueries, transformedCollectionQueries;

	for (ot::QueryDescription& queryDescription : m_queryDescriptionsQuantities)
	{
		std::list<BsonViewOrValue > queryElements;
		//First we need to define a query for the quantity ID
		const std::string fieldValue = queryDescription.getQueryTargetDescription().getMongoDBFieldName();
		ot::TupleInstance instance;
		instance.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName() });
		ot::ValueComparisonDescription quantitySelection(MetadataQuantity::getFieldName(), "=", fieldValue, instance);
		queryElements.push_back(builder.createComparison(quantitySelection));

		//Now we create the value comparison, if set
		const ot::QueryTargetDescription& targetDescription = queryDescription.getQueryTargetDescription();
		auto storedTupleDescription = targetDescription.getTupleInstance();
		const ot::ValueComparisonDescription& comparisonDescription = queryDescription.getValueComparisonDescription();
		
		auto queryTupleDescription = comparisonDescription.getTupleInstance();
		bool targetTransformedCollection = transformationNecessary(queryTupleDescription, storedTupleDescription);
		// If the query has no comparator set, we simply target all documents with the named quantity. 
		if (!comparisonDescription.getComparator().empty())
		{
			auto valueQueryDescription = createQuantityValueQueryDescription(queryDescription);
			if (targetTransformedCollection)
			{
				// If transform necessary :
				// 1) Storage tuple format to SIBase -> Store all data points as SIBase in new format in transformed db
				// 2) target tuple format to SIBase -> Adjust query value accordingly
				// 3) Determine unit transformation from target tuple format to SIBase -> Take inverse and use it for the resulting data 
				if (!alreadyStoredTransformation(queryDescription))
				{
					storeTransformation(queryDescription);
				}
				std::optional<BsonViewOrValue> valueQuery = generateQueryFromSIBaseToTarget(valueQueryDescription);
				if (valueQuery.has_value())
				{
					// Value comparison was viable for creating a query.
					queryElements.push_back(valueQuery.value());
				}
				else
				{
					assert(false);
				}
			}
			else
			{
				std::optional<BsonViewOrValue> valueQuery = generateComparisonConsideringUnits(valueQueryDescription);

				if (valueQuery.has_value())
				{
					// Value comparison was viable for creating a query.
					queryElements.push_back(valueQuery.value());
				}
				else
				{
					assert(false);
				}
			}

		}
		// Depending on wether or not a comparator was set, we have two components that need to be and connected or not.
		BsonViewOrValue finalQuery;
		if (queryElements.size() > 1)
		{
			finalQuery = builder.connectWithAND(std::move(queryElements));
			const std::string debugQuery = bsoncxx::to_json(finalQuery.view());
			// Without transformation: Simple transformation from target format to storage format
			
		}
		else if(queryElements.size() == 1)
		{
			finalQuery = queryElements.front();
		}
		else
		{
			assert(false);
		}

		if (targetTransformedCollection)
		{
			transformedCollectionQueries.push_back(std::move(finalQuery));
		}
		else
		{
			resultCollectionQueries.push_back(std::move(finalQuery));
		}

	}


	//Finally, if we query multiple quantities, they need to be or connected.
	if (resultCollectionQueries.size() == 1)
	{
		_resultCollectionQuery = std::move(resultCollectionQueries.front());
	}
	else if(resultCollectionQueries.size() > 1)
	{
		_resultCollectionQuery = builder.connectWithOR(std::move(resultCollectionQueries));
	}
	

	if (transformedCollectionQueries.size() == 1)
	{
		_transformedCollectionQuery = std::move(transformedCollectionQueries.front());
	}
	else if (transformedCollectionQueries.size() > 1)
	{
		_transformedCollectionQuery = builder.connectWithOR(std::move(transformedCollectionQueries));
	}
}

ot::QueryDescription DataLakeAccessor::createQuantityValueQueryDescription(ot::QueryDescription _queryDescription)
{
	ot::ValueComparisonDescription valueQueryComparison = _queryDescription.getValueComparisonDescription();
	valueQueryComparison.setName(QuantityContainer::getFieldName());
	_queryDescription.setComparisonDescription(valueQueryComparison);
	return _queryDescription;
}

std::optional<BsonViewOrValue> DataLakeAccessor::generateComparisonConsideringUnits(ot::QueryDescription& _queryDescription)
{
	ot::ValueComparisonDescription selectedQuery = _queryDescription.getValueComparisonDescription();
	if (!selectedQuery.getComparator().empty() && !selectedQuery.getValue().empty() && !selectedQuery.getName().empty())
	{
		//First we get the components for the query
		AdvancedQueryBuilder queryBuilder;
		auto valueComparisonCleaned = _queryDescription.getValueComparisonDescription();
		std::list<std::string> mongoDBComparators = queryBuilder.extractMongoComparators(valueComparisonCleaned);
		std::vector<ot::Variable> values = queryBuilder.getListOfValuesFromString(valueComparisonCleaned);
		const std::string fieldName = queryBuilder.createFieldName(valueComparisonCleaned);

		//We may need to adjust the values if the unit that was selected in the query differs from the unit the data are stored with
		auto unitsTarget = selectedQuery.getTupleInstance().getTupleUnits();
		auto unitsCurrent = _queryDescription.getQueryTargetDescription().getTupleInstance().getTupleUnits();
		
		assert(unitsTarget.size() == unitsCurrent.size());
		ValueProcessingChainBuilder chainBuilder;
		bool queryForEntireTuple = _queryDescription.getValueComparisonDescription().getTupleTarget() == _queryDescription.getValueComparisonDescription().getTupleInstance().getTupleTypeName();
		bool queryTuple = 	!_queryDescription.getValueComparisonDescription().getTupleInstance().isSingle();
		if (!queryTuple || (queryTuple  && queryForEntireTuple))
		{
			if (values.size() != unitsCurrent.size())
			{
				//If not tuple, then units size = 1, thus unit size cannot be larger then values
				assert(values.size() < unitsCurrent.size());
				throw std::exception("Not values provided for a tuple query.");
			}

			for (size_t i = 0; i < values.size(); i++)
			{
				ot::ValueProcessing processingChain = chainBuilder.build(unitsCurrent[i], unitsTarget[i]);
				ot::ValueProcessing inverseProcessingChain;
				if (!processingChain.isEmpty())
				{
					values[i] = processingChain.execute(values[i]);
					inverseProcessingChain = processingChain.inverse();
				}
				else
				{
					inverseProcessingChain = processingChain;
				}

				if (values.size() > 1)
				{
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(inverseProcessingChain));
				}
				else
				{
					m_inverseParameterTransformationsByFieldKey.insert({ fieldName, std::move(inverseProcessingChain) });
				}
			}
		}
		else
		{
			// Special handling for query of a single tuple entry.
			assert(values.size() == 1);

			ot::TupleDescription* tupleDescription = TupleFactory::create(_queryDescription.getQueryTargetDescription().getTupleInstance().getTupleTypeName());
			ot::TupleDescriptionComplex* complexTupleDescription = dynamic_cast<ot::TupleDescriptionComplex*>(tupleDescription);
			assert(complexTupleDescription != nullptr);
			const std::string targetElement = _queryDescription.getValueComparisonDescription().getTupleTarget();

			auto tupleElementNames = complexTupleDescription->getTupleElementNames(_queryDescription.getQueryTargetDescription().getTupleInstance().getTupleFormatName());
			auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();
			
			for (size_t i = 0; i < unitsCurrent.size(); i++)
			{
				const ot::ValueProcessing processingChain = chainBuilder.build(unitsCurrent[i], unitsTarget[i]);
				if (!processingChain.isEmpty())
				{
					if (i == pos)
					{
						values[0] = processingChain.execute(values[0]);
					}
					const ot::ValueProcessing inverseProcessingChain = processingChain.inverse();
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(inverseProcessingChain));
				}
				else
				{
					// Keep the processing chain as inverse, so that later chains can be associated by index to the returned value.
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(processingChain));
				}

			}

		}

		//Finally we create the query
		BsonViewOrValue query = queryBuilder.createComparison(fieldName, mongoDBComparators, { values.begin(),values.end()});
		return query;
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<BsonViewOrValue> DataLakeAccessor::generateQueryFromSIBaseToTarget(ot::QueryDescription& _queryDescription)
{

	ot::ValueComparisonDescription selectedQuery = _queryDescription.getValueComparisonDescription();
	if (!selectedQuery.getComparator().empty() && !selectedQuery.getValue().empty() && !selectedQuery.getName().empty())
	{
		//First we get the components for the query
		AdvancedQueryBuilder queryBuilder;
		auto valueComparisonCleaned = _queryDescription.getValueComparisonDescription();
		std::list<std::string> mongoDBComparators = queryBuilder.extractMongoComparators(valueComparisonCleaned);
		std::vector<ot::Variable> values = queryBuilder.getListOfValuesFromString(valueComparisonCleaned);
		const std::string fieldName = queryBuilder.createFieldName(valueComparisonCleaned);

		//We may need to adjust the values if the unit that was selected in the query differs from the unit the data are stored with
		auto unitsTarget = selectedQuery.getTupleInstance().getTupleUnits();
		
		ValueProcessingChainBuilder chainBuilder;
		bool queryForEntireTuple = _queryDescription.getValueComparisonDescription().getTupleTarget() == _queryDescription.getValueComparisonDescription().getTupleInstance().getTupleTypeName();
		bool queryTuple = _queryDescription.getValueComparisonDescription().getTupleInstance().isSingle();
		if (!queryTuple || (queryTuple && queryForEntireTuple))
		{
			if (values.size() != unitsTarget.size())
			{
				//If not tuple, then units size = 1, thus unit size cannot be larger then values
				assert(values.size() < unitsTarget.size());
				throw std::exception("Not values provided for a tuple query.");
			}

			for (size_t i = 0; i < values.size(); i++)
			{
				ot::ValueProcessing processingChain = chainBuilder.buildToSIChain(unitsTarget[i]);
				if (!processingChain.isEmpty())
				{
					values[i] = processingChain.execute(values[i]);
					ot::ValueProcessing inverseProcessingChain = processingChain.inverse();
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(inverseProcessingChain));
				}
				else
				{
					// Keep the processing chain as inverse, so that later chains can be associated by index to the returned value.
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(processingChain));
				}
			}
		}
		else
		{
			// Special handling for query of a single tuple entry.
			assert(values.size() == 1);

			ot::TupleDescription* tupleDescription = TupleFactory::create(_queryDescription.getQueryTargetDescription().getTupleInstance().getTupleTypeName());
			ot::TupleDescriptionComplex* complexTupleDescription = dynamic_cast<ot::TupleDescriptionComplex*>(tupleDescription);
			assert(complexTupleDescription != nullptr);
			const std::string targetElement = _queryDescription.getValueComparisonDescription().getTupleTarget();

			auto tupleElementNames = complexTupleDescription->getTupleElementNames(_queryDescription.getQueryTargetDescription().getTupleInstance().getTupleFormatName());
			auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();

			for (size_t i = 0; i < unitsTarget.size(); i++)
			{
				ot::ValueProcessing processingChain = chainBuilder.buildToSIChain(unitsTarget[i]);
				if (!processingChain.isEmpty())
				{
					if (i == pos)
					{
						values[0] = processingChain.execute(values[0]);
					}
					ot::ValueProcessing inverseProcessingChain = processingChain.inverse();
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(inverseProcessingChain));
				}
				else
				{
					// Keep the processing chain as inverse, so that later chains can be associated by index to the returned value.
					m_inverseQuantityTransformationsByFieldKey[fieldName].push_back(std::move(processingChain)); 
				}
			}
		}

		//Finally we create the query
		BsonViewOrValue query = queryBuilder.createComparison(fieldName, mongoDBComparators, { values.begin(),values.end() });
		return query;
	}
	else
	{
		return std::nullopt;
	}
}

bool DataLakeAccessor::compare(const ot::ValueComparisonDescription& _comparisonDef, const ot::JsonValue& _value)
{
	std::string type;
	if (_value.IsInt())
	{
		type = ot::TypeNames::getInt32TypeName();
	}
	else if (_value.IsInt64())
	{
		type = ot::TypeNames::getInt64TypeName();
	}
	else if (_value.IsFloat())
	{
		type = ot::TypeNames::getFloatTypeName();
	}
	else if (_value.IsDouble())
	{
		type = ot::TypeNames::getDoubleTypeName();
	}
	else if (_value.IsBool())
	{
		type = ot::TypeNames::getBoolTypeName();
	}
	else
	{
		throw std::exception("Not supported data type for comparison");
	}

	ot::ExplicitStringValueConverter converter;
	ot::Variable givenValue = converter.setValueFromString(_comparisonDef.getValue(), type);

	ot::JSONToVariableConverter converterJ;
	ot::Variable isValue = converterJ(_value);

	if (_comparisonDef.getComparator() == "=")
	{
		return givenValue == isValue;
	}
	else if (_comparisonDef.getComparator() == "<")
	{
		return givenValue < isValue;
	}
	else if (_comparisonDef.getComparator() == "<=")
	{
		return (givenValue < isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == ">")
	{
		return (givenValue > isValue);
	}
	else if (_comparisonDef.getComparator() == ">=")
	{
		return (givenValue > isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == "!=")
	{
		return !(givenValue == isValue);
	}
	else
	{
		throw std::exception(("Not supported operator for comparison: " + _comparisonDef.getComparator()).c_str());
	}
	return false;
}

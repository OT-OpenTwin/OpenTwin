#include "DataLakeAccessor.h"
#include "Application.h"
#include "QueryDescriptionBuilder.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTDataStorage/DataLakeAPI.h"
#include "OTResultDataAccess/ResultCollection/IndexHandler.h"
#include "OTResultDataAccess/QuantityContainer.h"
#include "OTCore/Tuple/TupleFactory.h"
#include <tuple>
#include "ValueProcessingChainBuilder.h"
void DataLakeAccessor::accessPartition(const std::string& _collectionName)
{
	m_collectionName = _collectionName;
	m_queryDescriptionsParameters.clear();
	m_queryDescriptionsQuantities.clear();
	m_queryDescriptionsSeries.clear();
}

void DataLakeAccessor::setValueDescriptionsSeries(const std::list<ot::QueryDescription>& _queryDescriptions)
{
	m_queryDescriptionsSeries = _queryDescriptions;
}

void DataLakeAccessor::setValueDescriptionsQuantities(const std::list<ot::QueryDescription>& _queryDescriptions)
{
	m_queryDescriptionsQuantities = _queryDescriptions;
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
	IndexHandler indexHandler(m_collectionName);
	indexHandler.createDefaultIndexes();

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

bool DataLakeAccessor::transformationNecessary(const TupleInstance& _storedFormat, const TupleInstance& _queryFormat)
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
	TupleInstance instance;
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
	const std::string fieldValue = _queryDescription.getQueryTargetDescription().getMongoDBFieldName();
	TupleInstance instance;
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

		const ot::QueryTargetDescription& storedDataDescr = _queryDescription.getQueryTargetDescription();
		const ot::ValueComparisonDescription& queryDescription =	_queryDescription.getValueComparisonDescription();

		const std::string storedFormatName = storedDataDescr.getTupleInstance().getTupleFormatName();
		const std::string storedTupleType = storedDataDescr.getTupleInstance().getTupleTypeName();
		const std::string queryFormatName = queryDescription.getTupleInstance().getTupleFormatName();
		TupleDescription* tupleDescription = TupleFactory::create(storedTupleType);
		TupleDescriptionComplex* complexDescr = dynamic_cast<TupleDescriptionComplex*>(tupleDescription);
		std::function<std::pair<double, double>(double, double)> transform;

		if (queryFormatName == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian))
		{
			assert(storedTupleType == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar));
			transform =
				[](double _first, double _second) -> std::pair<double, double>
				{
					std::complex<double> transformed = ot::ComplexNumberConversion::polarToCartesian(_first, _second);
					return { transformed.real(), transformed.imag() };
				};
		}
		else
		{
			assert(storedTupleType == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian));
			transform =
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
			std::pair<double,double> transFormedValues = transform(first, second);
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
		TupleInstance instance;
		instance.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName() });
		ot::ValueComparisonDescription quantitySelection(MetadataQuantity::getFieldName(), "=", fieldValue, instance);
		queryElements.push_back(builder.createComparison(quantitySelection));

		//Now we create the value comparison
		ot::QueryDescription valueQueryDescription = queryDescription;
		ot::ValueComparisonDescription valueQueryComparison = valueQueryDescription.getValueComparisonDescription();
		valueQueryComparison.setName(QuantityContainer::getFieldName());
		valueQueryDescription.setComparisonDescription(valueQueryComparison);
		std::optional<BsonViewOrValue> valueQuery =	generateComparisonConsideringUnits(valueQueryDescription);
		
		if (valueQuery.has_value())
		{
			// Value comparison was viable for creating a query.
			queryElements.push_back(valueQuery.value());
			BsonViewOrValue combinedQuery = builder.connectWithAND(std::move(queryElements));
			const std::string debugQuery = bsoncxx::to_json(combinedQuery.view());

			// Lastly we check if the 
			const ot::QueryTargetDescription& targetDescription = queryDescription.getQueryTargetDescription();
			auto storedTupleDescription = targetDescription.getTupleInstance();
			const ot::ValueComparisonDescription& comparisonDescription = queryDescription.getValueComparisonDescription();
			auto queryTupleDescription = comparisonDescription.getTupleInstance();
		
			if (transformationNecessary(queryTupleDescription, storedTupleDescription))
			{
				if (!alreadyStoredTransformation(queryDescription))
				{
					storeTransformation(queryDescription);
				}
				transformedCollectionQueries.push_back(combinedQuery);
			}
			else
			{
				resultCollectionQueries.push_back(combinedQuery);
			}
		}
	}

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
		assert(values.size() == unitsTarget.size() == unitsCurrent.size());
		ValueProcessingChainBuilder chainBuilder;
		for (size_t i = 0; i < values.size(); i++)
		{
			ValueProcessing processingChain = chainBuilder.build(unitsCurrent[i], unitsTarget[i]);
			if (processingChain.executionNecessary())
			{
				values[i] = processingChain.executeSequence(values[i]);
				ValueProcessing inverseProcessingChain = processingChain.createInverse();
				m_inverseTransformationsByFieldKey.insert({ fieldName, std::move(inverseProcessingChain) });
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

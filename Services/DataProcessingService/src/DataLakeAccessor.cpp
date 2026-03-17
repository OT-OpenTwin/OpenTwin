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

void DataLakeAccessor::setValueDescriptionsParameters(const std::list<ot::QueryDescription>& _queryDescriptions)
{
	m_queryDescriptionsParameters = _queryDescriptions;
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

	if (!resultCollectionQuery.view().empty())
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName);
		auto collectionHandleCreated = std::chrono::high_resolution_clock::now();
		DataStorageAPI::DataStorageResponse dbResponse = resultCollectionAccess.searchInDataLakePartition(resultCollectionQuery, _options);
		auto queryExecuted = std::chrono::high_resolution_clock::now();
		if (dbResponse.getSuccess())
		{
			const std::string queryResponse = dbResponse.getResult();
			ot::JsonDocument doc;
			doc.fromJson(queryResponse);
			auto allMongoDocuments = ot::json::getArray(doc, "Documents");
		}
	}



	//const std::string queryDuration = TimeFormatter::formatDuration(startTime, endTime);

	return ot::JsonDocument();
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
	}
	
	if (!resultCollectionQuantityQuery.view().empty())
	{
		std::list<BsonViewOrValue> allQueries = allQueriesBase;
		allQueries.push_back(resultCollectionQuantityQuery);
		_resultCollectionQuery = builder.connectWithAND(std::move(allQueries));
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
	DataStorageAPI::DataLakeAPI transformedCollectionAccess(m_collectionName,".transformed");

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
		
		DataStorageAPI::DataLakeAPI transformationCollectionAccess(m_collectionName,".transformed");
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

BsonViewOrValue DataLakeAccessor::generateSeriesQuery()
{
	AdvancedQueryBuilder builder;

	std::list<BsonViewOrValue> comparisons;
	for (ot::QueryDescription& queryDescription : m_queryDescriptionsSeries)
	{
		const ot::ValueComparisonDescription& selectedQuery = queryDescription.getValueComparisonDescription();
		BsonViewOrValue comparison = builder.createComparison(selectedQuery);
		comparisons.push_back(comparison);
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
	AdvancedQueryBuilder builder;

	std::list<BsonViewOrValue> comparisons;
	for (ot::QueryDescription& queryDescription : m_queryDescriptionsParameters)
	{
		ot::ValueComparisonDescription selectedQuery = queryDescription.getValueComparisonDescription();
		const std::string fieldName = queryDescription.getQueryTargetDescription().getMongoDBFieldName();
		selectedQuery.setName(fieldName);
		BsonViewOrValue comparison = builder.createComparison(selectedQuery);
		comparisons.push_back(comparison);
		const std::string debugQuery = bsoncxx::to_json(comparison);
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

		ot::ValueComparisonDescription valueQuery = queryDescription.getValueComparisonDescription();
		valueQuery.setName(QuantityContainer::getFieldName());
		queryElements.push_back(builder.createComparison(valueQuery));

		BsonViewOrValue combinedQuery = builder.connectWithAND(std::move(queryElements));
		const std::string debugQuery = bsoncxx::to_json(combinedQuery.view());

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

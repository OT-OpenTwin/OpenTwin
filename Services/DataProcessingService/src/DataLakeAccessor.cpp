#include "DataLakeAccessor.h"
#include "Application.h"
#include "QueryDescriptionBuilder.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTDataStorage/DataLakeAPI.h"

void DataLakeAccessor::accessPartition(const std::string& _collectionName)
{
	m_collectionName = _collectionName;
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


	BsonViewOrValue resultCollectionQuery, transformedCollectionQuery;
	createQueries(resultCollectionQuery,transformedCollectionQuery);
	
	if (!resultCollectionQuery.view().empty())
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		DataStorageAPI::DataLakeAPI resultCollectionAccess(m_collectionName);
		auto collectionHandleCreated = std::chrono::high_resolution_clock::now();
		DataStorageAPI::DataStorageResponse dbResponse = resultCollectionAccess.searchInResultCollection(resultCollectionQuery, _options);
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
		_resultCollectionQuery = builder.connectWithAND(std::move(allQueries));
	}
	
	if (!resultCollectionQuantityQuery.view().empty())
	{
		std::list<BsonViewOrValue> allQueries = allQueriesBase;
		allQueries.push_back(transformedCollectionQuantityQuery);
		_transformedCollectionQuery = builder.connectWithAND(std::move(allQueries));
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

bool DataLakeAccessor::alreadyStoredTransformation(const TupleInstance& _storedFormat, const TupleInstance& _queryFormat)
{
	return false;
}

void DataLakeAccessor::storeTransformation(const TupleInstance& _storedFormat, const TupleInstance& _queryFormat)
{

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

	return seriesQuery;
}

std::list<BsonViewOrValue> DataLakeAccessor::generateParameterQueries()
{
	AdvancedQueryBuilder builder;

	std::list<BsonViewOrValue> comparisons;
	for (ot::QueryDescription& queryDescription : m_queryDescriptionsParameters)
	{
		const ot::ValueComparisonDescription& selectedQuery = queryDescription.getValueComparisonDescription();
		BsonViewOrValue comparison = builder.createComparison(selectedQuery);
		comparisons.push_back(comparison);
	}

	return comparisons;
}

void DataLakeAccessor::generateQuantityQueries(BsonViewOrValue& _resultCollectionQuery, BsonViewOrValue& _transformedCollectionQuery)
{
	AdvancedQueryBuilder builder;
	std::list<BsonViewOrValue> resultCollectionQueries, transformedCollectionQueries;

	for (ot::QueryDescription& queryDescription : m_queryDescriptionsQuantities)
	{
		const ot::ValueComparisonDescription& selectedQuery = queryDescription.getValueComparisonDescription();
		BsonViewOrValue comparison = builder.createComparison(selectedQuery);

		const ot::QueryTargetDescription& targetDescription = queryDescription.getQueryTargetDescription();
		auto storedTupleDescription = targetDescription.getTupleInstance();
		const ot::ValueComparisonDescription& comparisonDescription = queryDescription.getValueComparisonDescription();
		auto queryTupleDescription = comparisonDescription.getTupleInstance();
		if (transformationNecessary(queryTupleDescription, storedTupleDescription))
		{
			if (!alreadyStoredTransformation(queryTupleDescription, storedTupleDescription))
			{
				storeTransformation(queryTupleDescription, storedTupleDescription);
			}
			transformedCollectionQueries.push_back(comparison);
		}
		else
		{
			resultCollectionQueries.push_back(comparison);
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

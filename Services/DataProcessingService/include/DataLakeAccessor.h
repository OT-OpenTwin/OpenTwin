#pragma once
#include "OTDataStorage/Helper/QueryBuilder.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/QueryDescription/QueryDescription.h"
#include "mongocxx/options/find.hpp"
#include <map>
#include "OTCore/ValueProcessing/ValueProcessing.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"

class DataLakeAccessor
{
public:
	void accessPartition(const std::string& _collectionName);
	void createQueryDescriptionsSeries(const std::list<ot::ValueComparisonDescription>& _valueComparisons, const std::string& _seriesLabel);
	void createQueryDescriptionsParameter(const std::list<ot::ValueComparisonDescription>& _valueComparisons);
	void createQueryDescriptionQuantity(const ot::ValueComparisonDescription& _valueComparisons);
	
	ot::JsonDocument executeQuery(mongocxx::options::find _options);
	ot::DataLakeAccessCfg createConfig();

	ot::JsonDocument executeQuery(ot::DataLakeAccessCfg& _config, mongocxx::options::find _options);
private:
	std::list<ot::QueryDescription> m_queryDescriptionsSeries;
	std::list<ot::QueryDescription> m_queryDescriptionsQuantities;
	std::list<ot::QueryDescription> m_queryDescriptionsParameters;
	std::map<std::string, ot::ValueProcessing> m_inverseParameterTransformationsByFieldKey;
	std::map<std::string, std::list<ot::ValueProcessing>> m_inverseQuantityTransformationsByFieldKey;

	std::string m_collectionName;
	const std::string m_transformedCollectionEnding = ".transformed";
	const std::string m_resultCollectionEnding = ".results";
	const std::string m_resultDataField = "Data";
	
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;

	bool compare(const ot::ValueComparisonDescription& _comparisionDef, const ot::JsonValue& _value);

	void addValueDescriptionsParameters(const std::list<ot::QueryDescription>& _queryDescriptions);
	
	void createQueries(BsonViewOrValue& _resultCollectionQueries, BsonViewOrValue& _transformedCollectionQueries);
	
	//! @brief  If there is a mismatch between the query tuple format and the stored tuple format, it becomes necessary to transform the tuple.
	bool transformationNecessary(const ot::TupleInstance& _storedFormat, const ot::TupleInstance& _queryFormat);
	
	//! @brief Transformed tuple are stored in a "<collectionID>.transformed" collection. 
	//! A simple check is performed that counts the number of documents in the result collection which hold the target quantity. 
	//! Since currently no documents are deleted, the data is completely transformed if the number of documents matches the number of documents of the quantity in the transformed collection.
	bool alreadyStoredTransformation(const ot::QueryDescription& _queryDescription);

	//! @brief Store transformed tuple in SI base unit in "<collectionID>.transformed" collection
	void storeTransformation(const ot::QueryDescription& _queryDescription);

	ot::JsonDocument createClearTextResult(const ot::JsonDocument& _databaseResults);
	ot::JsonDocument createClearTextResult(ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults);
	std::map<std::string,std::string> createFieldKeyLabelLookup(const std::list<ot::QueryDescription>& _queryDescription);

	//! @brief All provided series are valid options and each document has only one series it belongs to. Thus, the series query is assembled as an or connected list of series ID options
	//! @return Empty if not series was provided. Should be prevented at a higher level, since queries without series specification will target every datapoint regardless of the modelstate.
	BsonViewOrValue generateSeriesQuery();
	
	//! @brief All parameter filter are generated separately. They are intended to be added to an $and chain.
	std::list<BsonViewOrValue> generateParameterQueries();

	//! @brief All provided quantities are valid options and each document holds only one quantity. Thus, the quantity query is assembled as an or connected list of quantity ID options.
	//! Depending on the targeted and stored tuple format, data points may be transformed and stored in another collection.
	void generateQuantityQueries(BsonViewOrValue& _resultCollectionQueries, BsonViewOrValue& _transformedCollectionQueries);

	ot::QueryDescription createQuantityValueQueryDescription(ot::QueryDescription _queryDescription);

	std::optional<BsonViewOrValue> generateComparisonConsideringUnits(ot::QueryDescription& _queryDescription);

	std::optional<BsonViewOrValue> generateQueryFromSIBaseToTarget(ot::QueryDescription& _queryDescription);

};

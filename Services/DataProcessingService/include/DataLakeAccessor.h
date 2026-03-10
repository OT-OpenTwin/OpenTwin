#pragma once
#include "OTDataStorage/Helper/QueryBuilder.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/QueryDescription/QueryDescription.h"
#include "mongocxx/options/find.hpp"

class DataLakeAccessor
{
public:
	void accessPartition(const std::string& _collectionName);
	void setValueDescriptionsSeries(const std::list<ot::QueryDescription>& _queryDescriptions);
	void setValueDescriptionsQuantities(const std::list<ot::QueryDescription>& _queryDescriptions);
	void setValueDescriptionsParameters(const std::list<ot::QueryDescription>& _queryDescriptions);

	ot::JsonDocument executeQuery(mongocxx::options::find _options);
private:
	std::list<ot::QueryDescription> m_queryDescriptionsSeries;
	std::list<ot::QueryDescription> m_queryDescriptionsQuantities;
	std::list<ot::QueryDescription> m_queryDescriptionsParameters;
	std::string m_collectionName;
	
	void createQueries(BsonViewOrValue& _resultCollectionQueries, BsonViewOrValue& _transformedCollectionQueries);
	bool transformationNecessary(const TupleInstance& _storedFormat, const TupleInstance& _queryFormat);
	bool alreadyStoredTransformation(const ot::QueryDescription& _queryDescription);
	void storeTransformation(const TupleInstance& _storedFormat, const TupleInstance& _queryFormat);

	//! @brief All provided series are valid options and each document has only one series it belongs to. Thus, the series query is assembled as an or connected list of series ID options
	//! @return Empty if not series was provided. Should be prevented at a higher level, since queries without series specification will target every datapoint regardless of the modelstate.
	BsonViewOrValue generateSeriesQuery();
	
	//! @brief All parameter filter are generated separately. They are intended to be added to an $and chain.
	std::list<BsonViewOrValue> generateParameterQueries();

	//! @brief All provided quantities are valid options and each document holds only one quantity. Thus, the quantity query is assembled as an or connected list of quantity ID options.
	//! Depending on the targeted and stored tuple format, data points may be transformed and stored in another collection.
	void generateQuantityQueries(BsonViewOrValue& _resultCollectionQueries, BsonViewOrValue& _transformedCollectionQueries);
	
};

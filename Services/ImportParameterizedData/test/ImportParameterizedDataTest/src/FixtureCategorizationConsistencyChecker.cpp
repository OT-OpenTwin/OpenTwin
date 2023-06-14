#include "FixtureCategorizationConsistencyChecker.h"
#include "EntityTableSelectedRanges.h"
#include "EntityParameterizedDataCategorization.h"

std::map<std::string, MetadataAssemblyData> FixtureCategorizationConsistencyChecker::GetFailureCollection_MissingParameter()
{
	std::map<std::string, MetadataAssemblyData> collection;
	collection.insert({ "a", MetadataAssemblyData() });
	collection["a"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b", MetadataAssemblyData() });
	collection["b"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	collection["a"].next = &collection["b"];
	collection.insert({ "c", MetadataAssemblyData() });
	collection["c"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	collection["b"].next = &collection["c"];

	collection.insert({ "a2", MetadataAssemblyData() });
	collection["a2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b2", MetadataAssemblyData() });
	collection["b2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	//Missing link 
	collection.insert({ "c2", MetadataAssemblyData() });
	collection["c2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	collection["b2"].next = &collection["c2"];

	return collection;
}

std::map<std::string, MetadataAssemblyData> FixtureCategorizationConsistencyChecker::GetFailureCollection_MissingQuantity()
{
	std::map<std::string, MetadataAssemblyData> collection;
	collection.insert({ "a", MetadataAssemblyData() });
	collection["a"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b", MetadataAssemblyData() });
	collection["b"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	collection["a"].next = &collection["b"];
	collection.insert({ "c", MetadataAssemblyData() });
	collection["c"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	collection["b"].next = &collection["c"];

	collection.insert({ "a2", MetadataAssemblyData() });
	collection["a2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b2", MetadataAssemblyData() });
	collection["b2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	collection["a2"].next = &collection["b2"];
	collection.insert({ "c2", MetadataAssemblyData() });
	collection["c2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	//Missing link 

	return collection;
}

std::map<std::string, MetadataAssemblyData> FixtureCategorizationConsistencyChecker::GetFailureCollection_QuantityAndParameterDontShareTable()
{
	std::map<std::string, MetadataAssemblyData> collection = GetCorrectCollection_AllLinksCorrect();
	
	std::shared_ptr<EntityTableSelectedRanges> range1(new EntityTableSelectedRanges(-1, nullptr, nullptr, nullptr, nullptr, ""));
	range1->SetTableProperties("Table1", -1, -1, "");
	std::shared_ptr<EntityTableSelectedRanges> range2(new EntityTableSelectedRanges(-1, nullptr, nullptr, nullptr, nullptr, ""));
	range2->SetTableProperties("Table2", -1, -1, "");

	collection["b"].allSelectionRanges = { range1, range1 };
	collection["c"].allSelectionRanges = { range1 , range1};
	
	collection["b2"].allSelectionRanges = { range1, range1 };
	collection["c2"].allSelectionRanges = { range2, range1 };

	return collection;
}

std::map<std::string, MetadataAssemblyData> FixtureCategorizationConsistencyChecker::GetCorrectCollection_AllLinksCorrect()
{
	std::map<std::string, MetadataAssemblyData> collection;
	collection.insert({ "a", MetadataAssemblyData() });
	collection["a"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b", MetadataAssemblyData() });
	collection["b"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	collection["a"].next = &collection["b"];
	collection.insert({ "c", MetadataAssemblyData() });
	collection["c"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	collection["b"].next = &collection["c"];

	collection.insert({ "a2", MetadataAssemblyData() });
	collection["a2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
	collection.insert({ "b2", MetadataAssemblyData() });
	collection["b2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
	collection["a2"].next = &collection["b2"];
	collection.insert({ "c2", MetadataAssemblyData() });
	collection["c2"].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
	collection["b2"].next = &collection["c2"];

	return collection;
}

std::map<std::string, MetadataAssemblyData> FixtureCategorizationConsistencyChecker::GetCorrectCollection_AllTableNamesCorrect()
{
	std::map<std::string, MetadataAssemblyData> collection = GetCorrectCollection_AllLinksCorrect();
	std::shared_ptr<EntityTableSelectedRanges> range1(new EntityTableSelectedRanges(-1, nullptr, nullptr, nullptr, nullptr, ""));
	range1->SetTableProperties("Table1", -1, -1, "");
	std::shared_ptr<EntityTableSelectedRanges> range2(new EntityTableSelectedRanges(-1, nullptr, nullptr, nullptr, nullptr, ""));
	range2->SetTableProperties("Table2", -1, -1, "");

	collection["b"].allSelectionRanges = { range1, range1 };
	collection["c"].allSelectionRanges = { range1, range1 };

	collection["b2"].allSelectionRanges = { range2, range2 };
	collection["c2"].allSelectionRanges = { range2, range2 };

	return collection;
}


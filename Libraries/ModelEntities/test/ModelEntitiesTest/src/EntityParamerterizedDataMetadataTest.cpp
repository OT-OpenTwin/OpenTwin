#include "gtest/gtest.h"
#include "EntityMeasurementMetadata.h"
#include <vector>
#include <stdint.h>

TEST(EntityMeasurementMetadata, TestFindDuplicateInStringList)
{
	EntityMeasurementMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.InsertInField(fieldName, values);
	std::list<std::string> dupplicateFieldNameValues{ "otherValue1", "otherValue2", "otherValue3"};
	EXPECT_ANY_THROW(metaData.InsertInField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityMeasurementMetadata, TestFindDuplicateInInt32List)
{
	EntityMeasurementMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.InsertInField(fieldName, values);
	std::list<int32_t> dupplicateFieldNameValues{ 1,2,3,4 };
	
	EXPECT_ANY_THROW(metaData.InsertInField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityMeasurementMetadata, TestFindDuplicateInInt64List)
{
	EntityMeasurementMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.InsertInField(fieldName, values);
	std::list<int64_t> dupplicateFieldNameValues{ 1,2,3,4 };
	EXPECT_ANY_THROW(metaData.InsertInField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityMeasurementMetadata, TestFindDuplicateInDoubleList)
{
	EntityMeasurementMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.InsertInField(fieldName, values);
	std::list<double> dupplicateFieldNameValues{ 1.,2.,3.,4. };
	EXPECT_ANY_THROW(metaData.InsertInField(fieldName, dupplicateFieldNameValues));
}

//Doesn't work
//TEST(EntityMeasurementMetadata, TestFindFieldnameAlreadyUsedByEntityBase)
//{
//	std::vector<std::string> values{ "value1", "value2" };
//	std::string fieldName = "EntityID";
//	auto uidGenerator = new DataStorageAPI::UniqueUIDGenerator(1, 1);
//	EntityMeasurementMetadata metaData(uidGenerator->getUID(), nullptr, nullptr, nullptr, nullptr, "");
//	metaData.InsertInField(fieldName, values);
//	EntityBase::setUidGenerator(uidGenerator);
//	auto clName = metaData.getClassName();
//	EXPECT_ANY_THROW(metaData.StoreToDataBase());
//}
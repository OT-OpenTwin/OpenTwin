#include "gtest/gtest.h"
#include "EntityParameterizedDataMetadata.h"
#include <vector>
#include <stdint.h>

TEST(EntityParameterizedDataMetadata, TestFindDuplicateInStringList)
{
	EntityParameterizedDataMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.AddDocumentField(fieldName, values);
	std::list<std::string> dupplicateFieldNameValues{ "otherValue1", "otherValue2", "otherValue3"};
	EXPECT_ANY_THROW(metaData.AddDocumentField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityParameterizedDataMetadata, TestFindDuplicateInInt32List)
{
	EntityParameterizedDataMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.AddDocumentField(fieldName, values);
	std::list<int32_t> dupplicateFieldNameValues{ 1,2,3,4 };
	
	EXPECT_ANY_THROW(metaData.AddDocumentField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityParameterizedDataMetadata, TestFindDuplicateInInt64List)
{
	EntityParameterizedDataMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.AddDocumentField(fieldName, values);
	std::list<int64_t> dupplicateFieldNameValues{ 1,2,3,4 };
	EXPECT_ANY_THROW(metaData.AddDocumentField(fieldName, dupplicateFieldNameValues));
}
TEST(EntityParameterizedDataMetadata, TestFindDuplicateInDoubleList)
{
	EntityParameterizedDataMetadata metaData(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::list<std::string> values{ "value1", "value2" };
	std::string fieldName = "Fieldname";
	metaData.AddDocumentField(fieldName, values);
	std::list<double> dupplicateFieldNameValues{ 1.,2.,3.,4. };
	EXPECT_ANY_THROW(metaData.AddDocumentField(fieldName, dupplicateFieldNameValues));
}

//Doesn't work
//TEST(EntityParameterizedDataMetadata, TestFindFieldnameAlreadyUsedByEntityBase)
//{
//	std::vector<std::string> values{ "value1", "value2" };
//	std::string fieldName = "EntityID";
//	auto uidGenerator = new DataStorageAPI::UniqueUIDGenerator(1, 1);
//	EntityParameterizedDataMetadata metaData(uidGenerator->getUID(), nullptr, nullptr, nullptr, nullptr, "");
//	metaData.AddDocumentField(fieldName, values);
//	EntityBase::setUidGenerator(uidGenerator);
//	auto clName = metaData.getClassName();
//	EXPECT_ANY_THROW(metaData.StoreToDataBase());
//}
// @otlicense

#include "FixtureEntityWithDynamicFields.h"


TEST_F(FixtureEntityWithDynamicFields, FindInsertedDocument)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	const std::string documentName = "FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_NO_THROW(entity.getDocument(documentName));
}

TEST_F(FixtureEntityWithDynamicFields, FindInsertedDocument_RootInName)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	const std::string documentName = "/FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_NO_THROW(entity.getDocument(documentName));
}

TEST_F(FixtureEntityWithDynamicFields, InsertedDocumentCount)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	const std::string documentName = "FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_EQ(entity.getDocumentsNames().size(), 2);
}

TEST_F(FixtureEntityWithDynamicFields, FindRootDocument_viaName)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	EXPECT_NO_THROW(entity.getDocument("/"));
}

TEST_F(FixtureEntityWithDynamicFields, FindRootDocument_asTopLevelDoc)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	EXPECT_NO_THROW(entity.getDocumentTopLevel());
}

TEST_F(FixtureEntityWithDynamicFields, OrderDocuments)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");
	
	const std::string firstDocumentName = "FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, firstDocumentName);
	const std::string secondDocumentName = "SecondDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, firstDocumentName + "/" + secondDocumentName);
	
	OrderDocumentsHierarchical(entity);
	const GenericDocument*  topLevelDoc = entity.getDocumentTopLevel();
	auto firstLevelDocuments =	topLevelDoc->getSubDocuments();
	ASSERT_EQ(firstLevelDocuments.size(), 1);
	auto secondLevelDocuments =	(*firstLevelDocuments.begin())->getSubDocuments();
	ASSERT_EQ(secondLevelDocuments.size(), 1);
	auto thirdLevelDocuments = (*secondLevelDocuments.begin())->getSubDocuments();
	ASSERT_EQ(thirdLevelDocuments.size(), 0);
}

TEST_F(FixtureEntityWithDynamicFields, OrderDocumentsWithGaps)
{
	EntityWithDynamicFields entity(0, nullptr, nullptr, nullptr, nullptr, "");

	const std::string documentName = "FirstDocument/SecondDocument/ThirdDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	OrderDocumentsHierarchical(entity);
	const GenericDocument* topLevelDoc = entity.getDocumentTopLevel();
	auto firstLevelDocuments = topLevelDoc->getSubDocuments();
	ASSERT_EQ(firstLevelDocuments.size(), 1);
	auto secondLevelDocuments = (*firstLevelDocuments.begin())->getSubDocuments();
	ASSERT_EQ(secondLevelDocuments.size(), 1);
	auto thirdLevelDocuments = (*secondLevelDocuments.begin())->getSubDocuments();
	ASSERT_EQ(thirdLevelDocuments.size(), 1);
	auto fourthLevelDocuments = (*thirdLevelDocuments.begin())->getSubDocuments();
	ASSERT_EQ(fourthLevelDocuments.size(), 0);


}
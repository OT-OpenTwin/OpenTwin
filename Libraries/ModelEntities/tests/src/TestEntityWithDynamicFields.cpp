// @otlicense
// File: TestEntityWithDynamicFields.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "FixtureEntityWithDynamicFields.h"


TEST_F(FixtureEntityWithDynamicFields, FindInsertedDocument)
{
	EntityWithDynamicFields entity;
	const std::string documentName = "FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_NO_THROW(entity.getDocument(documentName));
}

TEST_F(FixtureEntityWithDynamicFields, FindInsertedDocument_RootInName)
{
	EntityWithDynamicFields entity;
	const std::string documentName = "/FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_NO_THROW(entity.getDocument(documentName));
}

TEST_F(FixtureEntityWithDynamicFields, InsertedDocumentCount)
{
	EntityWithDynamicFields entity;
	const std::string documentName = "FirstDocument";
	entity.InsertInField("FirstField", { ot::Variable(1.3) }, documentName);
	EXPECT_EQ(entity.getDocumentsNames().size(), 2);
}

TEST_F(FixtureEntityWithDynamicFields, FindRootDocument_viaName)
{
	EntityWithDynamicFields entity;
	EXPECT_NO_THROW(entity.getDocument("/"));
}

TEST_F(FixtureEntityWithDynamicFields, FindRootDocument_asTopLevelDoc)
{
	EntityWithDynamicFields entity;
	EXPECT_NO_THROW(entity.getDocumentTopLevel());
}

TEST_F(FixtureEntityWithDynamicFields, OrderDocuments)
{
	EntityWithDynamicFields entity;
	
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
	EntityWithDynamicFields entity;

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
// @otlicense
// File: TestGenericDataStruct.cpp
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

// Google test header
#include <gtest/gtest.h>

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTCore/DataStruct/GenericDataStructVector.h"

TEST(GenericDataStructMatrix, FillingWithOneValue)
{
	ot::Variable expectedValue(5);
	ot::MatrixEntryPointer entry;
	entry.setColumn(3);
	entry.setRow(3);
	ot::GenericDataStructMatrix matrix(entry);
	{
		ot::Variable value(5);
		entry.setColumn(2);
		entry.setRow(2);
		matrix.setValue(entry, std::move(value));
	}
	const ot::Variable& actualValue = matrix.getValue(entry);

	EXPECT_EQ(actualValue, expectedValue);
}

TEST(GenericDataStructMatrix, Serialization)
{
	ot::MatrixEntryPointer entry;
	entry.setColumn(4);
	entry.setRow(4);
	ot::GenericDataStructMatrix expectedMatrix(entry);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.setColumn(j);
			entry.setRow(i);
			expectedMatrix.setValue(entry, ot::Variable(i * 4 + j));
		}
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedMatrix.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Matrix", val, doc.GetAllocator());
	
	auto serActualMatrix = ot::json::getObject(doc, "Matrix");
	
	entry.setColumn(4);
	entry.setRow(4);

	ot::GenericDataStructMatrix actualMatrix(entry);
	actualMatrix.setFromJsonObject(serActualMatrix);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.setColumn(j);
			entry.setRow(i);
			const ot::Variable actVal = actualMatrix.getValue(entry);
			const ot::Variable expectVal = expectedMatrix.getValue(entry);
			EXPECT_EQ(actVal, expectVal );
		}
	}
}

TEST(GenericDataStructVector, FillingWithOneValue)
{
	ot::Variable expectedValue(5);
	ot::GenericDataStructVector vector(1);
	{
		ot::Variable value(5);
		vector.setValue(0,std::move(value));
	}
	const ot::Variable& actualValue = vector.getValue(0);

	EXPECT_EQ(actualValue, expectedValue);
}

TEST(GenericDataStructVector, Serialization)
{
	ot::GenericDataStructVector expectedvector(4);
	for (int i = 0; i < 4; i++)
	{	
		expectedvector.setValue(i, ot::Variable(i * 4));
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedvector.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Vector", val, doc.GetAllocator());

	auto serActualVector = ot::json::getObject(doc, "Vector");
	ot::GenericDataStructVector actualVector(4);
	actualVector.setFromJsonObject(serActualVector);
	for (int i = 0; i < 4; i++)
	{
		EXPECT_EQ(actualVector.getValue(i), expectedvector.getValue(i));
	}
}

TEST(GenericDataStruct, TypeDetectionVector)
{
	ot::GenericDataStructVector expectedvector(4);
	for (int i = 0; i < 4; i++)
	{
		expectedvector.setValue(i, ot::Variable(i * 4));
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedvector.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Vector", val, doc.GetAllocator());

	auto serActualVector = ot::json::getObject(doc, "Vector");

	std::unique_ptr<ot::GenericDataStruct> dataStruct;
	EXPECT_NO_THROW(dataStruct.reset(ot::GenericDataStruct::fromJson(serActualVector)));
	EXPECT_EQ(dataStruct->getClassName(), ot::GenericDataStructVector::className());
}

TEST(GenericDataStruct, TypeDetectionMatrix)
{
	ot::MatrixEntryPointer entry;
	entry.setColumn(4);
	entry.setRow(4);
	ot::GenericDataStructMatrix expectedMatrix(entry);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.setColumn(j);
			entry.setRow(i);
			expectedMatrix.setValue(entry, ot::Variable(i * 4 + j));
		}
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedMatrix.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Matrix", val, doc.GetAllocator());

	auto serActualMatrix = ot::json::getObject(doc, "Matrix");

	std::unique_ptr<ot::GenericDataStruct> dataStruct;
	EXPECT_NO_THROW(dataStruct.reset(ot::GenericDataStruct::fromJson(serActualMatrix)));
	EXPECT_EQ(dataStruct->getClassName(), ot::GenericDataStructMatrix::className());
}
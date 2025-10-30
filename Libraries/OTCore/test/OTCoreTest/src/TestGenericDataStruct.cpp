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

#include <gtest/gtest.h>
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"

TEST(GenericDataStructMatrix, FillingWithOneValue)
{
	ot::Variable expectedValue(5);
	ot::MatrixEntryPointer entry;
	entry.m_column = 3;
	entry.m_row = 3;
	ot::GenericDataStructMatrix matrix(entry);
	{
		ot::Variable value(5);
		entry.m_column = 2;
		entry.m_row = 2;
		matrix.setValue(entry, std::move(value));
	}
	const ot::Variable& actualValue = matrix.getValue(entry);

	EXPECT_EQ(actualValue, expectedValue);
}

TEST(GenericDataStructMatrix, Serialization)
{
	ot::MatrixEntryPointer entry;
	entry.m_column = 4;
	entry.m_row = 4;
	ot::GenericDataStructMatrix expectedMatrix(entry);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.m_column = j;
			entry.m_row = i;
			expectedMatrix.setValue(entry, ot::Variable(i * 4 + j));
		}
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedMatrix.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Matrix", val, doc.GetAllocator());
	
	auto serActualMatrix = ot::json::getObject(doc, "Matrix");
	
	entry.m_column = 4;
	entry.m_row = 4;

	ot::GenericDataStructMatrix actualMatrix(entry);
	actualMatrix.setFromJsonObject(serActualMatrix);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.m_column = j;
			entry.m_row = i;
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

	ot::GenericDataStruct dataStruct;
	dataStruct.setFromJsonObject(serActualVector);

	EXPECT_EQ(dataStruct.getTypeIdentifyer(), ot::GenericDataStructVector::getClassName());

	ot::GenericDataStructVector actualVector;
	EXPECT_NO_THROW(actualVector.setFromJsonObject(serActualVector));
}

TEST(GenericDataStruct, TypeDetectionMatrix)
{
	ot::MatrixEntryPointer entry;
	entry.m_column = 4;
	entry.m_row = 4;
	ot::GenericDataStructMatrix expectedMatrix(entry);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			entry.m_column = j;
			entry.m_row = i;
			expectedMatrix.setValue(entry, ot::Variable(i * 4 + j));
		}
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedMatrix.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Matrix", val, doc.GetAllocator());

	auto serActualMatrix = ot::json::getObject(doc, "Matrix");

	ot::GenericDataStruct dataStruct;
	dataStruct.setFromJsonObject(serActualMatrix);

	EXPECT_EQ(dataStruct.getTypeIdentifyer(), ot::GenericDataStructMatrix::getClassName());

	ot::GenericDataStructMatrix actualMatrix;
	EXPECT_NO_THROW(actualMatrix.setFromJsonObject(serActualMatrix));
}
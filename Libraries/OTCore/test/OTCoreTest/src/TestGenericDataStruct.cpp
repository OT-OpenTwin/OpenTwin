#include <gtest/gtest.h>
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"

TEST(GenericDataStructMatrix, FillingWithOneValue)
{
	ot::Variable expectedValue(5);
	ot::GenericDataStructMatrix matrix(1, 1);
	{
		ot::Variable value(5);
		matrix.setValue(0, 0, std::move(value));
	}
	const ot::Variable& actualValue = matrix.getValue(0, 0);

	EXPECT_EQ(actualValue, expectedValue);
}

TEST(GenericDataStructMatrix, Serialization)
{
	ot::GenericDataStructMatrix expectedMatrix(4, 4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			expectedMatrix.setValue(j, i, ot::Variable(i * 4 + j));
		}
	}

	ot::JsonDocument doc;
	ot::JsonObject val;
	expectedMatrix.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("Matrix", val, doc.GetAllocator());
	
	auto serActualMatrix = ot::json::getObject(doc, "Matrix");
	ot::GenericDataStructMatrix actualMatrix(4, 4);
	actualMatrix.setFromJsonObject(serActualMatrix);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			EXPECT_EQ(actualMatrix.getValue(j, i), expectedMatrix.getValue(j, i));
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
	ot::GenericDataStructMatrix expectedMatrix(4, 4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			expectedMatrix.setValue(j, i, ot::Variable(i * 4 + j));
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
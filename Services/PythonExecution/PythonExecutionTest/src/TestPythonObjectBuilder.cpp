#include <gtest/gtest.h>
#include <limits>
#include <optional>
#include <memory>

#include "PythonObjectBuilder.h"
#include "FixturePythonObjectBuilder.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

TEST_F(FixturePythonObjectBuilder, FloatToVariable)
{
	constexpr float expectedValue = std::numeric_limits<float>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue); 
	std::optional<ot::Variable> value = 	builder.getVariable(doubleValue);
	//PythonObjects do not differ between float and double. Its all double
	EXPECT_TRUE(value.value().isDouble());
}

TEST_F(FixturePythonObjectBuilder, DoubleToVariable)
{
	constexpr double expectedValue = std::numeric_limits<double>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue);
	std::optional<ot::Variable> value = builder.getVariable(doubleValue);
	double clearValue = value.value().getDouble();
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, Int32ToVariable)
{
	constexpr int32_t expectedValue = std::numeric_limits<int32_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt32(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	//difference between int32 and int64 cannot be detected in a pythonobject. Thus int32 is stored as a int64.
	EXPECT_TRUE(vValue.value().isInt64());
}

TEST_F(FixturePythonObjectBuilder, Int64ToVariable)
{
	constexpr int64_t expectedValue = std::numeric_limits<int64_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt64(expectedValue);
	PyObject* pyValue = PyLong_FromLongLong(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	int64_t value = vValue.value().getInt64();
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, StringToVariable)
{
	std::string expectedValue = "Blub";
	PythonObjectBuilder builder;
	CPythonObjectNew pValue =  builder.setString(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	std::string value =std::string(vValue.value().getConstCharPtr());
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, TupleCreation)
{
	PythonObjectBuilder builder;
	builder.StartTupleAssemply(4);
	auto val1 = builder.setInt32(1);
	builder << &val1;
	auto val2 = builder.setString("Temp");
	builder << &val2;
	auto val3 = builder.setBool(true);
	builder << &val3;
	auto val4 = builder.setDouble(3.);
	builder << &val4;

	CPythonObjectNew result(builder.getAssembledTuple());
	EXPECT_TRUE(PyTuple_Check(result));
	EXPECT_TRUE(result != nullptr);
	EXPECT_EQ(PyTuple_Size(result), 4);
}

TEST_F(FixturePythonObjectBuilder, Variable)
{
	ot::Variable expected(3.134);

	PythonObjectBuilder builder;
	auto pValue = builder.setVariable(expected);
	auto actual = builder.getVariable(pValue);
	
	ASSERT_EQ(actual, expected);
}

TEST_F(FixturePythonObjectBuilder, VariableList)
{
	std::list<ot::Variable> expected{ ot::Variable(3.134), ot::Variable(5.1), ot::Variable(13.) };


	PythonObjectBuilder builder;
	auto pValue = builder.setVariableList(expected);

	auto actual = builder.getVariableList(pValue);
	ASSERT_EQ(actual.size(), expected.size());
	auto actualVal = actual.begin();
	for (const auto& extectedVal : expected)
	{
		EXPECT_EQ(*actualVal, extectedVal);
		actualVal++;
	}
}


TEST_F(FixturePythonObjectBuilder, GenericDataStructure)
{
	ot::GenericDataStructVector expected ({ 1,2,3,4,5,6 });

	PythonObjectBuilder builder;
	auto pValue = builder.setGenericDataStruct(&expected);
	auto gValue = builder.getGenericDataStruct(pValue);
	auto actual = dynamic_cast<ot::GenericDataStructVector*>(gValue);

	ASSERT_EQ(actual->getNumberOfEntries(), expected.getNumberOfEntries());
}

int initiateNumpyLoc()
{
	import_array1(0);
	return 0;
}

TEST_F(FixturePythonObjectBuilder, GenericDataStructureMatrixToNumpyMatrix)
{
	initiateNumpyLoc();
	double expected = 7.7;
	double data[] = { 1.0, 2.0, 3., 4.0, 5.5, 6., expected,8.8,9.9 };
	npy_intp dims[] = { 3,3,3 };

	ot::GenericDataStructMatrix expectedMatrix(3, 3);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int index = i * 3 + j;
			ot::Variable temp(data[index]);
			expectedMatrix.setValue(j, i,temp);
		}
	}

	PythonObjectBuilder builder;
	CPythonObjectNew pDataStruct(builder.setGenericDataStruct(&expectedMatrix));
	PyObject* numpy_array = pDataStruct;
	pDataStruct.DropOwnership();

	double third_element = *(double*)PyArray_GETPTR2((PyArrayObject*)numpy_array, 0, 2);

	ASSERT_DOUBLE_EQ(third_element, expected);
}

TEST_F(FixturePythonObjectBuilder, NumpyMatrixToGenericDataStructureMatrix)
{
	initiateNumpyLoc();
	double expected = 7.7;
	double data[] = { 1.0, 2.0, 3., 4.0, 5.5, 6., expected,8.8,9.9 };
	npy_intp dims[] = { 3,3,3 };

	ot::GenericDataStructMatrix expectedMatrix(3, 3);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int index = i * 3 + j;
			ot::Variable temp(data[index]);
			expectedMatrix.setValue(j, i, temp);
		}
	}

	PythonObjectBuilder builder;
	CPythonObjectNew pDataStruct(builder.setGenericDataStruct(&expectedMatrix));
	ot::GenericDataStruct* actualDataStruct = builder.getGenericDataStruct(pDataStruct);
	ot::GenericDataStructMatrix* actualMatrix = dynamic_cast<ot::GenericDataStructMatrix*>(actualDataStruct);
	const ot::Variable& expectedValue = expectedMatrix.getValue(2, 3);
	const ot::Variable& actualValue = actualMatrix->getValue(2, 3);
	ASSERT_TRUE( actualValue==expectedValue );
}

TEST_F(FixturePythonObjectBuilder, GenericDataStructureList)
{
	ot::GenericDataStructList expected{ new ot::GenericDataStructVector({ 1.2,2.3,3.4,4.5,5.6,6.7 }),new ot::GenericDataStructVector({ 7,8,9}),new ot::GenericDataStructVector({ 10,12,13,14,15,16 }) };

	PythonObjectBuilder builder;
	auto pValue = builder.setGenericDataStructList(expected);
	auto gValue = builder.getGenericDataStructList(pValue);
	auto actual = dynamic_cast<ot::GenericDataStructVector*>(*gValue.begin());

	ASSERT_EQ(actual->getNumberOfEntries(), (*expected.begin())->getNumberOfEntries());
}





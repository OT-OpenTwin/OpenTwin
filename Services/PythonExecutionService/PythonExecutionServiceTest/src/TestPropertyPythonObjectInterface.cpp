#pragma once
#include <gtest/gtest.h>
#include "FixturePropertyPythonObjectInterface.h"
#include "PropertyPythonObjectInterface.h"
#include "PythonObjectBuilder.h"
#include "EntityProperties.h"
TEST_F(FixturePropertyPythonObjectInterface, PropertyDoubleValueType)
{
	const double expectedValue = 13.;
	
	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PyFloat_Check(value),1);	
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyDoubleGetValue)
{
	const double expectedValue = 13.;
	
	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getDoubleValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyDoubleSetValue)
{
	const double expectedValue = 13.;

	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble());
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setDouble(expectedValue));
	interface.SetValue(pvalue);
	double result = property->getValue();
	
	EXPECT_EQ(result, expectedValue);
}


TEST_F(FixturePropertyPythonObjectInterface, PropertyIntValueType)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PyLong_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyIntGetValue)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getInt32Value(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyIntSetValue)
{
	const int32_t expectedValue = 13;

	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger());
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setInt32(expectedValue));
	interface.SetValue(pvalue);
	int32_t result = property->getValue();

	EXPECT_EQ(result, expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyBoolValueType)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyBool_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyBoolGetValue)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getBoolValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyBoolSetValue)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean());
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setBool(expectedValue));
	interface.SetValue(pvalue);
	bool result = property->getValue();

	EXPECT_EQ(result, expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyStringValueType)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyUnicode_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyStringGetValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getStringValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyStringSetValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString());
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setString(expectedValue));
	interface.SetValue(pvalue);
	std::string result = property->getValue();

	EXPECT_EQ(result, expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertySelectionValueType)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesSelection> property(new EntityPropertiesSelection());
	EntityProperties* properties = new EntityProperties();
	property->createProperty("", "", {expectedValue}, expectedValue, "",*properties);
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyUnicode_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertySelectionGetValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesSelection> handle(new EntityPropertiesSelection());
	EntityProperties* properties = new EntityProperties();
	handle->createProperty("", "test", { expectedValue }, expectedValue, "", *properties);
	std::unique_ptr <EntityPropertiesBase> property(properties->getProperty("test"));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getStringValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertySelectionSetValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesSelection> handle(new EntityPropertiesSelection());
	EntityProperties* properties = new EntityProperties();
	handle->createProperty("", "test", { expectedValue, "Bread"}, "Bread", "", *properties);
	std::unique_ptr<EntityPropertiesSelection> property(dynamic_cast<EntityPropertiesSelection*>(properties->getProperty("test")));
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setString(expectedValue));
	interface.SetValue(pvalue);
	std::string result = property->getValue();

	EXPECT_EQ(result, expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyEntityListGetValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesEntityList> property(new EntityPropertiesEntityList());
	property->setValueName(expectedValue);
	property->setValueID(1);
	PropertyPythonObjectInterface interface(property.get());
	
	CPythonObjectNew value = interface.GetValue();
	PythonObjectBuilder pyObBuilder;
	EXPECT_EQ(pyObBuilder.getStringValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyEntityListValueType)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesEntityList> property(new EntityPropertiesEntityList());
	property->setValueName(expectedValue);
	property->setValueID(1);
	PropertyPythonObjectInterface interface(property.get());

	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyUnicode_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyEntityListSetValue_NotSupported)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesEntityList> property(new EntityPropertiesEntityList());
	PropertyPythonObjectInterface interface(property.get());
	PythonObjectBuilder pyObBuilder;
	CPythonObjectNew pvalue(pyObBuilder.setString(expectedValue));
	EXPECT_ANY_THROW(interface.SetValue(pvalue));
}
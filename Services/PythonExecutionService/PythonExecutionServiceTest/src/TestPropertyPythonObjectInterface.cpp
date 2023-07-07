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

TEST_F(FixturePropertyPythonObjectInterface, PropertyDoubleValue)
{
	const double expectedValue = 13.;
	
	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getDoubleValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyIntValueType)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PyLong_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyIntValue)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getInt32Value(value, ""), expectedValue);
}


TEST_F(FixturePropertyPythonObjectInterface, PropertyBoolValueType)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyBool_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyBoolValue)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getBoolValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyStringValueType)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyUnicode_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyStringValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getStringValue(value, ""), expectedValue);
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

TEST_F(FixturePropertyPythonObjectInterface, PropertySelectionValue)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesSelection> handle(new EntityPropertiesSelection());
	EntityProperties* properties = new EntityProperties();
	handle->createProperty("", "test", { expectedValue }, expectedValue, "", *properties);
	std::unique_ptr <EntityPropertiesBase> property(properties->getProperty("test"));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getStringValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PropertyEntityListNotSupported)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesEntityList> property(new EntityPropertiesEntityList());
	property->setValueName(expectedValue);
	property->setValueID(1);
	PropertyPythonObjectInterface interface(property.get());
	
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getStringValue(value, ""), expectedValue);
}

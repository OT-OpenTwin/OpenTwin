#pragma once
#include <gtest/gtest.h>
#include "FixturePropertyPythonObjectInterface.h"
#include "PropertyPythonObjectInterface.h"
#include "PythonObjectBuilder.h"
#include "EntityProperties.h"
TEST_F(FixturePropertyPythonObjectInterface, PythonObjectTypeIsFloat)
{
	const double expectedValue = 13.;
	
	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PyFloat_Check(value),1);	
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectDoubleValue)
{
	const double expectedValue = 13.;
	
	std::unique_ptr<EntityPropertiesDouble> property(new EntityPropertiesDouble("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getDoubleValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectTypeIsLong)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PyLong_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectIntValue)
{
	const int32_t expectedValue = 13;
	
	std::unique_ptr<EntityPropertiesInteger> property(new EntityPropertiesInteger("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();
	
	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getInt32Value(value, ""), expectedValue);
}


TEST_F(FixturePropertyPythonObjectInterface, PythonObjectTypeIsBool)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyBool_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectBoolValue)
{
	const bool expectedValue = true;

	std::unique_ptr<EntityPropertiesBoolean> property(new EntityPropertiesBoolean("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PythonObjectBuilder::INSTANCE()->getBoolValue(value, ""), expectedValue);
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectTypeIsString)
{
	const std::string expectedValue = "Cheese";

	std::unique_ptr<EntityPropertiesString> property(new EntityPropertiesString("AProperty", expectedValue));
	PropertyPythonObjectInterface interface(property.get());
	CPythonObjectNew value = interface.GetValue();

	EXPECT_EQ(PyUnicode_Check(value), 1);
}

TEST_F(FixturePropertyPythonObjectInterface, PythonObjectStringValue)
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

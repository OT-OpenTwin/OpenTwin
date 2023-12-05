#include <iostream>
#include "gtest/gtest.h"
#include "FixtureMatrix.h"
#include "OpenTwinCore/TypeNames.h"
#include "OpenTwinCore/TemplateTypeName.h"

#include <string>
#include <stdint.h>

TEST(TemplateTypenameGetterTest, TypenameString)
{
	EXPECT_EQ(ot::TemplateTypeName<std::string>::getTypeName(), ot::TypeNames::getStringTypeName());
}


TEST(TemplateTypenameGetterTest, TypenameInt32)
{
	EXPECT_EQ(ot::TemplateTypeName<int32_t>::getTypeName(), ot::TypeNames::getInt32TypeName());
}


TEST(TemplateTypenameGetterTest, TypenameInt64)
{
	EXPECT_EQ(ot::TemplateTypeName<int64_t>::getTypeName(), ot::TypeNames::getInt64TypeName());
}


TEST(TemplateTypenameGetterTest, TypenameDouble)
{
	EXPECT_EQ(ot::TemplateTypeName<double>::getTypeName(), ot::TypeNames::getDoubleTypeName());
}


TEST(TemplateTypenameGetterTest, TypenameChar)
{
	EXPECT_EQ(ot::TemplateTypeName<char>::getTypeName(), ot::TypeNames::getCharTypeName());
}


TEST(TemplateTypenameGetterTest, TypenameFloat)
{
	EXPECT_EQ(ot::TemplateTypeName<float>::getTypeName(), ot::TypeNames::getFloatTypeName());
}
// @otlicense
// File: TypeNameTemplateTest.cpp
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

#include <iostream>
#include "gtest/gtest.h"
#include "FixtureMatrix.h"
#include "OTCore/TypeNames.h"
#include "OTCore/TemplateTypeName.h"

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
// @otlicense
// File: VariableToBSONConverter.cpp
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

#include "VariableToBSONConverter.h"
#include<cassert>

void VariableToBSONConverter::operator()(bsoncxx::builder::basic::document& doc, const ot::Variable& value, const std::string& fieldName)
{
	if (value.isBool())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getBool()));
	}
	else if (value.isConstCharPtr())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getConstCharPtr()));
	}
	else if (value.isDouble())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getDouble()));
	}
	else if (value.isFloat())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getFloat()));
	}
	else if (value.isInt32())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getInt32()));
	}
	else if (value.isInt64())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getInt64()));
	}
	else
	{
		assert(0);
	}
}

void VariableToBSONConverter::operator()(bsoncxx::builder::basic::array& array, const ot::Variable& value)
{
	if (value.isBool())
	{
		array.append(value.getBool());
	}
	else if (value.isConstCharPtr())
	{
		array.append(value.getConstCharPtr());
	}
	else if (value.isDouble())
	{
		array.append(value.getDouble());
	}
	else if (value.isFloat())
	{
		array.append(value.getFloat());
	}
	else if (value.isInt32())
	{
		array.append(value.getInt32());
	}
	else if (value.isInt64())
	{
		array.append(value.getInt64());
	}
	else
	{
		assert(0);
	}
}

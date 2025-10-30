// @otlicense
// File: BSONToVariableConverter.cpp
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

#include "BSONToVariableConverter.h"

ot::Variable BSONToVariableConverter::operator()(const bsoncxx::v_noabi::document::element& element)
{
	auto type = element.type();
	switch (type)
	{
		case bsoncxx::type::k_double:
		{
			return ot::Variable(element.get_double());
		}
		case bsoncxx::type::k_utf8:
		{
			return ot::Variable(element.get_utf8().value.data());
		}
		case bsoncxx::type::k_int32:
		{
			return ot::Variable(element.get_int32());
		}
		case bsoncxx::type::k_int64:
		{
			return ot::Variable(element.get_int64());
		}
		default:
		{
			throw std::exception("Not supported BSON to ot::Variable conversion");
		}
	}
}

ot::Variable BSONToVariableConverter::operator()(const bsoncxx::v_noabi::array::element& element)
{
	auto type = element.type();
	switch (type)
	{
		case bsoncxx::type::k_double:
		{
			return ot::Variable(element.get_double());
		}
		case bsoncxx::type::k_utf8:
		{
			return ot::Variable(element.get_utf8().value.data());
		}
		case bsoncxx::type::k_int32:
		{
			return ot::Variable(element.get_int32());
		}
		case bsoncxx::type::k_int64:
		{
			return ot::Variable(element.get_int64());
		}
		default:
		{
			throw std::exception("Not supported BSON to ot::Variable conversion");
		}
	};
}

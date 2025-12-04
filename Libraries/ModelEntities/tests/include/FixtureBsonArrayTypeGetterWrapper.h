// @otlicense
// File: FixtureBsonArrayTypeGetterWrapper.h
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

#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

#include <string>
#include <vector>
#include <stdint.h>
#include "gtest/gtest.h"

class FixtureBsonArrayTypeGetterWrapper : public testing::Test
{
public:
	bsoncxx::v_noabi::array::view GetUTF8StringArray();
	bsoncxx::v_noabi::array::view GetCharArray();
	bsoncxx::v_noabi::array::view GetInt32Array();
	bsoncxx::v_noabi::array::view GetInt64Array();
	bsoncxx::v_noabi::array::view GetDoubleArray();
	bsoncxx::v_noabi::array::view GetFloatArray();
	~FixtureBsonArrayTypeGetterWrapper() {};

private:
	std::vector<std::string> exampleDataUTF8String{ "a", "b", "c", "d" };
	std::vector<char> exampleDataChar{ 'a', 'b', 'c', 'd' };
	std::vector<int32_t> exampleDataInt32{1,2,3,4};
	std::vector<int64_t> exampleDataInt64{1,2,3,4};
	std::vector<double> exampleDataDouble{1.,2.,3.,4.};
	//Float is not supported and is being used as an example for the template default case.
	std::vector<float> exampleDataFloat{1.f,2.f,3.f,4.f};

	template<class T> 
	bsoncxx::v_noabi::array::view CreateArrayView(std::vector<T> exampleData);

};

template<class T>
inline bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::CreateArrayView(std::vector<T> exampleData)
{
	auto dataArray = bsoncxx::builder::basic::array();

	for (int i = 0; i < exampleData.size(); i++)
	{
		dataArray.append(exampleData[i]);
	}

	bsoncxx::builder::basic::document storage;
	storage.append(bsoncxx::builder::basic::kvp("exampleData", dataArray));
	return storage.view()["exampleData"].get_array().value;
}
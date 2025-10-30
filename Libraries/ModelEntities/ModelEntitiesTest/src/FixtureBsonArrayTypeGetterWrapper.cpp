// @otlicense
// File: FixtureBsonArrayTypeGetterWrapper.cpp
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
#include "FixtureBsonArrayTypeGetterWrapper.h"



bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetUTF8StringArray()
{
	return CreateArrayView<std::string>(exampleDataUTF8String);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetCharArray()
{
	return bsoncxx::v_noabi::array::view();
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetInt32Array()
{
	return CreateArrayView<int32_t>(exampleDataInt32);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetInt64Array()
{
	return CreateArrayView<int64_t>(exampleDataInt64);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetDoubleArray()
{
	return CreateArrayView<double>(exampleDataDouble);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetFloatArray()
{
	return CreateArrayView<float>(exampleDataFloat);
}

// @otlicense
// File: BsonArrayTypeGetterWrapper.hpp
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
#include "BsonArrayTypeGetterWrapper.h"


template<class T>
T BsonArrayTypeGetterWrapper<T>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	throw std::exception("Type is not supported by this wrapper");
}


template<>
inline int32_t BsonArrayTypeGetterWrapper<int32_t>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_int32();
}

template<>
inline int64_t BsonArrayTypeGetterWrapper<int64_t>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_int64();
}

template<>
inline std::string BsonArrayTypeGetterWrapper<std::string>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_utf8().value.data();
}

template<>
inline double BsonArrayTypeGetterWrapper<double>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_double();
}
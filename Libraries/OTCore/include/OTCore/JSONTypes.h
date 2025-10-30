// @otlicense
// File: JSONTypes.h
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

// rapidjson header
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	//! @brief Size type
	typedef rapidjson::SizeType JsonSizeType;

	//! @brief Writable JSON value
	typedef rapidjson::Value JsonValue;

	//! @brief Allocator used for writing to JSON values
	typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> JsonAllocator;

	//typedef rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonObjectHandle;

	//! @brief Read only JSON Object
	typedef rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonObject;

	typedef std::list<ConstJsonObject> ConstJsonObjectList;

	//typedef rapidjson::GenericArray<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonArrayHandle;

	//! @brief Read only JSON Array
	typedef rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonArray;

	typedef std::list<ConstJsonArray> ConstJsonArrayList;

	//! @brief Read only JSON Object iterator
	typedef rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonMemberIterator;



}
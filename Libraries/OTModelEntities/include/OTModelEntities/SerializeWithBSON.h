// @otlicense
// File: SerializeWithBSON.h
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
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

class __declspec(dllexport) SerializeWithBSON
{
public:
	virtual bsoncxx::builder::basic::document SerializeBSON() const = 0 ;
	virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) = 0;
	void DeserializeBSON(bsoncxx::v_noabi::types::bson_value::view& storage)
	{
		auto subDocument = storage.get_document();
		DeserializeBSON(subDocument);
	};
};

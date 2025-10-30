// @otlicense
// File: GenericBsonDocument.h
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

#include <vector>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include "GenericDocument.h"

class GenericBsonDocument : public GenericDocument
{
public:
	void addAllFieldsToDocument(bsoncxx::builder::basic::document& _storage) const;

private:
	std::vector<char> m_forbiddenFieldNameContent{ '$', '.' };
	
	//virtual void CheckForIlligalName(std::string fieldName) override;
};
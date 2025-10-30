// @otlicense
// File: GenericBsonDocument.cpp
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

#include "GenericBsonDocument.h"
#include "VariableToBSONConverter.h"
void GenericBsonDocument::addAllFieldsToDocument(bsoncxx::builder::basic::document& _storage) const
{
	VariableToBSONConverter converter;
	for (auto mapEntry : m_fields)
	{
		std::list<ot::Variable>& fieldValues = mapEntry.second;
		const std::string fieldName = mapEntry.first;
		if (fieldValues.size() == 1)
		{
			const auto& value = *fieldValues.begin();
			converter(_storage,value,fieldName);
		}
		else
		{
			auto valueArray = bsoncxx::builder::basic::array();
			for (auto& fieldValue : fieldValues)
			{
				converter(valueArray, fieldValue);
			}
			_storage.append(bsoncxx::builder::basic::kvp(fieldName, valueArray));
		}
	}
}

//void GenericBsonDocument::CheckForIlligalName(std::string fieldName)
//{
//	for (char& forbiddenCharacters : _forbiddenFieldNameContent)
//	{
//		if (fieldName.find(forbiddenCharacters) != std::string::npos)
//		{
//			std::string errorMessage = "Character  is not permitted to be contained in a fieldname.\n";
//			throw std::exception(errorMessage.insert(10, 1, forbiddenCharacters).c_str());
//		}
//	}
//}

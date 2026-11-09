// @otlicense
// File: JSONVectoriser.cpp
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

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/JSON/JSONVectoriser.h"

const std::string ot::JSONVectoriser::m_separator = "~";

void ot::JSONVectoriser::vectorise(const JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase, const VectorisationFlags& _flags)
{
    if (_value.IsObject())
    {
        std::string separator = _nameBase.empty() ? "" : m_separator;
        for (auto& element : _value.GetObject())
        {
            std::string name = _nameBase + separator + element.name.GetString();
            ot::String::removeControlCharacters(name);

            if (element.value.IsObject())
            {
                if (!_flags.has(VectorisationFlag::AddLeavesOnly))
                {
                    _allEntries.push_back(name);
                }

                if (!_flags.has(VectorisationFlag::TopLevelOnly))
                {
                    vectorise(element.value, _allEntries, name, _flags);
                }
            }
            else
            {
				_allEntries.push_back(name);
            }
        }
    }
}

std::optional<std::reference_wrapper<const ot::JsonValue>> ot::JSONVectoriser::getValue(const JsonDocument& _structure, const std::string& _fieldName)
{
    std::list<std::string> entries = String::split(_fieldName, m_separator);

    const ot::JsonValue* cur = &_structure;
    for (const std::string& entry : entries)
    {
        if (!cur->IsObject())
        {
            return std::nullopt;
        }

        auto obj = cur->GetObject();
        if (!json::exists(obj, entry))
        {
            return std::nullopt;
        }

        const JsonValue& value = obj[entry.c_str()];

        if (entry == entries.back()) 
        {
            return value;
        }

        cur = &value;
    }

    return std::nullopt;
}
// @otlicense
// File: ShortParameterDescription.h
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
#include <string>

struct AdditionalDependency
{
	std::string m_label = "";
	std::string m_unit = "";
	std::string m_value = "";

    bool operator==(const AdditionalDependency& other) const {
        return m_label == other.m_label &&
            m_unit == other.m_unit &&
            m_value == other.m_value;
    }
};

using DependencyList = std::vector<AdditionalDependency>;

inline DependencyList makeSortedKey(DependencyList deps)
{
    std::sort(deps.begin(), deps.end(), [](const AdditionalDependency& a, const AdditionalDependency& b) {
        return std::tie(a.m_label, a.m_unit, a.m_value) < std::tie(b.m_label, b.m_unit, b.m_value);
        });
    return deps;
}

namespace std {
    template<>
    struct hash<DependencyList> {
        size_t operator()(const DependencyList& _list) const {
            size_t seed = _list.size();
            for (const auto& entry : _list) 
            {
                auto h = std::hash<std::string>{}(entry.m_label);
                h ^= std::hash<std::string>{}(entry.m_unit) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<std::string>{}(entry.m_value) + 0x9e3779b9 + (h << 6) + (h >> 2);
                seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

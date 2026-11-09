// @otlicense
// File: HeaderFilterState.cpp
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
#include "OTWidgets/Header/HeaderFilterState.h"

void ot::HeaderFilterState::clear()
{
	m_selectedFilters.clear();

	m_hoveredFilter = -1;
	m_pressedFilter = -1;
}

bool ot::HeaderFilterState::hasActiveFilter() const
{
	for (const auto& it : m_selectedFilters)
	{
		if (!it.second.isEmpty())
		{
			return true;
		}
	}

	return false;
}

bool ot::HeaderFilterState::isFilterActive(int _index) const
{
	const auto it = m_selectedFilters.find(_index);
	if (it != m_selectedFilters.end())
	{
		return !it->second.isEmpty();
	}
	else
	{
		return false;
	}
}

QStringList ot::HeaderFilterState::getFilter(int _index) const
{
	auto it = m_selectedFilters.find(_index);
	if (it != m_selectedFilters.end())
	{
		return it->second;
	}
	else
	{
		return QStringList();
	}
}

std::map<int, QStringList> ot::HeaderFilterState::getActiveFilters() const
{
	std::map<int, QStringList> result;
	for (const auto& it : m_selectedFilters)
	{
		if (!it.second.isEmpty())
		{
			result.insert(result.end(), it);
		}
	}
	return result;
}

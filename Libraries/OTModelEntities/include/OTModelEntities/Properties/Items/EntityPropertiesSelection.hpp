// @otlicense
// File: EntityPropertiesSelection.hpp
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

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesSelection.h"

template <typename TContainer>
void EntityPropertiesSelection::resetOptionsImpl(const TContainer& _options)
{
	// Clear and reserve space
	m_options.clear();
	m_options.reserve(_options.size());
	
	// Add options
	for (const auto& item : _options)
	{
		this->addOption(item);
	}
	m_options.shrink_to_fit();

	// Check whether the current value is still valid and reset to default if not
	if (m_options.empty())
	{
		m_value = "";
	}
	else
	{
		m_value = m_options.front().first;
	}

}
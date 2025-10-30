// @otlicense
// File: MetadataAssemblyData.h
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
#include "EntityParameterizedDataCategorization.h"
#include "EntityTableSelectedRanges.h"

//! @brief Container for groups of TableSelectionEntities that describe the same metadata categorization.
struct MetadataAssemblyData
{
public:
	MetadataAssemblyData() = default;
	~MetadataAssemblyData() = default;
	MetadataAssemblyData& operator=(const MetadataAssemblyData& _other) = delete;
	MetadataAssemblyData& operator=(MetadataAssemblyData&& _other) = delete;

	MetadataAssemblyData(MetadataAssemblyData&& _other) noexcept
		:m_allSelectionRanges(std::move(_other.m_allSelectionRanges)), m_next(std::move(_other.m_next)), m_dataCategory(_other.m_dataCategory)
	{
		_other.m_next = nullptr;		
		_other.m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::UNKNOWN;
	}

	MetadataAssemblyData(const MetadataAssemblyData& _other)
		: m_allSelectionRanges(_other.m_allSelectionRanges), m_next(_other.m_next), m_dataCategory(_other.m_dataCategory)
	{}
	/**
	 * Used to connect a MSMD MetadataAssemblyData with its parameter MetadataAssemblyData. 
	 * The parameter MetadataAssemblyData gets connected with the quantity MetadataAssemblyData of the same MSMD.
	 */
	MetadataAssemblyData* m_next = nullptr;
	std::list< std::shared_ptr<EntityTableSelectedRanges>> m_allSelectionRanges;
	EntityParameterizedDataCategorization::DataCategorie m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::UNKNOWN;
	char m_decimalSeparator = '.';
};

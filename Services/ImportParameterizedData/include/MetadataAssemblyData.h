/*****************************************************************//**
 * \file   MetadataAssemblyData.h
 * \brief  Container for groups of TableSelectionEntities that describe the same metadata categorization.
 * 
 * \author Wagner
 * \date   March 2023
 *********************************************************************/
#pragma once
#include "EntityParameterizedDataCategorization.h"
#include "EntityTableSelectedRanges.h"

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

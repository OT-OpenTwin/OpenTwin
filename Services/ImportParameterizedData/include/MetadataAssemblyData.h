/*****************************************************************//**
 * \file   MetadataAssemblyData.h
 * \brief  
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
	MetadataAssemblyData() {};
	MetadataAssemblyData(MetadataAssemblyData&& other) noexcept
	{
		this->allSelectionRanges.merge(other.allSelectionRanges);
		this->next = other.next;
		this->dataCategory = other.dataCategory;
	}
	MetadataAssemblyData(const MetadataAssemblyData& other)
	{
		this->allSelectionRanges = other.allSelectionRanges;
		this->next = other.next;
		this->dataCategory = other.dataCategory;
	}
	/**
	 * Used to connect a MSMD MetadataAssemblyData with its parameter MetadataAssemblyData. 
	 * The parameter MetadataAssemblyData gets connected with the quantity MetadataAssemblyData of the same MSMD.
	 */
	MetadataAssemblyData* next = nullptr;
	std::list< std::shared_ptr<EntityTableSelectedRanges>> allSelectionRanges;
	EntityParameterizedDataCategorization::DataCategorie dataCategory = EntityParameterizedDataCategorization::DataCategorie::UNKNOWN;
};

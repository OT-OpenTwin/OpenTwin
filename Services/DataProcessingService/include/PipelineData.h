#pragma once

#include "DataDocument.h"

#include "MetadataCampaign.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataQuantity.h"


#include <memory>
#include <map>


using PipelineDataDocumentList = std::list<PipelineDataDocument>;
//! @brief Data struct that is send down the pipeline connections
struct PipelineData
{
	const MetadataCampaign* m_campaign = nullptr;
	const MetadataSeries* m_series = nullptr;

	const MetadataParameter* m_parameter = nullptr;
	const MetadataQuantity* m_quantity = nullptr;
	const MetadataQuantityValueDescription* m_quantityDescription = nullptr;

	std::map<std::string, ot::Variable> m_fixedParameter; 
	PipelineDataDocumentList m_data;

	void copyMetaDataReferences(PipelineData& _other);
};

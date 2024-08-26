#pragma once

#include "OTCore/GenericDataStruct.h"

#include "MetadataCampaign.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataQuantity.h"

#include <memory>

using GenericDataList = std::list<std::shared_ptr<ot::GenericDataStruct>>;
struct PipelineData
{
	GenericDataList m_data;

	const MetadataCampaign* m_campaign = nullptr;
	const MetadataSeries* m_series = nullptr;
	const MetadataParameter* m_parameter = nullptr;
	const MetadataQuantity* m_quantity = nullptr;
	const MetadataQuantityValueDescription* m_quantityDescription = nullptr;

	void copyMetaDataReferences(PipelineData& _other);

};

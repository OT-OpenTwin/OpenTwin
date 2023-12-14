#include "DigitalTwinExtender.h"

DigitalTwinExtender::DigitalTwinExtender(MetadataCampaign& metadataCampaign)
	:_metadataCampaign(metadataCampaign)
{

}

void DigitalTwinExtender::AddQuantity(MetadataQuantity&& quantity, const std::string metadataSeriesName)
{
	std::list<MetadataSeries>& allSeriesMetadata = _metadataCampaign.getSeriesMetadata();

	MetadataSeries* selectedMetadataSeries = nullptr;
	for (auto& metadataSeries : allSeriesMetadata)
	{
		if (metadataSeries.getName() == metadataSeriesName)
		{
			selectedMetadataSeries = &metadataSeries;
		}
	}
	if (selectedMetadataSeries == nullptr)
	{
		throw std::exception("Cannot add a quantity to an series that is not part of the campaign");
	}
	else
	{
		quantity.quantityIndex = FindNextFreeQuantityIndex();
		quantity.quantityAbbreviation = _quantityAbbreviationBase + std::to_string(quantity.quantityIndex);

		selectedMetadataSeries->AddQuantity(std::move(quantity));
	}
		
}

uint32_t DigitalTwinExtender::FindNextFreeQuantityIndex()
{
	auto& quantitiesByName = _metadataCampaign.getMetadataQuantitiesByName();
	return quantitiesByName.size();
}

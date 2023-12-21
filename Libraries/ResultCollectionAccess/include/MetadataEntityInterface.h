#pragma once
#include "MetadataCampaign.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "MetadataEntry.h"

class __declspec(dllexport) MetadataEntityInterface
{
public:
	MetadataCampaign CreateCampaign(std::shared_ptr<EntityMetadataCampaign> rmd, std::list<std::shared_ptr<EntityMetadataSeries>> msmds);
	void StoreCampaign(ot::components::ModelComponent& modelComponent, MetadataCampaign& metaDataCampaign);
	void StoreCampaign(ot::components::ModelComponent& modelComponent, std::list<const MetadataSeries*>& seriesMetadata);
	void StoreCampaign(ot::components::ModelComponent& modelComponent,  MetadataCampaign& metaDataCampaign, std::list<const MetadataSeries*>& seriesMetadata);
private:
	const std::string _nameField = "Name";
	const std::string _datatypeField = "Datatype";
	const std::string _valuesField = "Values";
	std::list<ot::UID> _newEntityIDs;
	std::list<ot::UID> _newEntityVersions;

	void ExtractCampaignMetadata(MetadataCampaign& measurementCampaign, std::shared_ptr<EntityMetadataCampaign> rmd);
	void ExtractSeriesMetadata(MetadataCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMetadataSeries>> msmds);
	


	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataObjects(const GenericDocument& document);
	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataFields(const GenericDocument& document);
};


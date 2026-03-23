#pragma once
#include "OTResultDataAccess/ResultCollection/ProjectToCollectionConverter.h"
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include <memory>

class MetadataHandler
{
public:
	
	MetadataCampaign getMetadataCampaign(const std::string& _projectName);

private: 
	std::unique_ptr<ProjectToCollectionConverter> m_projectToCollectionConverter;
};

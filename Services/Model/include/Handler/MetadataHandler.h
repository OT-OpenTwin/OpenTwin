#pragma once
#include "OTResultDataAccess/ResultCollection/ProjectToCollectionConverter.h"
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
#include "OTCore/QueryDescription/DataLakeQueryCfg.h"
#include <memory>

class MetadataHandler
{
public:
	
	MetadataCampaign getMetadataCampaign(const std::string& _projectName, std::string& _collectionName);
	ot::DataLakeAccessCfg createConfig(const MetadataCampaign& _campaign, const std::string& _collectionName, const DataLakeQueryCfg& _queryCfg);
private: 
	std::unique_ptr<ProjectToCollectionConverter> m_projectToCollectionConverter;
};

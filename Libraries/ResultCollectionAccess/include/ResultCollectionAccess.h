#pragma once
#include <string>
#include <list>

#include "OTServiceFoundation/ModelComponent.h"
#include "Document/DocumentAccess.h"
#include "EntityBase.h"
#include "ResultDataStorageAPI.h"

#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"

class ClassFactory;


class __declspec(dllexport) ResultCollectionAccess
{
public:
	ResultCollectionAccess(const std::string& projectName, ot::components::ModelComponent& modelComponent, ClassFactory* classFactory);
	ResultCollectionAccess(const std::string& crossProjectName, ot::components::ModelComponent& modelComponent, const std::string& sessionServiceURL, const std::string& modelServiceURL, ClassFactory* classFactory);
	virtual ~ResultCollectionAccess() {};

	const std::list<std::string> ListAllSeriesNames() const;
	const std::list<std::string> ListAllParameterNames() const;
	const std::list<std::string> ListAllQuantityNames() const;

	const MetadataSeries* FindMetadataSeries(const std::string& name);
	const MetadataParameter* FindMetadataParameter(const std::string& name);
	const MetadataQuantity* FindMetadataQuantity(const std::string& name);

	const MetadataCampaign& getMetadataCampaign() const { return _metadataCampaign; }
	//ToDo: Query Methode
protected:
	ot::components::ModelComponent& _modelComponent;
	DataStorageAPI::ResultDataStorageAPI _dataStorageAccess;
	MetadataCampaign _metadataCampaign;

private:
	void LoadExistingCampaignData(ClassFactory* classFactory);
	std::vector<EntityBase*> FindAllExistingMetadata(ClassFactory* classFactory);
};

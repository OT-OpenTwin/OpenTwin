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


class __declspec(dllexport) ResultMetadataAccess
{
public:
	ResultMetadataAccess(const std::string& collectionName, ot::components::ModelComponent* modelComponent, ClassFactory* classFactory);
	ResultMetadataAccess(const std::string& crossProjectName, ot::components::ModelComponent* modelComponent, ClassFactory* classFactory, const std::string& sessionServiceURL);
	ResultMetadataAccess(ResultMetadataAccess&& other);
	ResultMetadataAccess& operator=(ResultMetadataAccess&& other) noexcept;

	virtual ~ResultMetadataAccess() {};

	const std::list<std::string> ListAllSeriesNames() const;
	const std::list<std::string> ListAllParameterNames() const;
	const std::list<std::string> ListAllQuantityNames() const;

	const MetadataSeries* FindMetadataSeries(const std::string& name);
	const MetadataParameter* FindMetadataParameter(const std::string& name);
	const MetadataQuantity* FindMetadataQuantity(const std::string& name);

	const MetadataCampaign& getMetadataCampaign() const { return _metadataCampaign; }

	const std::string& getCollectionName() const { return _collectionName; }
protected:
	std::string _collectionName;
	ot::components::ModelComponent* _modelComponent;
	MetadataCampaign _metadataCampaign;

private:
	void LoadExistingCampaignData(ClassFactory* classFactory);
	std::vector<EntityBase*> FindAllExistingMetadata(ClassFactory* classFactory);
};

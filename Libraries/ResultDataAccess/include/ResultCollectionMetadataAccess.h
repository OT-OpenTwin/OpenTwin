#pragma once
#include <string>
#include <list>

#include "OTServiceFoundation/ModelComponent.h"

#include "EntityBase.h"

#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"

class ClassFactory;


class __declspec(dllexport) ResultCollectionMetadataAccess
{
public:
	ResultCollectionMetadataAccess(const std::string& collectionName, ot::components::ModelComponent* modelComponent, ClassFactory* classFactory);
	ResultCollectionMetadataAccess(const std::string& crossCollectionName, ot::components::ModelComponent* modelComponent, ClassFactory* classFactory, const std::string& sessionServiceURL);
	ResultCollectionMetadataAccess(ResultCollectionMetadataAccess&& other) noexcept;
	ResultCollectionMetadataAccess& operator=(ResultCollectionMetadataAccess&& other) noexcept;

	virtual ~ResultCollectionMetadataAccess() {};

	const std::list<std::string> listAllSeriesNames() const;
	const std::list<std::string> listAllParameterLabels() const;
	const std::list<std::string> listAllQuantityLabels() const;

	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(const std::string& _label);
	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(ot::UID _index);
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(const std::string& _label);
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(ot::UID _index);
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(const std::string& _label);
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(ot::UID _index); //Needs to look through the value descriptions!

	std::list< MetadataQuantity*>findQuantityWithSameName(const std::string& _name);
	std::list< MetadataParameter*>findParameterWithSameName(const std::string& _name);

	const MetadataCampaign& getMetadataCampaign() const { return m_metadataCampaign; }

	const std::string& getCollectionName() const { return m_collectionName; }

	bool collectionHasMetadata() const { return m_metadataExistInProject; }
protected:
	ResultCollectionMetadataAccess() {};
	bool m_metadataExistInProject = false;
	std::string m_collectionName;
	ot::components::ModelComponent* m_modelComponent = nullptr;
	MetadataCampaign m_metadataCampaign;

private:
	void loadExistingCampaignData(ClassFactory* classFactory);
	std::vector<EntityBase*> findAllExistingMetadata(ClassFactory* classFactory);
};

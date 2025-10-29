// @otlicense

#pragma once
#include <string>
#include <list>

#include "OTServiceFoundation/ModelComponent.h"

#include "EntityBase.h"

#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"

class __declspec(dllexport) ResultCollectionMetadataAccess
{
public:
	ResultCollectionMetadataAccess(const std::string& collectionName, ot::components::ModelComponent* modelComponent);
	ResultCollectionMetadataAccess(const std::string& crossCollectionName, ot::components::ModelComponent* modelComponent, const std::string& sessionServiceURL);
	ResultCollectionMetadataAccess(ResultCollectionMetadataAccess&& other) noexcept;
	ResultCollectionMetadataAccess& operator=(ResultCollectionMetadataAccess&& other) noexcept;

	virtual ~ResultCollectionMetadataAccess() {};

	const std::list<std::string> listAllSeriesNames() const;
	const std::list<std::string> listAllParameterLabels() const;
	const std::list<std::string> listAllQuantityLabels() const;

	const std::list<std::string> listAllParameterLabelsFromSeries(const std::string& _label) const;
	const std::list<std::string> listAllQuantityLabelsFromSeries(const std::string& _label) const;

	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(ot::UID _index) const ;
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(ot::UID _index) const;
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(ot::UID _index) const; //Needs to look through the value descriptions!

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
	void loadExistingCampaignData();
	std::vector<EntityBase*> findAllExistingMetadata();
};

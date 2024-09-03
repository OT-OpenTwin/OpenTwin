#pragma once

#include "ResultCollectionMetadataAccess.h"
#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"
#include "MetadataEntry.h"
#include "QuantityContainer.h"

#include "DatasetDescription.h"
#include "ResultDataStorageAPI.h"
#include <string>

class ClassFactory;

class __declspec(dllexport) ResultCollectionExtender : public ResultCollectionMetadataAccess
{
public:
	ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent, ClassFactory* _classFactory, const std::string& _ownerServiceName);
	ResultCollectionExtender(const ResultCollectionExtender& _other) = delete;
	ResultCollectionExtender& operator=(const ResultCollectionExtender& _other) = delete;
	~ResultCollectionExtender() = default;
	
	void setSaveModel(bool _saveModel) { m_saveModel = _saveModel; }

	ot::UID buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata);
	ot::UID buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>&& _seriesMetadata);

	bool campaignMetadataWithSameNameExists(std::shared_ptr<MetadataEntry> _metadata);
	bool campaignMetadataWithSameValueExists(std::shared_ptr<MetadataEntry> _metadata);
	
	//! @brief Throws exception if any parameter/quantity constellation is unvalid.
	void processDataPoints(DatasetDescription* dataDescription, uint64_t seriesMetadataIndex);

	/// <summary>
	/// Potentially overrides an entry with the same field name. Use existence checks beforehand to look into that matter.
	/// </summary>
	/// <param name="metadata"></param>
	void addCampaignMetadata(std::shared_ptr<MetadataEntry> _metadata);
	
	void storeCampaignChanges();
	bool removeSeries(ot::UID _uid);
protected:
	inline bool invariant() { return true; }

	ResultCollectionExtender() {};
	bool m_requiresUpdateMetadataCampaign = false;
	std::list<const MetadataSeries*> m_seriesMetadataForStorage;
	
	std::map<std::string,uint32_t> m_parameterBuckets;
	const std::string m_ownerServiceName;

	ot::UIDList addCampaignContextDataToParameters(DatasetDescription& _dataDescription);
	void addCampaignContextDataToQuantities(DatasetDescription& _dataDescription, ot::UIDList& _dependingParameter);
	ot::UID createNewSeries(std::list< DatasetDescription>& _dataDescription, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata);
	void addMetadataToSeries(std::list< DatasetDescription>& _dataDescription, MetadataSeries& _newSeries);

	std::map<std::string, std::list<MetadataQuantity*>> m_quantitiesUpForStorageByName;
	std::map<std::string, std::list<MetadataParameter*>> m_parameterUpForStorageByName;

	virtual const uint64_t findNextFreeSeriesIndex();
	virtual const uint64_t findNextFreeQuantityIndex();
	virtual const uint64_t findNextFreeParameterIndex();

	bool quantityIsCorrectlySet(MetadataQuantity& _quantity);
	bool parameterIsCorrectlySet(MetadataParameter& _parameter);

	bool m_saveModel = true;
};			  



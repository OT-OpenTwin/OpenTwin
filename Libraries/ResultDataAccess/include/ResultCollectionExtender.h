#pragma once


#include "ResultMetadataAccess.h"
#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"
#include "MetadataEntry.h"
#include "QuantityContainer.h"
#include "OTCore/CoreTypes.h"
#include "DatasetDescription.h"
#include "DatasetDescription1D.h"
#include "ResultDataStorageAPI.h"
#include <string>

class ClassFactory;

class __declspec(dllexport) ResultCollectionExtender : public ResultMetadataAccess
{
public:
	ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent, ClassFactory* _classFactory, const std::string& _ownerServiceName);
	ResultCollectionExtender(const ResultCollectionExtender& _other) = delete;
	ResultCollectionExtender& operator=(const ResultCollectionExtender& _other) = delete;
	~ResultCollectionExtender();
	
	void setBucketSize(const uint64_t _bucketSize) { m_bucketSize = _bucketSize; }
	void setSaveModel(bool _saveModel) { m_saveModel = _saveModel; }

	ot::UID buildSeriesMetadata(std::list<DatasetDescription*>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata);
	ot::UID buildSeriesMetadata(std::list<DatasetDescription*>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>&& _seriesMetadata);

	bool campaignMetadataWithSameNameExists(std::shared_ptr<MetadataEntry> _metadata);
	bool campaignMetadataWithSameValueExists(std::shared_ptr<MetadataEntry> _metadata);
	
	void processDataPoints(DatasetDescription1D& dataDescription, uint64_t seriesMetadataIndex);

	/// <summary>
	/// Potentially overrides an entry with the same field name. Use existence checks beforehand to look into that matter.
	/// </summary>
	/// <param name="metadata"></param>
	void addCampaignMetadata(std::shared_ptr<MetadataEntry> _metadata);
	
	void storeCampaignChanges();
	void flushQuantityContainer();

	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);
	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);

protected:
	inline bool invariant() { return true; }

private:
	friend class FixtureResultCollectionExtender;
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;

	bool m_requiresUpdateMetadataCampaign;
	std::list<const MetadataSeries*> m_seriesMetadataForStorage;
	std::vector<QuantityContainer> m_quantityContainer;

	std::map<std::string,uint32_t> m_parameterBuckets;
	uint64_t m_bucketSize = 1;
	const uint32_t m_bufferSize = 50;
	const std::string m_ownerServiceName;

	ot::UIDList addCampaignContextDataToParameters(DatasetDescription& _dataDescription);
	void addCampaignContextDataToQuantities(DatasetDescription& _dataDescription, ot::UIDList& _dependingParameter);
	ot::UID createNewSeries(std::list< DatasetDescription*>& _dataDescription, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata);
	void AddMetadataToSeries(std::list< DatasetDescription*>& _dataDescription, MetadataSeries& _newSeries);

	const uint64_t findNextFreeSeriesIndex();
	const uint64_t findNextFreeQuantityIndex();
	const uint64_t findNextFreeParameterIndex();

	bool m_saveModel = true;
};			  



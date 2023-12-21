#pragma once
#include "ResultCollectionAccess.h"
#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"
#include "QuantityContainer.h"

class __declspec(dllexport) ResultCollectionExtender : public ResultCollectionAccess
{
public:
	ResultCollectionExtender(const std::string& projectName, ot::components::ModelComponent& modelComponent);
	ResultCollectionExtender(const ResultCollectionExtender& other) = delete;
	ResultCollectionExtender operator=(const ResultCollectionExtender& other) = delete;
	~ResultCollectionExtender();
	
	void AddSeries(MetadataSeries&& series);
	bool CampaignMetadataWithSameNameExists(std::shared_ptr<MetadataEntry> metadata);
	bool CampaignMetadataWithSameValueExists(std::shared_ptr<MetadataEntry> metadata);
	
	/// <summary>
	/// Potentially overrides an entry with the same field name. Use existence checks beforehand to look into that matter.
	/// </summary>
	/// <param name="metadata"></param>
	void AddCampaignMetadata(std::shared_ptr<MetadataEntry> metadata);
	
	void StoreCampaignChanges();
	void FlushQuantityContainer();

	void AddQuantityContainer(uint32_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, uint32_t quantityIndex, const ot::Variable& quantityValue);

private:
	friend class FixtureResultCollectionExtender;
	
	bool _requiresUpdateMetadataCampaign;
	std::list<const MetadataSeries*> _seriesMetadataForStorage;
	std::vector<QuantityContainer> _quantityContainer;

	std::map<std::string,uint32_t> _parameterBuckets;
	const uint64_t _bucketSize = 1;
	const uint32_t _bufferSize = 50;
	const std::string _parameterAbbreviationBase = "P_";
	const std::string _quantityAbbreviationBase = "Q_";

	const uint64_t FindNextFreeSeriesIndex();
	const uint64_t FindNextFreeQuantityIndex();
	const uint64_t FindNextFreeParameterIndex();
};			  



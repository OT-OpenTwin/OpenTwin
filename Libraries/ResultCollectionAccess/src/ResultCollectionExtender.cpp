#include "ResultCollectionExtender.h"
#include "MetadataEntryComperator.h"
#include <set>
#include <cassert>
#include <algorithm>

ResultCollectionExtender::ResultCollectionExtender(const std::string& projectName, ot::components::ModelComponent& modelComponent)
	:ResultCollectionAccess(projectName,modelComponent), _requiresUpdateMetadataCampaign(false)
{
	_quantityContainer.reserve(_bufferSize);
}

ResultCollectionExtender::~ResultCollectionExtender()
{
	if (_quantityContainer.size() != 0)
	{
		FlushQuantityContainer();
	}
}

void ResultCollectionExtender::AddSeries(MetadataSeries&& series)
{
	uint32_t seriesIndex;
	const MetadataSeries* existingSeries = FindMetadataSeries(series.getName());
	if (existingSeries != nullptr)
	{
		seriesIndex = existingSeries->getSeriesIndex();
	}
	else
	{
		seriesIndex = FindNextFreeSeriesIndex();
	}

	MetadataSeries newSeries(series.getName(), seriesIndex);

	for (const auto& metadata : series.getMetadata())
	{
		newSeries.AddMetadata(metadata.second);
	}

	uint32_t startIndex = FindNextFreeQuantityIndex();
	for (auto& quantity : series.getQuantities())
	{
		const std::string newQuantityName = quantity.quantityName;
		const MetadataQuantity* addedQuantityInExistingParameter = FindMetadataQuantity(newQuantityName);
		if (addedQuantityInExistingParameter == nullptr)
		{
			auto& quantityForEditing = const_cast<MetadataQuantity&>(quantity);
			quantityForEditing.quantityIndex = startIndex++;
			quantityForEditing.quantityAbbreviation = _quantityAbbreviationBase + std::to_string(quantity.quantityIndex);
			newSeries.AddQuantity(std::move(quantityForEditing));
		}
		//Assumes that the metadata and type of this quantity are the same as before. 
	}

	startIndex = FindNextFreeParameterIndex();
	for (auto& parameter : series.getParameter())
	{
		const std::string newParameterName = parameter.parameterName;
		const MetadataParameter* addedParameterInExistingParameter = FindMetadataParameter(newParameterName);
		
		auto& parameterForEditing = const_cast<MetadataParameter&>(parameter);
		if (addedParameterInExistingParameter != nullptr)
		{
			const std::list<ot::Variable>& values = addedParameterInExistingParameter->values;
			std::list<ot::Variable> valueDifference;
			std::set_difference(values.begin(), values.end(), parameter.values.begin(), parameter.values.end(), valueDifference.begin());
			if (valueDifference.size() != 0)
			{
				parameterForEditing.values = valueDifference;
				newSeries.AddParameter(std::move(parameterForEditing));
			}
			else
			{
				//identically existing in this campaign
			}
		}
		else
		{
			parameterForEditing.parameterAbbreviation = _parameterAbbreviationBase + std::to_string(startIndex++);
			newSeries.AddParameter(std::move(parameterForEditing));
		}
	}

	_metadataCampaign.AddSeriesMetadata(std::move(newSeries));
	_seriesMetadataForStorage.push_back(&_metadataCampaign.getSeriesMetadata().back());
	_metadataCampaign.UpdateMetadataOverviewFromLastAddedSeries();

}

bool ResultCollectionExtender::CampaignMetadataWithSameNameExists(std::shared_ptr<MetadataEntry> otherMetadata)
{
	auto& allMetadata = _metadataCampaign.getMetaData();
	return allMetadata.find(otherMetadata->getEntryName()) != allMetadata.end();
}

bool ResultCollectionExtender::CampaignMetadataWithSameValueExists(std::shared_ptr<MetadataEntry> otherMetadata)
{
	auto& allMetadata = _metadataCampaign.getMetaData();
	auto metadataEntryByName = allMetadata.find(otherMetadata->getEntryName());
	if (metadataEntryByName == allMetadata.end())
	{
		return false;
	}
	else
	{
		std::shared_ptr<MetadataEntry> metadataEntry = metadataEntryByName->second;
		MetadataEntryComperator comperator;
		return comperator(metadataEntry, otherMetadata);
	}
}

void ResultCollectionExtender::AddCampaignMetadata(std::shared_ptr<MetadataEntry> metadata)
{
	_requiresUpdateMetadataCampaign = true;
	_metadataCampaign.AddMetaInformation(metadata->getEntryName(), metadata);
}

void ResultCollectionExtender::StoreCampaignChanges()
{
	if (_requiresUpdateMetadataCampaign || _seriesMetadataForStorage.size() != 0)
	{
			
	}
}

void ResultCollectionExtender::AddQuantityContainer(uint32_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, uint32_t quantityIndex,const  ot::Variable& quantityValue)
{
	if (_quantityContainer.size() == 0)
	{
		_quantityContainer.push_back(QuantityContainer(seriesIndex, parameterAbbreviations,std::move(parameterValues), quantityIndex));
	}
	QuantityContainer* quantityContainer = &_quantityContainer.back();
	const uint64_t storedValues = quantityContainer->GetValueArraySize();
	
	if (storedValues == _bucketSize)
	{
		_quantityContainer.push_back(QuantityContainer(seriesIndex, parameterAbbreviations, std::move(parameterValues), quantityIndex));
		quantityContainer = &_quantityContainer.back();
	}
	quantityContainer->AddValue(quantityValue);

	if (_quantityContainer.size() == _bufferSize)
	{
		FlushQuantityContainer();
	}
}

const uint32_t ResultCollectionExtender::FindNextFreeSeriesIndex()
{
	return _metadataCampaign.getSeriesMetadata().size();
}

const uint32_t ResultCollectionExtender::FindNextFreeQuantityIndex()
{
	auto& quantitiesByName = _metadataCampaign.getMetadataQuantitiesByName();
	return static_cast<uint32_t>(quantitiesByName.size());
}

const uint32_t ResultCollectionExtender::FindNextFreeParameterIndex()
{
	auto& parameterByName = _metadataCampaign.getMetadataParameterByName();
	return static_cast<uint32_t>(parameterByName.size());
}

void ResultCollectionExtender::FlushQuantityContainer()
{
	for (auto& qc : _quantityContainer)
	{
		DataStorageAPI::DataStorageResponse response = _dataStorageAccess.InsertDocumentToResultStorage(qc.getMongoDocument(), false, true);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	_dataStorageAccess.FlushQueuedData();
	_quantityContainer.clear();
}

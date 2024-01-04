#include "ResultCollectionExtender.h"
#include "MetadataEntryComperator.h"
#include <set>
#include <cassert>
#include <algorithm>
#include "MetadataEntityInterface.h"

ResultCollectionExtender::ResultCollectionExtender(const std::string& projectName, ot::components::ModelComponent& modelComponent, ClassFactory* classFactory)
	:ResultCollectionAccess(projectName,modelComponent,classFactory), _requiresUpdateMetadataCampaign(false)
{
	_quantityContainer.reserve(_bufferSize);
}

ResultCollectionExtender::~ResultCollectionExtender()
{
	if (_quantityContainer.size() != 0)
	{
		FlushQuantityContainer();
	}
	StoreCampaignChanges();
}

void ResultCollectionExtender::AddSeries(MetadataSeries&& series)
{
	uint64_t seriesIndex;
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

	for (auto& quantity : series.getQuantities())
	{
		const std::string newQuantityName = quantity.quantityName;
		const MetadataQuantity* addedQuantityInExistingParameter = FindMetadataQuantity(newQuantityName);
		if (addedQuantityInExistingParameter == nullptr)
		{
			auto& quantityForEditing = const_cast<MetadataQuantity&>(quantity);
			quantityForEditing.quantityIndex = FindNextFreeQuantityIndex();
			quantityForEditing.quantityAbbreviation = _quantityAbbreviationBase + std::to_string(quantity.quantityIndex);
			newSeries.AddQuantity(std::move(quantityForEditing));
		}
		//Assumes that the metadata and type of this quantity are the same as before. 
	}

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
			parameterForEditing.parameterAbbreviation = _parameterAbbreviationBase + std::to_string(FindNextFreeParameterIndex());
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
	MetadataEntityInterface entityCreator;
	if (_requiresUpdateMetadataCampaign && _seriesMetadataForStorage.size() != 0)
	{
		entityCreator.StoreCampaign(_modelComponent, _metadataCampaign, _seriesMetadataForStorage);
	}
	else if (_requiresUpdateMetadataCampaign)
	{
		entityCreator.StoreCampaign(_modelComponent, _metadataCampaign);
	}
	else if (_seriesMetadataForStorage.size() != 0)
	{
		entityCreator.StoreCampaign(_modelComponent, _seriesMetadataForStorage);
	}
}

void ResultCollectionExtender::AddQuantityContainer(uint64_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, uint64_t quantityIndex, const  ot::Variable& quantityValue)
{
	if (_quantityContainer.size() == 0)
	{
		QuantityContainer newContainer(seriesIndex, parameterAbbreviations, std::move(parameterValues), quantityIndex);
		newContainer.AddValue(quantityValue);
		_quantityContainer.push_back(std::move(newContainer));
	}
	else
	{
		QuantityContainer* lastAddedQuantityContainer = &_quantityContainer.back();
		const uint64_t lastAddedQuantityContainerStoredValues = lastAddedQuantityContainer->GetValueArraySize();
		if (lastAddedQuantityContainerStoredValues == _bucketSize)
		{
			if (_quantityContainer.size() == _bufferSize)
			{
				FlushQuantityContainer();
			}
			QuantityContainer newContainer(seriesIndex, parameterAbbreviations, std::move(parameterValues), quantityIndex);
			newContainer.AddValue(quantityValue);
			_quantityContainer.push_back(std::move(newContainer));
		}
		else
		{
			lastAddedQuantityContainer->AddValue(quantityValue);
		}
	}
}

const uint64_t ResultCollectionExtender::FindNextFreeSeriesIndex()
{
	return _modelComponent.createEntityUID();
}

const uint64_t ResultCollectionExtender::FindNextFreeQuantityIndex()
{
	return _modelComponent.createEntityUID();
}

const uint64_t ResultCollectionExtender::FindNextFreeParameterIndex()
{
	return _modelComponent.createEntityUID();
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

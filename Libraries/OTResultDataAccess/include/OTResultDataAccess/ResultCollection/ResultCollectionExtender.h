// @otlicense
// File: ResultCollectionExtender.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityCallbackBase.h"
#include "OTServiceFoundation/ApplicationBase.h"

#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "OTCore/MetadataHandle/MetadataQuantity.h"
#include "OTCore/MetadataHandle/MetadataParameter.h"
#include "OTResultDataAccess/QuantityContainer.h"
#include "OTResultDataAccess/ResultImportLogger/ResultImportLogger.h"
#include "OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"

// std header
#include <string>

class OT_RESULTDATAACCESS_API_EXPORT ResultCollectionExtender : public ResultCollectionMetadataAccess, public ot::EntityCallbackBase {
public:
	ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent);
	ResultCollectionExtender(ot::ApplicationBase* _applicationBase);
	ResultCollectionExtender(const ResultCollectionExtender& _other) = delete;
	ResultCollectionExtender& operator=(const ResultCollectionExtender& _other) = delete;
	~ResultCollectionExtender() = default;
	
	void setSaveModel(bool _saveModel) { m_saveModel = _saveModel; }

	ot::UID buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, const ot::JsonDocument& seriesMetadata = ot::JsonDocument{});
	
	//! @brief Throws exception if any parameter/quantity constellation is unvalid.
	void processDataPoints(DatasetDescription* dataDescription, uint64_t seriesMetadataIndex);
			
	void storeCampaignChanges();
	bool removeSeries(ot::UID _uid);

	ResultImportLogger& getLogger() { return m_logger; }
	size_t getMemSize();
protected:
	inline bool invariant() { return true; }

	ResultCollectionExtender() {};
	bool m_requiresUpdateMetadataCampaign = false;
	std::list<const MetadataSeries*> m_seriesMetadataForStorage;
	
	std::map<std::string,uint32_t> m_parameterBuckets;

	ResultImportLogger m_logger;

	//! @brief  Here we are checking for a free parameter label. Also we assign the parameter UID.
	//! @return List of parameter UIDs occuring in the dataset
	ot::UIDList addCampaignContextDataToParameters(DatasetDescription& _dataDescription);
	void addCampaignContextDataToQuantities(DatasetDescription& _dataDescription, ot::UIDList& _dependingParameter);
	ot::UID createNewSeries(std::list< DatasetDescription>& _dataDescription, const std::string& _seriesName, const ot::JsonDocument& _seriesMetadata);
	void addMetadataToSeries(std::list< DatasetDescription>& _dataDescription, MetadataSeries& _newSeries);

	std::map<std::string, MetadataQuantity*> m_quantitiesUpForStorageByLabel;
	//Buffer throughout all dataset descriptions during the creation of a single series with multiple datasetDescription. 
	// Important since the parameter by label overview is only updated by the campaign after the series was added.
	std::map<std::string,MetadataParameter*> m_parameterUpForStorageByLabel;

	virtual const uint64_t findNextFreeSeriesIndex();
	virtual const uint64_t findNextFreeQuantityIndex();
	virtual const uint64_t findNextFreeParameterIndex();

	bool quantityIsCorrectlySet(MetadataQuantity& _quantity);
	bool parameterIsCorrectlySet(MetadataParameter& _parameter);

	bool m_saveModel = true;
};			  



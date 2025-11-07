// @otlicense
// File: MetadataEntityInterface.h
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
#include "MetadataCampaign.h"
#include "EntityCallbackBase.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "MetadataEntry.h"

#include <stdint.h>

class __declspec(dllexport) MetadataEntityInterface : public ot::EntityCallbackBase
{
public:
	MetadataEntityInterface() = default;
	MetadataCampaign createCampaign(std::shared_ptr<EntityMetadataCampaign> _rmd, std::list<std::shared_ptr<EntityMetadataSeries>> _msmds);
	MetadataSeries createSeries(std::shared_ptr<EntityMetadataSeries> _seriesMetadataEntity);
	MetadataSeries createSeries(EntityMetadataSeries* _seriesMetadataEntity);
	void storeCampaign(ot::components::ModelComponent& _modelComponent, MetadataCampaign& _metaDataCampaign);
	void storeCampaign(ot::components::ModelComponent& _modelComponent, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel);
	void storeCampaign(ot::components::ModelComponent& _modelComponent,  MetadataCampaign& _metaDataCampaign, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel);

private:
	const std::string m_dataDimensionsField = "DataDimensions";
	const std::string m_dependingParameterField = "ParameterDependencies";
	const std::string m_valueDescriptionsField = "ValueDescriptions";

	const std::string m_nameField = "Name";
	const std::string m_labelField = "Label";
	const std::string m_unitField = "Unit";
	const std::string m_dataTypeNameField = "DataTypeName";
		
	const std::string m_valuesField = "Values";

	std::list<ot::UID> m_newEntityIDs;
	std::list<ot::UID> m_newEntityVersions;

	void extractCampaignMetadata(MetadataCampaign& _measurementCampaign, std::shared_ptr<EntityMetadataCampaign> _rmd);
	void extractSeriesMetadata(MetadataCampaign& _measurementCampaign, std::list<std::shared_ptr<EntityMetadataSeries>> _msmds);
	
	void insertMetadata(EntityWithDynamicFields* _entity, MetadataEntry* _metadata, const std::string _documentName = "");

	std::vector<std::string> convertToStringVector(const MetadataEntry* _metaData)const;
	std::vector<uint32_t> convertToUInt32Vector(const MetadataEntry* _metaData) const;
	std::vector<uint64_t> convertToUInt64Vector(const MetadataEntry* _metaData) const;
	std::string convertToString(const MetadataEntry* _metaData) const;
	std::list<ot::Variable> convertToVariableList(const MetadataEntry* _metaData)const;

	std::list<ot::Variable> convertFromStringVector(const std::vector<std::string> _values) const;
	std::list<ot::Variable> convertFromUInt32Vector(const std::vector<uint32_t> _values) const;
	std::list<ot::Variable> convertFromUInt64Vector(const std::vector<uint64_t> _values) const;
	

	std::list<std::shared_ptr<MetadataEntry>> extractMetadataObjects(const GenericDocument& _document);
	std::list<std::shared_ptr<MetadataEntry>> extractMetadataFields(const GenericDocument& document);
};


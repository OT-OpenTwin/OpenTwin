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

// OpenTwin header
#include "OTModelEntities/EntityCallbackBase.h"
#include "OTModelEntities/EntityMetadataSeries.h"
#include "OTModelEntities/EntityMetadataCampaign.h"
#include "OTCore/MetadataHandle/MetadataCampaign.h"

#include "OTModelEntities/ModelEntitiesAPIExport.h"
#include "OTModelEntities/NewModelStateInfo.h"
// std header
#include <stdint.h>

class OT_MODELENTITIES_API_EXPORT MetadataEntityInterface : public ot::EntityCallbackBase
{
public:
	MetadataEntityInterface() = default;
	MetadataCampaign createCampaign(EntityMetadataCampaign* _rmd, std::list<EntityMetadataSeries*>& _allSeries);
		
	ot::NewModelStateInfo storeCampaign(MetadataCampaign& _metaDataCampaign);
	ot::NewModelStateInfo storeCampaign(std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel);
	ot::NewModelStateInfo storeCampaign(MetadataCampaign& _metaDataCampaign, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel);

private:
	std::list<ot::Variable> convertFromStringVector(const std::vector<std::string> _values) const;
	std::list<ot::Variable> convertFromUInt32Vector(const std::vector<uint32_t> _values) const;
	std::list<ot::Variable> convertFromUInt64Vector(const std::vector<uint64_t> _values) const;
};


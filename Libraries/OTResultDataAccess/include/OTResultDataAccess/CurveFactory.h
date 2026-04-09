// @otlicense
// File: CurveFactory.h
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
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTModelEntities/EntityResult1DCurve.h"
#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
class OT_RESULTDATAACCESS_API_EXPORT CurveFactory
{
public:
	//! @brief Adds the query definitions to the curve config.
	//! @param _series The query is targeting a specific series.
	//! @param _quantity The query is targeting this quantity.
	//! @param _config The curve config that is adjusted
	//! @param _appBase Needed for getting the current collection name and the model component. The latter is needed to assemble the current campaign
	//! @param _access If the series that shall be used for this curve, is not yet added to the model state it is necessary to pass the used ResultCollectionMetadataExtender here as a parameter.
	static void addToConfig(const MetadataSeries& _series, const MetadataQuantity& _quantity, ot::Plot1DCurveCfg& _config, ot::ApplicationBase* _appBase, ResultCollectionMetadataAccess* _access = nullptr);
	//static void addToConfig(ot::Plot1DCurveCfg& _config, ot::ApplicationBase* _appBase, const std::string& _seriesLabel = "");
};

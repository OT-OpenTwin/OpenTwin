// @otlicense
// File: CrossCollectionAccess.h
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
#include <memory>
#include <list>

#include "OTCore/CoreTypes.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"
#include "OTServiceFoundation/ModelComponent.h"

//! @brief Interface to access a different project and get its metadata regarding the result collection.
class __declspec(dllexport) CrossCollectionAccess
{
public:
	CrossCollectionAccess(const std::string& collectionName, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	std::list<std::shared_ptr<EntityMetadataSeries>> getMeasurementMetadata(ot::components::ModelComponent& modelComponent);
	std::shared_ptr<EntityMetadataCampaign> getMeasurementCampaignMetadata(ot::components::ModelComponent& modelComponent);

private:
	std::string _collectionName;
	std::string _modelServiceURL;
	std::pair<ot::UIDList,ot::UIDList> InquireMetadataEntityIdentifier(const std::string& className);
};

class DataBaseWrapper
{
public:
	DataBaseWrapper(const std::string& collectionName);
	~DataBaseWrapper();
private:
	const std::string _oldCollectionName;
};

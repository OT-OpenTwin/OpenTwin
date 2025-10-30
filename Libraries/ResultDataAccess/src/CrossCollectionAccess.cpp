// @otlicense
// File: CrossCollectionAccess.cpp
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
#include "CrossCollectionAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "DataBase.h"
#include "OTCore/FolderNames.h"
#include "EntityAPI.h"
#include "OTCore/ReturnMessage.h"

#include "OTModelAPI/ModelServiceAPI.h"

CrossCollectionAccess::CrossCollectionAccess(const std::string& collectionName, const std::string& sessionServiceURL, const std::string& modelServiceURL)
	:_collectionName(collectionName), _modelServiceURL(modelServiceURL)
{}

std::list<std::shared_ptr<EntityMetadataSeries>> CrossCollectionAccess::getMeasurementMetadata(ot::components::ModelComponent& modelComponent)
{
	EntityMetadataSeries temp;
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	
	ot::UIDList& entityIDs = entityIdentifier.first;
	ot::UIDList& entityVersions = entityIdentifier.second;

	std::list<std::shared_ptr<EntityMetadataSeries>> measurementMetadata;
	auto entityVersion = entityVersions.begin();
	DataBaseWrapper wrapper(_collectionName);	
	
	for(auto entityID= entityIDs.begin(); entityID != entityIDs.end(); entityID++)
	{
		auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(*entityID, *entityVersion);
		const std::shared_ptr<EntityMetadataSeries> metadata(dynamic_cast<EntityMetadataSeries*>(baseEnt));
		assert(metadata != nullptr);
		measurementMetadata.push_back(metadata);
		
		entityVersion++;
	}
	return measurementMetadata;
}

std::shared_ptr<EntityMetadataCampaign>  CrossCollectionAccess::getMeasurementCampaignMetadata(ot::components::ModelComponent& modelComponent)
{
	EntityMetadataCampaign temp;
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	if (entityIdentifier.first.size() == 1)
	{
		DataBaseWrapper wrapper(_collectionName);
		ot::UID& entityID = *entityIdentifier.first.begin();
		ot::UID& entityVersion = *entityIdentifier.second.begin();

		auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion);
		const std::shared_ptr<EntityMetadataCampaign> metadata(dynamic_cast<EntityMetadataCampaign*>(baseEnt));
		assert(metadata != nullptr);
		return metadata;
	}
	else
	{
		return std::shared_ptr<EntityMetadataCampaign>(nullptr);
	}
}

std::pair<ot::UIDList, ot::UIDList> CrossCollectionAccess::InquireMetadataEntityIdentifier(const std::string& className)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(_collectionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(ot::FolderNames::DatasetFolder, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Type, ot::JsonString(className, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Recursive, true, doc.GetAllocator());
		
	std::string response;
	if (!ot::msg::send("", _modelServiceURL, ot::EXECUTE, doc.toJson(), response))
	{
		throw std::exception("Timeout for requesting the model service.");
	}

	if (response == "")
	{
		return std::pair<ot::UIDList, ot::UIDList>();
	}
	
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	
	ot::UIDList entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	return std::make_pair<>(entityIDs, entityVersions);
}

DataBaseWrapper::DataBaseWrapper(const std::string& collectionName)
	:_oldCollectionName(DataBase::instance().getCollectionName())
{
	DataBase::instance().setCollectionName(collectionName);
}

DataBaseWrapper::~DataBaseWrapper()
{
	DataBase::instance().setCollectionName(_oldCollectionName);
}

// @otlicense
// File: MeasurementCampaignHandler.cpp
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

#include "MeasurementCampaignHandler.h"
#include "Application.h"
#include "OTCore/FolderNames.h"
#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "EntityAPI.h"

void MeasurementCampaignHandler::ConnectToCollection(const std::string& collectionName, const std::string& projectName)
{
	std::list<ot::EntityInformation> allMeasurementMetadata = getMSMDEntityInformation(collectionName, projectName);

	if(allMeasurementMetadata.size() > 0)
	{

		std::string oldProjectName = DataBase::instance().getCollectionName();
		DataBase::instance().setCollectionName(collectionName);
		Application::instance()->prefetchDocumentsFromStorage(allMeasurementMetadata);

		std::vector<std::shared_ptr<EntityMetadataSeries>> measurementMetadata;
		measurementMetadata.reserve(allMeasurementMetadata.size());

		for (auto& entityInfo : allMeasurementMetadata)
		{
			auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
			auto metadata = dynamic_cast<EntityMetadataSeries*>(baseEnt);
			assert(metadata != nullptr);
			measurementMetadata.push_back(std::shared_ptr<EntityMetadataSeries>(metadata));
		}

		DataBase::instance().setCollectionName(oldProjectName);

		CollectMetaInformation(measurementMetadata);
	}
	else
	{
		_uiComponent->displayMessage("No measurement metadata detected in project: " + projectName);
	}
}

std::list<ot::EntityInformation> MeasurementCampaignHandler::getMSMDEntityInformation(const std::string& collectionName, const std::string& projectName)
{
	EntityMetadataSeries temp;

	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(ot::FolderNames::DatasetFolder, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Type, ot::JsonString(temp.getClassName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Recursive, true, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(false, OT_INFO_SERVICE_TYPE_MODEL, doc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	ot::UIDList entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);

	auto version = entityVersions.begin();
	std::list<ot::EntityInformation> entityInfos;
	for (ot::UID id : entityIDs)
	{
		ot::EntityInformation entityInfo;
		entityInfo.setEntityID(id);
		entityInfo.setEntityVersion(*version);
		version++;
	}

	return entityInfos;
}

void MeasurementCampaignHandler::CollectMetaInformation(std::vector<std::shared_ptr<EntityMetadataSeries>>& measurementMetadata)
{
	for (auto& metaData : measurementMetadata)
	{
		metaData->getAllQuantityDocumentNames();
	}
}

std::vector<std::string> MeasurementCampaignHandler::GetParameterList()
{
	

	return std::vector<std::string>();
}

std::vector<std::string> MeasurementCampaignHandler::GetQuantityList()
{
	return std::vector<std::string>();
}


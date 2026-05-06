// @otlicense
// File: EntityMetadataCampaign.cpp
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

#include "OTModelEntities/EntityMetadataCampaign.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityMetadataCampaign> registrar("EntityMetadataCampaign");


void EntityMetadataCampaign::addVisualizationNodes()
{

	if (!getName().empty())
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
	}

	EntityBase::addVisualizationNodes();
}


void EntityMetadataCampaign::addStorageData(bsoncxx::builder::basic::document& _storage) 
{
	EntityBase::addStorageData(_storage);
	try
	{
		const std::string metadata = ot::json::toJson(m_campaign.getMetadata());
		bsoncxx::document::value doc = bsoncxx::from_json(metadata);
		_storage.append(bsoncxx::builder::basic::kvp("Metadata", doc));
	}
	catch (std::exception& _e)
	{
		const std::string message = "Failed to store metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
		OT_LOG_E(message);
	}
}
void EntityMetadataCampaign::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) 
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	if (doc_view.find("Metadata") != doc_view.end())
	{
		try
		{
			auto metadataView = doc_view["Metadata"];
			const std::string metadataString = bsoncxx::to_json(metadataView.get_document());
			ot::JsonDocument metadata;
			metadata.fromJson(metadataString);
			m_campaign.setMetadata(metadata);
		}
		catch (std::exception& _e)
		{
			const std::string message = "Failed to load metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
			OT_LOG_E(message);
		}
	}

}


EntityMetadataCampaign::EntityMetadataCampaign(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms)
	: EntityBase(ID, parent, mdl, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/RMD");
	treeItem.setHiddenIcon("Default/RMD");
	this->setDefaultTreeItem(treeItem);
}

bool EntityMetadataCampaign::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

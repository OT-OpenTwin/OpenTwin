// @otlicense
// File: CoordinateSystemManager.cpp
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

#include "CoordinateSystemManager.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCommunication/ActionTypes.h"

#include "OTModelEntities/EntityCoordinateSystem.h"

void CoordinateSystemManager::createNew()
{
	std::string itemName = createUniqueName("Coordinate Systems/LCS");

	EntityCoordinateSystem* csEntity = new EntityCoordinateSystem(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	csEntity->setName(itemName);
	csEntity->createProperties();
	csEntity->setTreeItemEditable(true);
	csEntity->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		serviceName
	);

	csEntity->storeToDataBase();

	std::list<ot::UID> topologyEntityIDList = { csEntity->getEntityID()};
	std::list<ot::UID> topologyEntityVersionList = { csEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList = { };
	std::list<ot::UID> dataEntityVersionList = { };
	std::list<ot::UID> dataEntityParentList = { };

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create new local coordinate system: " + itemName);
}

void CoordinateSystemManager::activateCoordinateSystem(const std::string &csName)
{
	activeCoordinateSystemName = csName;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_ActivateCS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, ot::ModelServiceAPI::getCurrentVisualizationModelID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_ObjectName, ot::JsonString(csName, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	uiComponent->sendMessage(true, doc, tmp);
}

std::string CoordinateSystemManager::createUniqueName(const std::string& name)
{
	// Find the parent folder name of the entity
	std::string parentFolder;
	size_t index = name.rfind('/');

	if (index != std::string::npos)
	{
		parentFolder = name.substr(0, index);
	}

	// Get the list of items in the parent folder
	std::list<std::string> folderItems = ot::ModelServiceAPI::getListOfFolderItems(parentFolder);

	// Now make sure that the new name of the item is unique
	std::string itemName = name;
	if (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end())
	{
		int count = 1;
		do
		{
			itemName = name + "_" + std::to_string(count);
			count++;
		} while (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end());
	}

	return itemName;
}

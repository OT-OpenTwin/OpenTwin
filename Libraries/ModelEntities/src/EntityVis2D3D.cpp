// @otlicense
// File: EntityVis2D3D.cpp
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


#include "EntityVis2D3D.h"
#include "OldTreeIcon.h"
#include "Database.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityVis2D3D> registrar("EntityVis2D3D");

EntityVis2D3D::EntityVis2D3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityContainer(ID, parent, obs, ms),
	resultType(EntityResultBase::UNKNOWN),
	sourceID(0),
	sourceVersion(0),
	meshID(0),
	meshVersion(0),
	visualizationDataID(0),
	visualizationDataVersion(0)
	//source(nullptr)
{
}

EntityVis2D3D::~EntityVis2D3D()
{
	//if (source != nullptr)
	//{
	//	delete source;
	//	source = nullptr;
	//}
}

void EntityVis2D3D::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::addStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("visualizationDataID", (long long) visualizationDataID),
		bsoncxx::builder::basic::kvp("visualizationDataVersion", (long long) visualizationDataVersion),
		bsoncxx::builder::basic::kvp("sourceID", (long long) sourceID),
		bsoncxx::builder::basic::kvp("sourceVersion", (long long) sourceVersion),
		bsoncxx::builder::basic::kvp("meshID", (long long) meshID),
		bsoncxx::builder::basic::kvp("meshVersion", (long long) meshVersion)
	);
}

void EntityVis2D3D::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	try {
		visualizationDataID      = doc_view["visualizationDataID"].get_int64();
		visualizationDataVersion = doc_view["visualizationDataVersion"].get_int64();
	}
	catch (std::exception)
	{
		visualizationDataID      = 0;
		visualizationDataVersion = 0;
	}

	try {
		sourceID      = doc_view["sourceID"].get_int64();
		sourceVersion = doc_view["sourceVersion"].get_int64();
	}
	catch (std::exception)
	{
		sourceID      = 0;
		sourceVersion = 0;
	}

	try {
		meshID      = doc_view["meshID"].get_int64();
		meshVersion = doc_view["meshVersion"].get_int64();
	}
	catch (std::exception)
	{
		meshID      = 0;
		meshVersion = 0;
	}

	resetModified();
}

void EntityVis2D3D::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Vis2DVisible";
		treeIcons.hiddenIcon = "Vis2DHidden";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_DataID, visualizationDataID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_DataVersion, visualizationDataVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_SourceID, sourceID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_SourceVersion, sourceVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_MeshID, meshID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_MeshVersion, meshVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, this->getInitiallyHidden(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityVis2D3D::createProperties(void)
{

}

bool EntityVis2D3D::updatePropertyVisibilities(void)
{
	return false;
}

bool EntityVis2D3D::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}

//void EntityVis2D3D::ensureSourceDataLoaded(void)
//{
//	if (source == nullptr)
//	{
//		if (sourceID != 0 && sourceVersion != 0)
//		{
//			assert(sourceID != 0);
//			assert(sourceVersion != 0);
//
//			std::map<ot::UID, EntityBase *> entityMap;
//			source = dynamic_cast<EntityResultBase *>(readEntityFromEntityIDAndVersion(this, sourceID, sourceVersion, entityMap));
//
//			if (source != nullptr)
//			{
//				assert(source->getResultType() == getResultType());
//			}
//		}
//	}
//}

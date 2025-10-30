// @otlicense
// File: EntityAnnotation.cpp
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

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityAnnotation.h"
#include "EntityAnnotationData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <OTCommunication/ActionTypes.h>
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityAnnotation> registrar(EntityAnnotation::className());

EntityAnnotation::EntityAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner),
	annotationData(nullptr),
	annotationDataStorageId(-1)
{
	
}

EntityAnnotation::~EntityAnnotation()
{
	if (annotationData != nullptr)
	{
		delete annotationData;
		annotationData = nullptr;
	}
}

bool EntityAnnotation::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityAnnotation::addPoint(double x, double y, double z, double r, double g, double b)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	annotationData->addPoint(x, y, z, r, g, b);
	setModified();
}

void EntityAnnotation::addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	annotationData->addTriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, r, g, b);
	setModified();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getPoints(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getPoints();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getPointsRGB(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getPointsRGB();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getTriangleP1(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getTriangleP1();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getTriangleP2(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getTriangleP2();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getTriangleP3(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getTriangleP3();
}

const std::vector<std::array<double, 3>> &EntityAnnotation::getTriangleRGB(void)
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getTriangleRGB();
}

const double* EntityAnnotation::getEdgeColorRGB()
{
	ensureAnnotationDataIsLoaded();
	assert(annotationData != nullptr);

	return annotationData->getEdgeColorRGB();
}

void EntityAnnotation::ensureAnnotationDataIsLoaded(void)
{
	if (annotationData == nullptr)
	{
		if (annotationDataStorageId == -1)
		{
			annotationData = new EntityAnnotationData(createEntityUID(), this, getObserver(), getModelState(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			annotationData = dynamic_cast<EntityAnnotationData *>(readEntityFromEntityID(this, annotationDataStorageId, entityMap));
		}
	}
}

void EntityAnnotation::storeAnnotationData(void)
{
	assert(annotationData != nullptr);

	annotationData->storeToDataBase();
	annotationDataStorageId = annotationData->getEntityID();
}

void EntityAnnotation::releaseAnnotationData(void)
{
	if (annotationData == nullptr) return;

	storeAnnotationData();

	delete annotationData;
	annotationData = nullptr;
}

void EntityAnnotation::storeToDataBase(void)
{
	// If the pointers to brep or facets are 0, then the objects are stored in the data storage and the storage IDs are up to date

	if (annotationData != nullptr)
	{
		storeAnnotationData();
	}

	// Afterward, we store the container itself
	EntityBase::storeToDataBase();
}

void EntityAnnotation::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(bsoncxx::builder::basic::kvp("DataID", annotationDataStorageId));
}


void EntityAnnotation::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates

	if (annotationData != nullptr)
	{
		delete annotationData;
		annotationData = nullptr;
	}

	// Now load the data from the storage

	annotationDataStorageId = doc_view["DataID"].get_int64();

	resetModified();
}

void EntityAnnotation::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityAnnotation::addVisualizationItem(bool isHidden)
{
	if (annotationDataStorageId == -1)
	{
		storeToDataBase();
		assert(annotationDataStorageId != -1);
	}

	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "WarningVisible";
	treeIcons.hiddenIcon = "WarningHidden";

	ot::UID storageVersion = getCurrentEntityVersion(annotationDataStorageId);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNodeFromDatabase, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_UID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, (unsigned long long) annotationDataStorageId, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, storageVersion, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(annotationDataStorageId, storageVersion));

	getObserver()->sendMessageToViewer(doc, prefetchIds);
}

void EntityAnnotation::removeChild(EntityBase *child)
{
	if (child == annotationData)
	{
		annotationData = nullptr;
	}
	else
	{
		EntityBase::removeChild(child);
	}
}


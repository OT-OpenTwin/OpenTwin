// @otlicense
// File: EntityMeshCartesianItem.cpp
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


#include "EntityMeshCartesianItem.h"
#include "EntityMeshCartesian.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshCartesianItem> registrar("EntityMeshCartesianItem");

EntityMeshCartesianItem::EntityMeshCartesianItem(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	mesh(nullptr),
	colorRGB{0.0, 0.0, 0.0}
{
}

EntityMeshCartesianItem::~EntityMeshCartesianItem()
{
}

bool EntityMeshCartesianItem::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	if (boundingBox.getEmpty()) return false;

	xmin = boundingBox.getXmin();
	xmax = boundingBox.getXmax();
	ymin = boundingBox.getYmin();
	ymax = boundingBox.getYmax();
	zmin = boundingBox.getZmin();
	zmax = boundingBox.getZmax();

	return true;
}

void EntityMeshCartesianItem::storeToDataBase(void)
{
	// Afterward, we store the container itself
	EntityBase::storeToDataBase();
}

void EntityMeshCartesianItem::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the specific entity data now
	storage.append(bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON()));
	storage.append(bsoncxx::builder::basic::kvp("ColorR", colorRGB[0]));
	storage.append(bsoncxx::builder::basic::kvp("ColorG", colorRGB[1]));
	storage.append(bsoncxx::builder::basic::kvp("ColorB", colorRGB[2]));

	auto f = bsoncxx::builder::basic::array();

	for (auto face : faces)
	{
		f.append((int)face);
	}

	storage.append(bsoncxx::builder::basic::kvp("Faces", f));
}

void EntityMeshCartesianItem::setMaterial(const std::string& materialName)
{
	EntityPropertiesString* material = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Material"));
	if (material == nullptr)
	{
		EntityPropertiesString::createProperty("Solver", "Material", materialName, "", getProperties());
		getProperties().getProperty("Material")->setReadOnly(true);
	}
	else
	{
		material->setValue(materialName);
	}
}

std::string EntityMeshCartesianItem::getMaterial()
{
	EntityPropertiesString* material = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Material"));
	if (material == nullptr) return "";

	return material->getValue();
}

void EntityMeshCartesianItem::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	auto bbox = doc_view["BoundingBox"].get_document();
	boundingBox.setFromBSON(bbox);

	colorRGB[0] = doc_view["ColorR"].get_double();
	colorRGB[1] = doc_view["ColorG"].get_double();
	colorRGB[2] = doc_view["ColorB"].get_double();

	try
	{
		auto face = doc_view["Faces"].get_array().value;

		size_t numberFaces = std::distance(face.begin(), face.end());

		auto f = face.begin();

		setNumberFaces(numberFaces);

		for (unsigned long findex = 0; findex < numberFaces; findex++)
		{
			setFace(findex, (int)DataBase::getIntFromArrayViewIterator(f));
			f++;
		}
	}
	catch (std::exception)
	{
		assert(0);
	}

	resetModified();
}

void EntityMeshCartesianItem::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityMeshCartesianItem::addVisualizationItem(bool isHidden)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "MeshItemVisible";
	treeIcons.hiddenIcon = "MeshItemHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_UID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_FacesList, ot::JsonArray(faces, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_ITEM_COLOR_R, colorRGB[0], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_ITEM_COLOR_G, colorRGB[1], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_ITEM_COLOR_B, colorRGB[2], doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	getObserver()->sendMessageToViewer(doc, prefetchIds);
}


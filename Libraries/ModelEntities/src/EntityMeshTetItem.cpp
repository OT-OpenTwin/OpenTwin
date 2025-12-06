// @otlicense
// File: EntityMeshTetItem.cpp
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


#include "DataBase.h"
#include "EntityMeshTet.h"
#include "EntityMeshTetItem.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetItemDataTets.h"
#include "EntityMeshTetItemDataTetedges.h"

#include <bsoncxx/builder/basic/array.hpp>

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityMeshTetItem> registrar("EntityMeshTetItem");

EntityMeshTetItem::EntityMeshTetItem(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityBase(ID, parent, obs, ms),
	mesh(nullptr),
	meshDataTets(nullptr),
	meshDataTetsStorageId(0),
	meshDataTetEdges(nullptr),
	meshDataTetEdgesStorageId(0),
	colorRGB{0.0, 0.0, 0.0},
	numberOfTriangles(0),
	numberOfTets(0),
	numberOfTetEdges(0)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Default/MeshItemVisible");
	treeItem.setHiddenIcon("Default/MeshItemHidden");
	this->setTreeItem(treeItem, true);
}

EntityMeshTetItem::~EntityMeshTetItem()
{
	if (meshDataTets != nullptr)
	{
		delete meshDataTets;
		meshDataTets = nullptr;
	}

	if (meshDataTetEdges != nullptr)
	{
		delete meshDataTetEdges;
		meshDataTetEdges = nullptr;
	}
}

EntityMeshTetFace *EntityMeshTetItem::getFace(size_t nF)
{
	assert(mesh != nullptr);
	return mesh->getFace(abs(getFaceId(nF)));
}

bool EntityMeshTetItem::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
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

void EntityMeshTetItem::updateStatistics(void)
{
	EnsureMeshItemDataTetsLoaded();
	assert(meshDataTets != nullptr);

	// Update the bounding Box
	boundingBox.reset();

	for (size_t nF = 0; nF < getNumberFaces(); nF++)
	{
		boundingBox.extend(getFace(nF)->getBoundingBox());
	}

	// Update the number of triangles
	numberOfTriangles = 0;
	for (size_t nF = 0; nF < getNumberFaces(); nF++)
	{
		numberOfTriangles += getFace(nF)->getNumberTriangles();
	}

	// Update the number of tets
	numberOfTets = meshDataTets->getNumberTets();

	// Update the number of tet edges
	if (meshDataTetEdges != nullptr)
	{
		numberOfTetEdges = meshDataTetEdges->getNumberEdges();
	}
	else
	{
		numberOfTetEdges = 0;
	}

	setModified();
}

void EntityMeshTetItem::setNumberTets(size_t nT)
{
	EnsureMeshItemDataTetsLoaded();
	assert(meshDataTets != nullptr);

	meshDataTets->setNumberTets(nT);
	setModified();
}

void EntityMeshTetItem::setNumberOfTetNodes(size_t nT, int nNodes)
{
	EnsureMeshItemDataTetsLoaded();
	assert(meshDataTets != nullptr);

	meshDataTets->setNumberOfTetNodes(nT, nNodes);
	setModified();
}

void EntityMeshTetItem::setTetNode(size_t nT, int nodeIndex, size_t node)
{
	EnsureMeshItemDataTetsLoaded();
	assert(meshDataTets != nullptr);

	meshDataTets->setTetNode(nT, nodeIndex, node);
	setModified();
}

size_t EntityMeshTetItem::getTetNode(size_t nT, int nodeIndex)
{
	EnsureMeshItemDataTetsLoaded();
	assert(meshDataTets != nullptr);

	return meshDataTets->getTetNode(nT, nodeIndex);
}

void EntityMeshTetItem::EnsureMeshItemDataTetsLoaded(void)
{
	if (meshDataTets == nullptr)
	{
		if (meshDataTetsStorageId == 0)
		{
			meshDataTets = new EntityMeshTetItemDataTets(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshDataTets = dynamic_cast<EntityMeshTetItemDataTets *>(readEntityFromEntityID(this, meshDataTetsStorageId, entityMap));
		}
	}
}

void EntityMeshTetItem::setNumberTetEdges(size_t nE)
{
	EnsureMeshItemDataTetEdgesLoaded();
	assert(meshDataTetEdges != nullptr);

	meshDataTetEdges->setNumberEdges(nE);
	setModified();
}

void EntityMeshTetItem::setTetEdgeNodes(size_t nE, size_t n[2])
{
	EnsureMeshItemDataTetEdgesLoaded();
	assert(meshDataTetEdges != nullptr);

	meshDataTetEdges->setEdgeNodes(nE, n);
	setModified();
}

void EntityMeshTetItem::getTetEdgeNodes(size_t nE, size_t n[2])
{
	EnsureMeshItemDataTetEdgesLoaded();
	assert(meshDataTetEdges != nullptr);

	meshDataTetEdges->getEdgeNodes(nE, n);
}

void EntityMeshTetItem::EnsureMeshItemDataTetEdgesLoaded(void)
{
	if (meshDataTetEdges == nullptr)
	{
		if (meshDataTetEdgesStorageId == 0)
		{
			meshDataTetEdges = new EntityMeshTetItemDataTetedges(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshDataTetEdges = dynamic_cast<EntityMeshTetItemDataTetedges *>(readEntityFromEntityID(this, meshDataTetEdgesStorageId, entityMap));
		}
	}
}

void EntityMeshTetItem::storeMeshData(void)
{
	if (meshDataTets != nullptr)
	{
		// Assign an entity id if needed
		if (meshDataTets->getEntityID() == 0)
		{
			meshDataTets->setEntityID(createEntityUID());
		}
		meshDataTets->storeToDataBase();
		meshDataTetsStorageId = meshDataTets->getEntityID();
	}

	if (meshDataTetEdges != nullptr)
	{
		if (meshDataTetEdges->getEntityID() == 0)
		{
			meshDataTetEdges->setEntityID(createEntityUID());
		}
		meshDataTetEdges->storeToDataBase();
		meshDataTetEdgesStorageId = meshDataTetEdges->getEntityID();
	}
}

void EntityMeshTetItem::releaseMeshData(void)
{
	storeMeshData();	

	if (meshDataTets != nullptr)
	{
		delete meshDataTets;
		meshDataTets = nullptr;
	}

	if (meshDataTetEdges != nullptr)
	{
		delete meshDataTetEdges;
		meshDataTetEdges = nullptr;
	}
}

void EntityMeshTetItem::storeToDataBase(void)
{
	// If the pointers to brep or facets are 0, then the objects are stored in the data storage and the storage IDs are up to date
	storeMeshData();

	// Afterward, we store the container itself
	EntityBase::storeToDataBase();
}

void EntityMeshTetItem::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	long long meshDataTetsStorageVersion     = 0;
	long long meshDataTetEdgesStorageVersion = 0;

	if (meshDataTetsStorageId != 0)
	{
		meshDataTetsStorageVersion = (meshDataTets != nullptr) ? meshDataTets->getEntityStorageVersion() : getCurrentEntityVersion(meshDataTetsStorageId);
	}

	if (meshDataTetEdgesStorageId != 0)
	{
		meshDataTetEdgesStorageVersion = (meshDataTetEdges != nullptr) ? meshDataTetEdges->getEntityStorageVersion() : getCurrentEntityVersion(meshDataTetEdgesStorageId);
	}

	// Add the specific entity data now
	storage.append(bsoncxx::builder::basic::kvp("MeshDataTetsID", (long long) meshDataTetsStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MeshDataTetsVersion", meshDataTetsStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MeshDataTetEdgesID", (long long) meshDataTetEdgesStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MeshDataTetEdgesVersion", (long long) meshDataTetEdgesStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON()));
	storage.append(bsoncxx::builder::basic::kvp("NumberTriangles", (long long)numberOfTriangles));
	storage.append(bsoncxx::builder::basic::kvp("NumberTets", (long long)numberOfTets));
	storage.append(bsoncxx::builder::basic::kvp("NumberTetEdges", (long long)numberOfTetEdges));
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

void EntityMeshTetItem::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates

	if (meshDataTets != nullptr)
	{
		delete meshDataTets;
		meshDataTets = nullptr;
	}

	// Now load the data from the storage

	meshDataTetsStorageId = DataBase::getIntFromView(doc_view, "MeshDataTetsID", 0);
	meshDataTetEdgesStorageId = DataBase::getIntFromView(doc_view, "MeshDataTetEdgesID", 0);

	numberOfTriangles = (size_t) DataBase::getIntFromView(doc_view, "NumberTriangles", 0);
	numberOfTets      = (size_t) DataBase::getIntFromView(doc_view, "NumberTets", 0);
	numberOfTetEdges  = (size_t) DataBase::getIntFromView(doc_view, "NumberTetEdges", 0);

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

void EntityMeshTetItem::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityMeshTetItem::addVisualizationItem(bool isHidden)
{
	

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshItemFromFacetDatabase, doc.GetAllocator()), doc.GetAllocator());
	
	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, this->getCurrentEntityVersion(this->getEntityID()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_TETEDGES_ID, meshDataTetEdgesStorageId, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_TETEDGES_Version, this->getCurrentEntityVersion(meshDataTetEdgesStorageId), doc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(getEntityID(), getCurrentEntityVersion(getEntityID())));

	getObserver()->sendMessageToViewer(doc, prefetchIds);
}

void EntityMeshTetItem::removeChild(EntityBase *child)
{
	if (child == meshDataTets)
	{
		meshDataTets = nullptr;
	}
	else if (child == meshDataTetEdges)
	{
		meshDataTetEdges = nullptr;
	}
	else
	{
		EntityBase::removeChild(child);
	}
}

bool EntityMeshTetItem::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Check the color
	EntityPropertiesColor* color = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));

	// We do not necessarily need to have a color property
	if (color != nullptr)
	{
		if (color->needsUpdate())
		{
			std::list<double> colorRGB;
			colorRGB.push_back(color->getColorR());
			colorRGB.push_back(color->getColorG());
			colorRGB.push_back(color->getColorB());

			setColor(color->getColorR(), color->getColorG(), color->getColorB());

			assert(getObserver() != nullptr);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateMeshColor, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, ot::JsonArray(colorRGB, doc.GetAllocator()), doc.GetAllocator());

			getObserver()->sendMessageToViewer(doc);

			// We have processed the color change -> reset the needs update flag for this property
			color->resetNeedsUpdate();
		}
	}

	return false;  // We do not need to update the property grid
}

// @otlicense
// File: EntityFaceAnnotation.cpp
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


#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"
#include "GeometryOperations.h"
#include "EntityFacetData.h"
#include "EntityBrep.h"
#include "DataBase.h"

#include "OTCommunication/ActionTypes.h"

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "BRepBndLib.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"

static EntityFactoryRegistrar<EntityFaceAnnotation> registrar(EntityFaceAnnotation::className());

EntityFaceAnnotation::EntityFaceAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	facetsStorageID(-1)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/FaceAnnotationVisible");
	treeItem.setHiddenIcon("Default/FaceAnnotationHidden");
	this->setDefaultTreeItem(treeItem);
	
	EntityPropertiesColor::createProperty(  "General", "Color",           {0, 0, 0}, "", getProperties());
	EntityPropertiesInteger::createProperty("General", "Number of faces",         0, "", getProperties())->setReadOnly(true);

	facets = nullptr;
}

EntityFaceAnnotation::~EntityFaceAnnotation()
{
	if (facets != nullptr)
	{
		delete facets;
		facets = nullptr;
	}
}

void EntityFaceAnnotation::addFacePick(EntityFaceAnnotationData annotation)
{
	// Get and increase the face count
	EntityPropertiesInteger *faceCount = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of faces"));
	assert(faceCount != nullptr);

	int faceIndex = faceCount->getValue() + 1;
	faceCount->setValue(faceIndex);

	std::string index = "(" + std::to_string(faceIndex) + ")";

	faceCount->resetNeedsUpdate();

	// Now store the new face
	EntityPropertiesString* reference = new EntityPropertiesString("Reference entity " + index, annotation.getEntityName());
	EntityPropertiesString* faceName = new EntityPropertiesString("Face name " + index, annotation.getFaceName());

	getProperties().createProperty(reference, "Location " + std::to_string(faceIndex));
	getProperties().createProperty(faceName, "Location " + std::to_string(faceIndex));

	reference->resetNeedsUpdate();
	faceName->resetNeedsUpdate();

	setModified();
}

void EntityFaceAnnotation::setColor(double r, double g, double b)
{ 
	EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(color != nullptr);

	color->setColorR(r);
	color->setColorG(g);
	color->setColorB(b);

	color->resetNeedsUpdate();

	setModified();
}

void EntityFaceAnnotation::getGeometryNames(std::list <std::string> &names)
{
	names.clear();

	EntityPropertiesInteger *faceCount = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of faces"));
	assert(faceCount != nullptr);

	std::map<std::string, bool> nameMap;

	for (int faceIndex = 1; faceIndex <= faceCount->getValue(); faceIndex++)
	{
		std::string index = "(" + std::to_string(faceIndex) + ")";

		// Now create a new visualization node (it may contain no nodes / triangles / edges if anything is invalid)

		EntityPropertiesString *geometryName = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Reference entity " + index));
		assert(geometryName != nullptr);

		nameMap[geometryName->getValue()] = true;
	}

	for (auto name : nameMap)
	{
		names.push_back(name.first);
	}
}

bool EntityFaceAnnotation::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	if (boundingBox.getEmpty()) return false;

	xmin = boundingBox.getXmin();
	ymin = boundingBox.getYmin();
	zmin = boundingBox.getZmin();
	xmax = boundingBox.getXmax();
	ymax = boundingBox.getYmax();
	zmax = boundingBox.getZmax();

	return true;
}

void EntityFaceAnnotation::ensureFacetsAreLoaded(void)
{
	if (facets != nullptr) return;

	assert(facetsStorageID != -1);

	// The facets are a hidden object (a child of the current object). Therefore it does not need to be present in the global maps
	std::map<ot::UID, EntityBase *> entityMap;
	facets = dynamic_cast<EntityFacetData *>(readEntityFromEntityID(this, facetsStorageID, entityMap));
}

void EntityFaceAnnotation::updateVisualization(bool isHidden)
{
	EntityPropertiesColor* color = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(color != nullptr);

	std::vector<double> colorRGB;
	colorRGB.push_back(color->getColorR());
	colorRGB.push_back(color->getColorG());
	colorRGB.push_back(color->getColorB());

	std::string errors;

	// Note: this call will modify a potentially existing node rather than creating a new one
	ot::UID storageVersion = getCurrentEntityVersion(facetsStorageID);

	std::string materialType = "Rough";

	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase, doc.GetAllocator()), doc.GetAllocator());
	
	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, ot::JsonArray(colorRGB, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, ot::JsonArray(colorRGB, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_MaterialType, ot::JsonString(materialType, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureType, ot::JsonString("None", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureReflective, false, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling, false, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_OffsetFactor, 0.5, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, (unsigned long long) facetsStorageID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, storageVersion, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageParentVis, this->getManageParentVisibility(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageChildVis, this->getManageChildVisibility(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected, false, doc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(facetsStorageID, storageVersion));

	getObserver()->sendMessageToViewer(doc, prefetchIds);
}

void EntityFaceAnnotation::addVisualizationNodes(void)
{
	updateVisualization(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

std::list<TopoDS_Shape> EntityFaceAnnotation::findFacesFromShape(EntityBrep *brep)
{
	std::list<TopoDS_Shape> facesList;

	EntityPropertiesInteger *faceCount = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of faces"));
	assert(faceCount != nullptr);

	for (int faceIndex = 1; faceIndex <= faceCount->getValue(); faceIndex++)
	{
		findFacesAtIndexFromShape(facesList, faceIndex, brep);
	}

	return facesList;
}

void EntityFaceAnnotation::findFacesAtIndexFromShape(std::list<TopoDS_Shape> &facesList, int faceIndex, EntityBrep* brep)
{
	std::string index = "(" + std::to_string(faceIndex) + ")";

	EntityPropertiesString* faceName = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Face name " + index));

	TopExp_Explorer exp;
	for (exp.Init(brep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		if (brep->getFaceName(aFace) == faceName->getValue())
		{
			facesList.push_back(exp.Current());
			return;
		}
	}
}

bool EntityFaceAnnotation::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	getProperties().forceResetUpdateForAllProperties();

	// The geometry will be updated as special entity update in the modeler.

	return false; // No property grid update necessary
}

void EntityFaceAnnotation::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(
		bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON()),
		bsoncxx::builder::basic::kvp("Facets",      facetsStorageID)
	);
}

void EntityFaceAnnotation::storeToDataBase(void)
{
	// If the pointers to brep or facets are 0, then the objects are stored in the data storage and the storage IDs are up to date

	if (facets != nullptr)
	{
		storeFacets();
	}

	// Afterward, we store the container itself
	EntityBase::storeToDataBase();
}

void EntityFaceAnnotation::storeFacets(void)
{
	// If necessary, assign an entity id and set the model for the facets object
	if (facets->getObserver() == nullptr) facets->setObserver(getObserver());
	if (facets->getModelState() == nullptr) facets->setModelState(getModelState());

	if (facets->getEntityID() == 0) facets->setEntityID(createEntityUID());

	// Now we need to store the facets object
	facets->storeToDataBase();
	facetsStorageID = facets->getEntityID();
}

void EntityFaceAnnotation::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the Brep and Facet objects
	facetsStorageID = doc_view["Facets"].get_int64();
	boundingBox.setFromBSON(doc_view["BoundingBox"].get_document().value);

	// Clean the objects from memory -> will be loaded on demand
	delete facets;
	facets = nullptr;

	resetModified();
}

void EntityFaceAnnotation::releaseFacets(void)
{
	if (facets != nullptr)
	{
		storeFacets();

		delete facets;
		facets = nullptr;
	}
}

void EntityFaceAnnotation::removeChild(EntityBase *child)
{
	if (child == facets)
	{
		facets = nullptr;
	}
	else
	{
		EntityBase::removeChild(child);
	}
}

void EntityFaceAnnotation::renewFacets(void)
{
	if (facets != nullptr)
	{
		delete facets;
		facets = nullptr;
	}

	facets = new EntityFacetData(createEntityUID(), this, getObserver(), getModelState());

	boundingBox.reset();
}

void EntityFaceAnnotation::storeUpdatedFacets(void)
{
	storeFacets();
	releaseFacets();

	updateVisualization(false);

	// Now the object is up-to-date and the update flags of the properties can be reset
	getProperties().forceResetUpdateForAllProperties();
	getProperties().checkWhetherUpdateNecessary();

	setModified();
}


#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"
#include "GeometryOperations.h"
#include "EntityFacetData.h"
#include "DataBase.h"
#include "Types.h"

#include <OpenTwinCommunication/ActionTypes.h>

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "BRepBndLib.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"

EntityFaceAnnotation::EntityFaceAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner),
	facetsStorageID(-1)
{
	EntityPropertiesColor::createProperty(  "General", "Color",           {0, 0, 0}, "", getProperties());
	EntityPropertiesInteger::createProperty("General", "Number of faces",         0, "", getProperties());

	getProperties().getProperty("Number of faces")->setReadOnly(true);

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
	EntityPropertiesString *reference = new EntityPropertiesString("Reference entity " + index, annotation.getEntityName());
	EntityPropertiesDouble *posX = new EntityPropertiesDouble("Position X " + index, annotation.getX());
	EntityPropertiesDouble *posY = new EntityPropertiesDouble("Position Y " + index, annotation.getY());
	EntityPropertiesDouble *posZ = new EntityPropertiesDouble("Position Z " + index, annotation.getZ());

	getProperties().createProperty(reference, "Location " + std::to_string(faceIndex));
	getProperties().createProperty(posX, "Location " + std::to_string(faceIndex));
	getProperties().createProperty(posY, "Location " + std::to_string(faceIndex));
	getProperties().createProperty(posZ, "Location " + std::to_string(faceIndex));

	reference->resetNeedsUpdate();
	posX->resetNeedsUpdate();
	posY->resetNeedsUpdate();
	posZ->resetNeedsUpdate();

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

void EntityFaceAnnotation::EnsureFacetsAreLoaded(void)
{
	if (facets != nullptr) return;

	assert(facetsStorageID != -1);

	// The facets are a hidden object (a child of the current object). Therefore it does not need to be present in the global maps
	std::map<ot::UID, EntityBase *> entityMap;
	facets = dynamic_cast<EntityFacetData *>(readEntityFromEntityID(this, facetsStorageID, entityMap));
}

void EntityFaceAnnotation::updateVisualization(bool isHidden)
{
	EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(color != nullptr);

	double colorRGB[3] = { color->getColorR(), color->getColorG(), color->getColorB() };
	std::string errors;

	// Note: this call will modify a potentially existing node rather than creating a new one
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "FaceAnnotationVisible";
	treeIcons.hiddenIcon = "FaceAnnotationHidden";

	ot::UID storageVersion = getCurrentEntityVersion(facetsStorageID);

	std::string materialType = "Rough";

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, colorRGB, 3);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, colorRGB, 3);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_MaterialType, materialType);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_TextureType, "None");
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_TextureReflective, false);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling, false);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_OffsetFactor, (double) 0.5);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, DataBase::GetDataBase()->getProjectName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ID, (unsigned long long) facetsStorageID);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Version, storageVersion);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_SelectChildren, getSelectChildren());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ManageParentVis, getManageParentVisibility());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ManageChildVis, getManageChildVisibility());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected, false);

	treeIcons.addToJsonDoc(&doc);	
	
	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(facetsStorageID, storageVersion));

	getObserver()->sendMessageToViewer(doc, prefetchIds);
}

void EntityFaceAnnotation::addVisualizationNodes(void)
{
	updateVisualization(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

std::list<TopoDS_Shape> EntityFaceAnnotation::findFacesFromShape(const TopoDS_Shape *shape)
{
	std::list<TopoDS_Shape> facesList;

	EntityPropertiesInteger *faceCount = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of faces"));
	assert(faceCount != nullptr);

	for (int faceIndex = 1; faceIndex <= faceCount->getValue(); faceIndex++)
	{
		findFacesAtIndexFromShape(facesList, faceIndex, shape);
	}

	return facesList;
}

void EntityFaceAnnotation::findFacesAtIndexFromShape(std::list<TopoDS_Shape> &facesList, int faceIndex, const TopoDS_Shape *shape)
{
	std::string index = "(" + std::to_string(faceIndex) + ")";

	EntityPropertiesDouble *posX = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position X " + index));
	assert(posX != nullptr);

	EntityPropertiesDouble *posY = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position Y " + index));
	assert(posY != nullptr);

	EntityPropertiesDouble *posZ = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position Z " + index));
	assert(posZ != nullptr);

	BRepBuilderAPI_MakeVertex vertex(gp_Pnt(posX->getValue(), posY->getValue(), posZ->getValue()));

	TopoDS_Shape result;
	bool success = false;

	double minDist = 1e30;

	TopExp_Explorer exp;
	for (exp.Init(*shape, TopAbs_SHELL); exp.More(); exp.Next())
	{
		TopoDS_Shell aShell = TopoDS::Shell(exp.Current());
		BRepExtrema_DistShapeShape minimumDist(aShell, vertex.Vertex(), Extrema_ExtFlag_MIN);

		int hits = minimumDist.NbSolution();

		assert(!minimumDist.InnerSolution());  // If the point is inside the solid, we get this behavior. In this case, it we don't know what the closest face is.
												// We need a special treatment for this situation (e.g. invert the solid or make a sheet model out of it).

		for (int hit = 1; hit <= hits; hit++)
		{
			if (minimumDist.SupportTypeShape1(hit) == BRepExtrema_SupportType::BRepExtrema_IsInFace)
			{
				// We found the face which is closest to the given point

				if (minimumDist.Value() < minDist)
				{
					success = true;
					minDist = minimumDist.Value();
					result = minimumDist.SupportOnShape1(hit);
				}
			}
		}
	}

	if (success)
	{
		facesList.push_back(result);
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

void EntityFaceAnnotation::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(
		bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON()),
		bsoncxx::builder::basic::kvp("Facets",      facetsStorageID)
	);
}

void EntityFaceAnnotation::StoreToDataBase(void)
{
	// If the pointers to brep or facets are 0, then the objects are stored in the data storage and the storage IDs are up to date

	if (facets != nullptr)
	{
		storeFacets();
	}

	// Afterward, we store the container itself
	EntityBase::StoreToDataBase();
}

void EntityFaceAnnotation::storeFacets(void)
{
	// If necessary, assign an entity id and set the model for the facets object
	if (facets->getObserver() == nullptr) facets->setObserver(getObserver());
	if (facets->getModelState() == nullptr) facets->setModelState(getModelState());

	if (facets->getEntityID() == 0) facets->setEntityID(createEntityUID());

	// Now we need to store the facets object
	facets->StoreToDataBase();
	facetsStorageID = facets->getEntityID();
}

void EntityFaceAnnotation::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
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

	facets = new EntityFacetData(createEntityUID(), this, getObserver(), getModelState(), getClassFactory(), getOwningService());

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

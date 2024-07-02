// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityGeometry.h"
#include "DataBase.h"

#include "OTCommunication/ActionTypes.h"

#include "GeometryOperations.h"

#include "Bnd_Box.hxx"
#include "BRepBndLib.hxx"
#include "BRepTools_ShapeSet.hxx"
#include "BRepTools.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "gp_Ax1.hxx"

EntityGeometry::EntityGeometry(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner),
	brepStorageID(-1),
	facetsStorageID(-1),
	showWhenSelected(false),
	tmpData(nullptr)
{
	brep   = new EntityBrep(0, this, obs, ms, factory, owner);
	facets = new EntityFacetData(0, this, obs, ms, factory, owner);

	treeIcons.size = 32;
	treeIcons.visibleIcon = "ModelVisible";
	treeIcons.hiddenIcon = "ModelHidden";
}

EntityGeometry::~EntityGeometry()
{
	if (brep != nullptr)
	{
		delete brep;
		brep = nullptr;
	}

	if (facets != nullptr)
	{
		delete facets;
		facets = nullptr;
	}
}

void EntityGeometry::resetBrep(void)
{
	if (brep != nullptr)
	{
		delete brep;
		brep = nullptr;
	}

	brep = new EntityBrep(0, this, getObserver(), getModelState(), getClassFactory(), getOwningService());
	brepStorageID = -1;
}

void EntityGeometry::resetFacets(void)
{
	if (facets != nullptr)
	{
		delete facets;
		facets = nullptr;
	}

	facets = new EntityFacetData(0, this, getObserver(), getModelState(), getClassFactory(), getOwningService());
	facetsStorageID = -1;
}

bool EntityGeometry::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	EnsureBrepIsLoaded();

	assert(brep != nullptr);
	if (brep == nullptr) return false;
	
	try
	{
		return brep->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax);
	}
	catch (Standard_Failure)
	{
		return false; // This shape seems to be empty
	}
}

bool EntityGeometry::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Check the color
	EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(color != nullptr);

	bool typeNeedsUpdate = false;
	std::string materialType = "Rough";
	std::string textureType = "None";

	EntityPropertiesSelection *typeProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));

	if (typeProp != nullptr)
	{
		materialType = typeProp->getValue();
		typeNeedsUpdate = typeProp->needsUpdate();
	}

	EntityPropertiesSelection *textureProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Texture"));

	if (textureProp != nullptr)
	{
		textureType = textureProp->getValue();
		typeNeedsUpdate |= textureProp->needsUpdate();
	}

	if (color->needsUpdate() || typeNeedsUpdate)
	{
		std::vector<double> surfaceColorRGB;
		surfaceColorRGB.push_back(color->getColorR());
		surfaceColorRGB.push_back(color->getColorG());
		surfaceColorRGB.push_back(color->getColorB());
		
		std::vector<double> edgeColorRGB = surfaceColorRGB;

		assert(getObserver() != nullptr);

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, ot::JsonArray(surfaceColorRGB, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, ot::JsonArray(edgeColorRGB, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_MaterialType, ot::JsonString(materialType, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureType, ot::JsonString(textureType, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureReflective, this->isTextureReflective(textureType), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);

		// We have processed the color change -> reset the needs update flag for this property
		color->resetNeedsUpdate();
		if (typeProp != nullptr) typeProp->resetNeedsUpdate();
		if (textureProp != nullptr) textureProp->resetNeedsUpdate();
	}

	updateDimensionProperties();

	bool updatePropertiesGrid = updatePropertyVisibilities();

	return updatePropertiesGrid;
}

bool EntityGeometry::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesDouble *meshStepWidth = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Maximum edge length"));
	EntityPropertiesDouble *meshPriority = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Mesh priority"));
	EntityPropertiesBoolean *considerForMeshing = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Consider for meshing"));
	EntityPropertiesSelection *meshRefinment = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Mesh refinement"));
	EntityPropertiesBoolean *curvatureRefinment = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Curvature refinement"));
	EntityPropertiesInteger *numStepsPerCircle = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of steps per circle"));
	EntityPropertiesDouble *minCurvRefinementRadius = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Minimum curvature refinement radius"));
	EntityPropertiesDouble *maximumDeviation = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Maximum deviation of mesh from shape"));
	EntityPropertiesEntityList *material = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Material"));

	bool showMeshStepWidth = true;
	bool showMeshPriority = true;
	bool showMeshRefinment = true;
	bool showCurvatureRefinment = true;
	bool showNumStepsPerCircle = true;
	bool showMinCurvRefinementRadius = true;
	bool showMaximumDeviation = true;

	if (considerForMeshing != nullptr)
	{
		if (!considerForMeshing->getValue())
		{
			// We do not consider this shape for meshing -> turn off all meshing relevant properties
			showMeshStepWidth = false;
			showMeshPriority = false;
			showMeshRefinment = false;
			showCurvatureRefinment = false;
			showNumStepsPerCircle = false;
			showMinCurvRefinementRadius = false;
			showMaximumDeviation = false;
		}
	}

	if (meshRefinment != nullptr)
	{
		if (meshRefinment->getValue() == "Global setting")
		{
			// We do not use local mesh properties (except for the maximum edge length)
			showCurvatureRefinment = false;
			showNumStepsPerCircle = false;
			showMinCurvRefinementRadius = false;
			showMaximumDeviation = false;
		}
	}

	if (curvatureRefinment != nullptr)
	{
		if (!curvatureRefinment->getValue())
		{
			// We do not use local curvature refinement 
			showNumStepsPerCircle = false;
			showMinCurvRefinementRadius = false;
			showMaximumDeviation = false;
		}
	}

	if (meshStepWidth != nullptr)
	{
		if (meshStepWidth->getVisible() != showMeshStepWidth)
		{
			meshStepWidth->setVisible(showMeshStepWidth);
			updatePropertiesGrid = true;
		}
	}

	if (meshPriority != nullptr)
	{
		if (meshPriority->getVisible() != showMeshPriority)
		{
			meshPriority->setVisible(showMeshPriority);
			updatePropertiesGrid = true;
		}
	}

	if (meshRefinment != nullptr)
	{
		if (meshRefinment->getVisible() != showMeshRefinment)
		{
			meshRefinment->setVisible(showMeshRefinment);
			updatePropertiesGrid = true;
		}
	}

	if (curvatureRefinment != nullptr)
	{
		if (curvatureRefinment->getVisible() != showCurvatureRefinment)
		{
			curvatureRefinment->setVisible(showCurvatureRefinment);
			updatePropertiesGrid = true;
		}
	}

	if (numStepsPerCircle != nullptr)
	{
		if (numStepsPerCircle->getVisible() != showNumStepsPerCircle)
		{
			numStepsPerCircle->setVisible(showNumStepsPerCircle);
			updatePropertiesGrid = true;
		}
	}

	if (minCurvRefinementRadius != nullptr)
	{
		if (minCurvRefinementRadius->getVisible() != showMinCurvRefinementRadius)
		{
			minCurvRefinementRadius->setVisible(showMinCurvRefinementRadius);
			updatePropertiesGrid = true;
		}
	}

	if (maximumDeviation != nullptr)
	{
		if (maximumDeviation->getVisible() != showMaximumDeviation)
		{
			maximumDeviation->setVisible(showMaximumDeviation);
			updatePropertiesGrid = true;
		}
	}

	if (meshStepWidth != nullptr) meshStepWidth->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (meshPriority != nullptr) meshPriority->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (considerForMeshing != nullptr) considerForMeshing->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (meshRefinment != nullptr) meshRefinment->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (curvatureRefinment != nullptr) curvatureRefinment->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (numStepsPerCircle != nullptr) numStepsPerCircle->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (minCurvRefinementRadius != nullptr) minCurvRefinementRadius->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (maximumDeviation != nullptr) maximumDeviation->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (material != nullptr) material->resetNeedsUpdate(); // This will be used for meshing and solving only, no action necessary here.
	if (material != nullptr) material->resetNeedsUpdate(); // This will be used for meshing and solving only, no action necessary here.

	return updatePropertiesGrid;
}

void EntityGeometry::StoreToDataBase(void)
{
	// If the pointers to brep or facets are 0, then the objects are stored in the data storage and the storage IDs are up to date

	if (brep != nullptr)
	{
		storeBrep();
	}

	if (facets != nullptr)
	{
		storeFacets();
	}

	// Afterward, we store the container itself
	EntityContainer::StoreToDataBase();
}

void EntityGeometry::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(
		bsoncxx::builder::basic::kvp("Brep", brepStorageID),
		bsoncxx::builder::basic::kvp("Facets", facetsStorageID),
		bsoncxx::builder::basic::kvp("visibleIcon", treeIcons.visibleIcon),
		bsoncxx::builder::basic::kvp("hiddenIcon", treeIcons.hiddenIcon),
		bsoncxx::builder::basic::kvp("showWhenSelected", showWhenSelected)
	);
}

void EntityGeometry::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the Brep and Facet objects
	brepStorageID   = doc_view["Brep"].get_int64();
	facetsStorageID = doc_view["Facets"].get_int64();

	treeIcons.visibleIcon = "ModelVisible";
	treeIcons.hiddenIcon = "ModelHidden";

	try
	{
		treeIcons.visibleIcon = doc_view["visibleIcon"].get_utf8().value.data();
	}
	catch (std::exception)
	{
	}

	try
	{
		treeIcons.hiddenIcon = doc_view["hiddenIcon"].get_utf8().value.data();
	}
	catch (std::exception)
	{
	}

	showWhenSelected = false;

	try
	{
		showWhenSelected = doc_view["showWhenSelected"].get_bool();
	}
	catch (std::exception)
	{
	}

	// Clean the objects from memory -> will be loaded on demand
	delete brep;
	brep = nullptr;

	delete facets;
	facets = nullptr;

	resetModified();
}

void EntityGeometry::EnsureBrepIsLoaded(void)
{
	if (brep != nullptr) return;

	assert(brepStorageID != -1);

	// The brep is a hidden object (a child of the current object). Therefore it does not need to be present in the global maps
	std::map<ot::UID, EntityBase *> entityMap;
	brep = dynamic_cast<EntityBrep *>(readEntityFromEntityID(this, brepStorageID, entityMap));
}

void EntityGeometry::EnsureFacetsAreLoaded(void)
{
	if (facets != nullptr) return;

	assert(facetsStorageID != -1);

	// The facets are a hidden object (a child of the current object). Therefore it does not need to be present in the global maps
	std::map<ot::UID, EntityBase *> entityMap;
	facets = dynamic_cast<EntityFacetData *>(readEntityFromEntityID(this, facetsStorageID, entityMap));
}

long long EntityGeometry::getBrepPrefetchID(void)
{
	if (brep != nullptr) return -1;
	return brepStorageID;
}

long long EntityGeometry::getFacetsPrefetchID(void)
{
	if (facets != nullptr) return -1;
	return facetsStorageID;
}

void EntityGeometry::addVisualizationNodes(void)
{
	EntityPropertiesColor *colorProp = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(colorProp != nullptr);

	std::vector<double> colorRGB;
	
	if (colorProp)
	{
		colorRGB.push_back(colorProp->getColorR());
		colorRGB.push_back(colorProp->getColorG());
		colorRGB.push_back(colorProp->getColorB());
	}
	else {
		colorRGB.push_back(0.8);
		colorRGB.push_back(0.8);
		colorRGB.push_back(0.8);
	}

	std::string materialType = "Rough";

	EntityPropertiesSelection *typeProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));

	if (typeProp != nullptr)
	{
		materialType = typeProp->getValue();
	}

	std::string textureType = "None";

	EntityPropertiesSelection *textureProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Texture"));

	if (textureProp != nullptr)
	{
		textureType = textureProp->getValue();
	}

	ot::UID storageVersion = getCurrentEntityVersion(facetsStorageID);

	std::vector<double> transformation;
	getTransformation(transformation);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, ot::JsonArray(colorRGB, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, ot::JsonArray(colorRGB, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_MaterialType, ot::JsonString(materialType, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureType, ot::JsonString(textureType, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureReflective, this->isTextureReflective(textureType), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_OffsetFactor, 1., doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, this->getInitiallyHidden(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(DataBase::GetDataBase()->getProjectName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, (unsigned long long) facetsStorageID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, storageVersion, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SelectChildren, this->getSelectChildren(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageParentVis, this->getManageParentVisibility(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageChildVis, this->getManageChildVisibility(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected, this->getShowWhenSelected(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Transformation, ot::JsonArray(transformation, doc.GetAllocator()), doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(facetsStorageID, storageVersion));

	getObserver()->sendMessageToViewer(doc, prefetchIds);

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityGeometry::setBrep(TopoDS_Shape shape)
{
	EnsureBrepIsLoaded();

	assert(brep != nullptr); 
	brep->setBrep(shape); 
	setModified();
}

TopoDS_Shape &EntityGeometry::getBrep(void) 
{ 
	EnsureBrepIsLoaded();

	assert(brep != nullptr); 
	return brep->getBrep(); 
}

EntityFacetData* EntityGeometry::getFacets(void) 
{ 
	EnsureFacetsAreLoaded();

	assert(facets != nullptr);
	return facets; 
}

EntityBrep* EntityGeometry::getBrepEntity(void)
{
	EnsureBrepIsLoaded();

	assert(brep != nullptr); 
	return brep; 
}

void EntityGeometry::storeBrep(void)
{
	if (brep != nullptr)
	{
		if (brep->getObserver() == nullptr) brep->setObserver(getObserver());
		if (brep->getModelState() == nullptr) brep->setModelState(getModelState());

		if (brep->getEntityID() == 0) brep->setEntityID(createEntityUID());

		// First, we need to store the brep object
		brep->StoreToDataBase();
		brepStorageID = brep->getEntityID();
	}
}

void EntityGeometry::detachBrep(void)
{
	brep = nullptr;
}

void EntityGeometry::storeFacets(void)
{
	// If necessary, assign an entity id and set the model for the facets object
	if (facets->getObserver() == nullptr) facets->setObserver(getObserver());
	if (facets->getModelState() == nullptr) facets->setModelState(getModelState());

	if (facets->getEntityID() == 0) facets->setEntityID(createEntityUID());

	// Now we need to store the facets object
	facets->StoreToDataBase();
	facetsStorageID = facets->getEntityID();
}

void EntityGeometry::releaseBrep(void)
{
	if (brep != nullptr)
	{
		storeBrep();

		delete brep;
		brep = nullptr;
	}
}

void EntityGeometry::releaseFacets(void)
{
	if (facets != nullptr)
	{
		storeFacets();

		delete facets;
		facets = nullptr;
	}
}

void EntityGeometry::addPrefetchingRequirementsForBrep(std::list<ot::UID> &prefetchIds)
{
	if (brep == nullptr)
	{
		if (brepStorageID > 0)
		{
			prefetchIds.push_back(brepStorageID);
		}
	}
}

void EntityGeometry::addPrefetchingRequirementsForFacets(std::list<ot::UID> &prefetchIds)
{
	if (facets == nullptr)
	{
		if (facetsStorageID > 0)
		{
			prefetchIds.push_back(facetsStorageID);
		}
	}
}

void EntityGeometry::removeChild(EntityBase *child)
{
	if (child == brep)
	{
		brep = nullptr;
	}
	else if (child == facets)
	{
		facets = nullptr;
	}
	else
	{
		EntityContainer::removeChild(child);
	}
}

void EntityGeometry::createTransformProperty(const std::string &propName, double value)
{
	EntityPropertiesDouble *doubleProp = new EntityPropertiesDouble(propName, value);
	doubleProp->setVisible(false);

	getProperties().createProperty(doubleProp, "Transformation");
}

void EntityGeometry::createProperties(int colorR, int colorG, int colorB, const std::string materialsFolder, ot::UID materialsFolderID)
{
	createMaterialPropertiesOnly(colorR, colorG, colorB, materialsFolder, materialsFolderID);
	createNonMaterialProperties();
}

void EntityGeometry::createMaterialPropertiesOnly(int colorR, int colorG, int colorB, const std::string materialsFolder, ot::UID materialsFolderID)
{
	EntityPropertiesEntityList::createProperty("Solver", "Material", materialsFolder, materialsFolderID, "", -1, "", getProperties());

	EntityPropertiesColor::createProperty("Appearance", "Color", { colorR, colorG, colorB}, "", getProperties());
	EntityPropertiesColor::createProperty("Appearance", "Color", { colorR, colorG, colorB}, "", getProperties());
	EntityPropertiesSelection::createProperty("Appearance", "Type", { "Rough", "Plastic", "Polished", "Metal" }, "Rough", "", getProperties());
	EntityPropertiesSelection::createProperty("Appearance", "Texture", { "None", "Rough", "Gold", "Aluminium", "Reflective" }, "None", "", getProperties());
}

void EntityGeometry::createNonMaterialProperties(void)
{
	createTransformProperty("#Position X", 0.0);
	createTransformProperty("#Position Y", 0.0);
	createTransformProperty("#Position Z", 0.0);

	createTransformProperty("#Axis X", 0.0);
	createTransformProperty("#Axis Y", 0.0);
	createTransformProperty("#Axis Z", 0.0);

	createTransformProperty("#Angle (deg.)", 0.0);

	EntityPropertiesString::createProperty("Transformation", "Position X", "0",	"", getProperties());
	EntityPropertiesString::createProperty("Transformation", "Position Y", "0",	"", getProperties());
	EntityPropertiesString::createProperty("Transformation", "Position Z", "0",	"", getProperties());

	EntityPropertiesString::createProperty("Transformation", "Axis X", "0",	"", getProperties());
	EntityPropertiesString::createProperty("Transformation", "Axis Y", "0",	"", getProperties());
	EntityPropertiesString::createProperty("Transformation", "Axis Z", "0",	"", getProperties());

	EntityPropertiesString::createProperty("Transformation", "Angle (deg.)", "0",	"", getProperties());

	EntityPropertiesBoolean::createProperty("Mesh", "Consider for meshing",											  true, "",     getProperties());
	EntityPropertiesDouble::createProperty("Mesh", "Mesh priority",													   0.0,	"",     getProperties());
	EntityPropertiesSelection::createProperty("Mesh", "Mesh refinement", { "Global setting", "Local setting" }, "Global setting", "", getProperties());
	EntityPropertiesDouble::createProperty("Mesh", "Maximum edge length",											   0.0, "",     getProperties());

	EntityPropertiesBoolean::createProperty("Mesh refinement", "Curvature refinement",					true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Mesh refinement", "Maximum deviation of mesh from shape", 0.0, "Tetrahedral Meshing", getProperties());
	EntityPropertiesInteger::createProperty("Mesh refinement", "Number of steps per circle",			   6, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Mesh refinement", "Minimum curvature refinement radius",    0.0, "Tetrahedral Meshing",  getProperties());

	EntityPropertiesBoolean *transformNeedsUpdateProp = new EntityPropertiesBoolean("transformNeedsUpdate", false);
	transformNeedsUpdateProp->setVisible(false);
	getProperties().createProperty(transformNeedsUpdateProp, "Internal");

	EntityPropertiesBoolean *geometryNeedsUpdateProp = new EntityPropertiesBoolean("geometryNeedsUpdate", false);
	geometryNeedsUpdateProp->setVisible(false);
	getProperties().createProperty(geometryNeedsUpdateProp, "Internal");

	getProperties().forceResetUpdateForAllProperties();

	updatePropertyVisibilities();
}

void EntityGeometry::resetTransformation(void)
{
	// First we reset the transformation string properties

	EntityPropertiesString *posX = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position X"));
	if (posX != nullptr) posX->setValue("0");

	EntityPropertiesString *posY = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position Y"));
	if (posY != nullptr) posY->setValue("0");

	EntityPropertiesString *posZ = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position Z"));
	if (posZ != nullptr) posZ->setValue("0");

	EntityPropertiesString *axisX = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis X"));
	if (axisX != nullptr) axisX->setValue("0");

	EntityPropertiesString *axisY = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis Y"));
	if (axisY != nullptr) axisY->setValue("0");

	EntityPropertiesString *axisZ = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis Z"));
	if (axisZ != nullptr) axisZ->setValue("0");

	EntityPropertiesString *angle = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Angle (deg.)"));
	if (angle != nullptr) angle->setValue("0");

	// Now we reset the corresponding double properties

	EntityPropertiesDouble *posXvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position X"));
	if (posXvalue != nullptr) posXvalue->setValue(0);

	EntityPropertiesDouble *posYvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Y"));
	if (posYvalue != nullptr) posYvalue->setValue(0);

	EntityPropertiesDouble *posZvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Z"));
	if (posZvalue != nullptr) posZvalue->setValue(0);

	EntityPropertiesDouble *axisXvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis X"));
	if (axisXvalue != nullptr) axisXvalue->setValue(0);

	EntityPropertiesDouble *axisYvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Y"));
	if (axisYvalue != nullptr) axisYvalue->setValue(0);

	EntityPropertiesDouble *axisZvalue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Z"));
	if (axisZvalue != nullptr) axisZvalue->setValue(0);

	EntityPropertiesDouble *angleValue = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Angle (deg.)"));
	if (angleValue != nullptr) angleValue->setValue(0);
}

void EntityGeometry::getTransformation(std::vector<double> &transformation)
{
	// Initialize an identity matrix
	gp_Trsf transform;

	// Get the current transformation matrix
	EntityPropertiesDouble *xposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position X"));
	EntityPropertiesDouble *yposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Y"));
	EntityPropertiesDouble *zposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Z"));
	EntityPropertiesDouble *xAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis X"));
	EntityPropertiesDouble *yAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Y"));
	EntityPropertiesDouble *zAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Z"));
	EntityPropertiesDouble *angleProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Angle (deg.)"));

	if (xposProperty != nullptr && yposProperty != nullptr && zposProperty != nullptr && xAxisProperty != nullptr && yAxisProperty != nullptr && zAxisProperty != nullptr && angleProperty != nullptr)
	{
		double xp = xposProperty->getValue();
		double yp = yposProperty->getValue();
		double zp = zposProperty->getValue();

		double vx = xAxisProperty->getValue();
		double vy = yAxisProperty->getValue();
		double vz = zAxisProperty->getValue();

		double angle = angleProperty->getValue() / 180.0 * M_PI;

		gp_Trsf transformR, transformT;

		if (vx != 0.0 || vy != 0.0 || vz != 0.0)
		{
			transformR.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(vx, vy, vz)), angle);
		}

		transformT.SetTranslation(gp_Vec(xp, yp, zp));

		transform = transformT * transformR;
	}

	// Convert the transformation to a 4x4 matrix
	NCollection_Mat4<double> matrix4x4;
	transform.GetMat4(matrix4x4);

	transformation.reserve(16);
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			transformation.push_back(matrix4x4.GetValue(row, col));
		}
	}
}

void EntityGeometry::setTransformation(const std::vector<double> &transformation)
{
	assert(transformation.size() == 16);

	// Get the current transformation matrix
	EntityPropertiesDouble *xposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position X"));
	EntityPropertiesDouble *yposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Y"));
	EntityPropertiesDouble *zposProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Position Z"));
	EntityPropertiesDouble *xAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis X"));
	EntityPropertiesDouble *yAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Y"));
	EntityPropertiesDouble *zAxisProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Axis Z"));
	EntityPropertiesDouble *angleProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Angle (deg.)"));

	EntityPropertiesString *xposPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position X"));
	EntityPropertiesString *yposPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position Y"));
	EntityPropertiesString *zposPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Position Z"));
	EntityPropertiesString *xAxisPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis X"));
	EntityPropertiesString *yAxisPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis Y"));
	EntityPropertiesString *zAxisPropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Axis Z"));
	EntityPropertiesString *anglePropertyString = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Angle (deg.)"));

	if (xposProperty != nullptr && yposProperty != nullptr && zposProperty != nullptr && xAxisProperty != nullptr && yAxisProperty != nullptr && zAxisProperty != nullptr && angleProperty != nullptr
		&& xposPropertyString != nullptr && yposPropertyString != nullptr && zposPropertyString != nullptr && xAxisPropertyString != nullptr && yAxisPropertyString != nullptr && zAxisPropertyString != nullptr && anglePropertyString != nullptr)
	{
		gp_Trsf transform;

		transform.SetValues(transformation[ 0], transformation[ 1], transformation[ 2], transformation[ 3],
							transformation[ 4], transformation[ 5], transformation[ 6], transformation[ 7],
							transformation[ 8], transformation[ 9], transformation[10], transformation[11]);
		assert(transformation[12] == 0.0);
		assert(transformation[13] == 0.0);
		assert(transformation[14] == 0.0);
		assert(transformation[15] == 1.0);

		gp_XYZ axis;
		double angle = 0.0;
		transform.GetRotation(axis, angle);
		angle *= 180.0 / M_PI;

		gp_XYZ translation = transform.TranslationPart();

		xposProperty->setValue(translation.X());
		yposProperty->setValue(translation.Y());
		zposProperty->setValue(translation.Z());

		xAxisProperty->setValue(axis.X());
		yAxisProperty->setValue(axis.Y());
		zAxisProperty->setValue(axis.Z());

		angleProperty->setValue(angle);

		xposPropertyString->setValue(std::to_string(translation.X()));
		yposPropertyString->setValue(std::to_string(translation.Y()));
		zposPropertyString->setValue(std::to_string(translation.Z()));

		xAxisPropertyString->setValue(std::to_string(axis.X()));
		yAxisPropertyString->setValue(std::to_string(axis.Y()));
		zAxisPropertyString->setValue(std::to_string(axis.Z()));

		anglePropertyString->setValue(std::to_string(angle));
	}
	else
	{
		assert(0);  // Some transformation properties are missing
	}
}

bool EntityGeometry::isTextureReflective(const std::string textureType)
{
	if (textureType == "Reflective") return true;

	return false;
}

void EntityGeometry::facetEntity(bool isHidden)
{
	double xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0), zmin(0.0), zmax(0.0);

	try
	{
		getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax);
	}
	catch (Standard_Failure)
	{
		return; // This shape seems to be empty
	}

	double diagonal = sqrt((xmax - xmin) * (xmax - xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

	double deflection = 0.0008 * diagonal;

	double colorRGB[3] = { 0.8, 0.8, 0.8 };
	std::string name;

	EntityPropertiesColor *colorProp = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(colorProp != nullptr);

	if (colorProp != nullptr)
	{
		colorRGB[0] = colorProp->getColorR();
		colorRGB[1] = colorProp->getColorG();
		colorRGB[2] = colorProp->getColorB();
	}

	std::string materialType = "Rough";

	EntityPropertiesSelection *typeProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));

	if (typeProp != nullptr)
	{
		materialType = typeProp->getValue();
	}

	std::string textureType = "None";
	bool textureReflective = false;

	EntityPropertiesSelection *textureProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Texture"));

	if (textureProp != nullptr)
	{
		textureType = textureProp->getValue();
	}

	getFacets()->resetErrors();

	getFacets()->getNodeVector().clear();
	getFacets()->getTriangleList().clear();
	getFacets()->getEdgeList().clear();

	GeometryOperations::facetEntity(getBrep(), brep, deflection, 
									getFacets()->getNodeVector(), getFacets()->getTriangleList(), getFacets()->getEdgeList(), getFacets()->getFaceNameMap(), getFacets()->getErrorString());
	getFacets()->setModified();

	// If we don't have a model state, are are creating the facets temporarily. Otherwise we save them to the data base.
	if (getModelState() != nullptr)
	{
		StoreToDataBase();

		// We do not need to keep the facets in memory. Allow the object to clear memory data
		releaseFacets();
	}

	setModified(); // The model has changed
}

double EntityGeometry::evaluateExpressionDouble(const std::string &expr)
{
	return std::stod(expr);
}

void EntityGeometry::updateDimensionProperties(void)
{
	// Here we loop over all properties and check which ones need to be updated
	std::list<EntityPropertiesBase *> needUpdate = getProperties().getListOfPropertiesWhichNeedUpdate();

	bool shapeNeedsUpdate = false;
	bool transformNeedsUpdate = false;

	for (auto prop : needUpdate)
	{
		assert(prop->needsUpdate());
		EntityPropertiesDouble *doubleProperty = dynamic_cast<EntityPropertiesDouble*>(prop);

		if (doubleProperty != nullptr)
		{
			doubleProperty->resetNeedsUpdate();

			if (isTransformProperty(prop->getName().substr(1)))
			{
				transformNeedsUpdate = true;
			}
			else
			{
				shapeNeedsUpdate = true;
			}
		}
	}

	EntityPropertiesBoolean *needsTransformUpdateProperty = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("transformNeedsUpdate"));

	if (needsTransformUpdateProperty != nullptr)
	{
		needsTransformUpdateProperty->setValue(transformNeedsUpdate);
		needsTransformUpdateProperty->resetNeedsUpdate();
	}

	EntityPropertiesBoolean *needsGeometryUpdateProperty = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("geometryNeedsUpdate"));

	if (needsGeometryUpdateProperty != nullptr)
	{
		needsGeometryUpdateProperty->setValue(shapeNeedsUpdate);
		needsGeometryUpdateProperty->resetNeedsUpdate();
	}
}

bool EntityGeometry::isTransformProperty(const std::string &propName)
{
	if (propName == "Position X" || propName == "Position Y" || propName == "Position Z") return true;
	if (propName == "Axis X"     || propName == "Axis Y"     || propName == "Axis Z") return true;
	if (propName == "Angle (deg.)") return true;

	return false;
}

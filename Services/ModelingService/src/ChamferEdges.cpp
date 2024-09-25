#include "ChamferEdges.h"
#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "DataBase.h"
#include "EntityCache.h"
#include "UpdateManager.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepAlgoAPI_Defeaturing.hxx"
#include <BRepFilletAPI_MakeFillet.hxx>

#include <map>
#include <set>

void ChamferEdges::enterChamferEdgesMode(void)
{
	std::map<std::string, std::string> options;

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::EDGE, 
									      true, "", ot::components::UiComponent::entitySelectionAction::CHAMFER_EDGE, "chamfer", options, serviceID);
}

void ChamferEdges::performOperation(const std::string &selectionInfo)
{
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockNavigationWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockProperties);

	uiComponent->lockUI(lockFlags);

	ot::JsonDocument doc;
	doc.fromJson(selectionInfo);

	ot::ConstJsonArray modelID = ot::json::getArray(doc, "modelID");
	ot::ConstJsonArray posX = ot::json::getArray(doc, "posX");
	ot::ConstJsonArray posY = ot::json::getArray(doc, "posY");
	ot::ConstJsonArray posZ = ot::json::getArray(doc, "posZ");
	ot::ConstJsonArray edgeName = ot::json::getArray(doc, "edgeName");

	// Get a list of all shapes which are involved in this operation for prefetching
	std::set<ot::UID> affectedEntitiesSet;
	std::list<ChamferEdgesData> edgeList;

	for (unsigned int i = 0; i < modelID.Size(); i++)
	{
		ot::UID entityID = modelID[i].GetUint64();
		
		affectedEntitiesSet.insert(entityID);

		ChamferEdgesData edgeData;
		edgeData.setEntityID(entityID);
		edgeData.setPosition(posX[i].GetDouble(), posY[i].GetDouble(), posZ[i].GetDouble());
		edgeData.setEdgeName(edgeName[i].GetString());
		edgeList.push_back(edgeData);
	}

	// Check whether all edges belong to the same entity
	if (affectedEntitiesSet.size() != 1)
	{
		uiComponent->displayErrorPrompt("All selected edges must belong to the same shape.");

		// Request a view refresh and release the user interface
		uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());
		uiComponent->unlockUI(lockFlags);

		return;
	}

	// Now get the current versions for the entity
	std::list<ot::UID> affectedEntities(affectedEntitiesSet.begin(), affectedEntitiesSet.end());
	assert(affectedEntities.size() == 1);

	std::list<ot::EntityInformation> entityInfo;
	modelComponent->getEntityInformation(affectedEntities, entityInfo);

	// Prefetch the geometry entity
	entityCache->prefetchEntities(entityInfo);

	// Now we prefetch all breps for these entities
	std::list<std::pair<ot::UID, ot::UID>> prefetchList;

	//entityVersionMap[entity.getID()] = entity.getVersion();

	EntityGeometry *baseEntity = dynamic_cast<EntityGeometry *>(entityCache->getEntity(entityInfo.front().getID(), entityInfo.front().getVersion()));
	ot::UID entityBrepID = baseEntity->getBrepStorageObjectID();

	std::list<ot::EntityInformation> entityBrepInfo;
	modelComponent->getEntityInformation(std::list<ot::UID>{entityBrepID}, entityBrepInfo);

	// Prefetch the brep entity
	entityCache->prefetchEntities(entityBrepInfo);

	//// Add the brep entities to the version map
	//for (auto entity : entityBrepInfo)
	//{
	//	entityVersionMap[entity.getID()] = entity.getVersion();
	//}

	EntityBrep* brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(entityBrepInfo.front().getID(), entityBrepInfo.front().getVersion()));
	assert(brepEntity != nullptr);

	std::string error;

	// TODO: Chamfer edge operation
	bool success = true;

	TopoDS_Shape shape = brepEntity->getBrep();

	// Build a new geometry entity and store it 
	if (success)
	{
		std::list<std::string> entityNameList;
		entityNameList.push_back(baseEntity->getName());

		// Now we update the base entity
		ot::UID entityID = modelComponent->createEntityUID();
		ot::UID brepID   = modelComponent->createEntityUID();
		ot::UID facetsID = modelComponent->createEntityUID();

		EntityGeometry* geometryEntity = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, serviceName);
		geometryEntity->setName(baseEntity->getName());
		geometryEntity->setEditable(true);
		geometryEntity->setSelectChildren(false);
		geometryEntity->setManageChildVisibility(false);
		//geometryEntity->setManageParentVisibility(false);  // The new boolean entity should manage the parent visibility as usual
		geometryEntity->setBrep(shape);
		geometryEntity->setTreeIcons("BooleanAddVisible", "BooleanAddHidden");
		//geometryEntity->getBrepEntity()->setFaceNameMap(resultFaceNames);

		geometryEntity->getProperties() = baseEntity->getProperties();

		deletePropertyCategory(geometryEntity, "Dimensions");
		deletePropertyCategory(geometryEntity, "Internal");

		EntityPropertiesDouble* doubleProp = new EntityPropertiesDouble("#Chamfer width", 0.0);
		doubleProp->setVisible(false);
		geometryEntity->getProperties().createProperty(doubleProp, "Dimensions", true);

		EntityPropertiesString* stringProp = new EntityPropertiesString("Chamfer width", "0");
		geometryEntity->getProperties().createProperty(stringProp, "Dimensions", true);

		EntityPropertiesString* baseShapeProperty = new EntityPropertiesString("baseShape", std::to_string(baseEntity->getEntityID()));
		baseShapeProperty->setVisible(false);
		geometryEntity->getProperties().createProperty(baseShapeProperty, "Internal");

		EntityPropertiesString* typeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("shapeType"));

		if (typeProperty != nullptr)
		{
			typeProperty->setValue("Chamfer Edges");
		}
		else
		{
			typeProperty = new EntityPropertiesString("shapeType", "Chamfer Edges");
			typeProperty->setVisible(false);
			geometryEntity->getProperties().createProperty(typeProperty, "Internal");
		}

		// The geometry entity has two children: brep and facets. We need to assign ids to both of them (since we do not have a model state object here)
		geometryEntity->getBrepEntity()->setEntityID(brepID);
		geometryEntity->getFacets()->setEntityID(facetsID);

		// Now we facet the entity and store it to the data base afterward.
		geometryEntity->facetEntity(false);
		geometryEntity->StoreToDataBase();

		ot::UID entityVersion = geometryEntity->getEntityStorageVersion();
		ot::UID brepVersion = geometryEntity->getBrepEntity()->getEntityStorageVersion();
		ot::UID facetsVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		entityCache->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		// Finally add the new entity to the model (data children need to come first)

		std::list<ot::UID> dataEntityIDList = { brepID, facetsID };
		std::list<ot::UID> dataEntityVersionList = { brepVersion , facetsVersion };
		std::list<ot::UID> dataEntityParentList = { entityID, entityID };

		modelComponent->addGeometryOperation(entityID, entityVersion, baseEntity->getName(), dataEntityIDList, dataEntityVersionList, dataEntityParentList, entityNameList, "chamfer edges: " + geometryEntity->getName());

		delete geometryEntity;
		geometryEntity = nullptr;
	}

	// Request a view refresh and release the user interface
	uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());
	uiComponent->unlockUI(lockFlags);

	// Notify the error, if any
	if (!error.empty())
	{
		uiComponent->displayErrorPrompt(error);
	}
}

void ChamferEdges::storeEdgeListInProperties(std::list<ChamferEdgesData> &edgeList, EntityProperties &properties)
{

}

std::list<ChamferEdgesData> ChamferEdges::readEdgeListFromProperties(EntityProperties& properties)
{
	std::list<ChamferEdgesData> edgeList;


	return edgeList;
}

void ChamferEdges::deletePropertyCategory(EntityGeometry* geometryEntity, const std::string category)
{
	std::list<std::string> deleteProperties = geometryEntity->getProperties().getListOfPropertiesForGroup(category);

	for (auto prop : deleteProperties)
	{
		geometryEntity->getProperties().deleteProperty(prop, category);
	}
}

void ChamferEdges::updateShape(EntityGeometry* geometryEntity, TopoDS_Shape& shape)
{
	EntityPropertiesString* baseShapeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("baseShape"));
	assert(baseShapeProperty != nullptr);

	ot::UID baseShapeID = std::stoull(baseShapeProperty->getValue());

	std::list<ot::EntityInformation> entityInfo;
	modelComponent->getEntityInformation(ot::UIDList{ baseShapeID }, entityInfo);
	ot::UID baseShapeVersion = entityInfo.front().getVersion();

	EntityGeometry *baseGeometry = dynamic_cast<EntityGeometry*>(entityCache->getEntity(baseShapeID, baseShapeVersion));

	ot::UID brepID = baseGeometry->getBrepStorageObjectID();

	entityInfo.clear();
	modelComponent->getEntityInformation(ot::UIDList{ brepID }, entityInfo);
	ot::UID brepVersion = entityInfo.front().getVersion();

	EntityBrep* baseBrep = dynamic_cast<EntityBrep*>(entityCache->getEntity(brepID, brepVersion));

	performOperation(geometryEntity, baseBrep, shape);
}

void ChamferEdges::performOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, TopoDS_Shape& shape)
{
	std::list<ChamferEdgesData> edgeList = readEdgeListFromProperties(geometryEntity->getProperties());

	EntityPropertiesDouble* width = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Chamfer width"));
	double chamferWidth = width->getValue();

	if (chamferWidth == 0.0)
	{
		shape = baseBrep->getBrep();
	}
	else
	{
		BRepFilletAPI_MakeFillet  MF(baseBrep->getBrep());

		// add all the edges  to fillet 
		TopExp_Explorer  ex(baseBrep->getBrep(), TopAbs_EDGE);
		while (ex.More())
		{
			MF.Add(chamferWidth, TopoDS::Edge(ex.Current()));
			ex.Next();
		}

		shape = MF.Shape();
	}
}

bool ChamferEdges::removeFacesFromEntity(EntityGeometry *geometryEntity, ot::UID brepID, ot::UID brepVersion, std::list<ChamferEdgesData> &faces, std::list<ot::UID> &modifiedEntities)
{
	assert(geometryEntity != nullptr);

	EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brepID, brepVersion));
	if (brepEntity == nullptr) return false;

	TopoDS_Shape shape = brepEntity->getBrep();

	// Search for all the OCC faces in the entity and store them in the algorithm
	BRepAlgoAPI_Defeaturing algorithm;
	algorithm.SetShape(shape);
	algorithm.SetRunParallel(false);
	algorithm.SetToFillHistory(false);

	for (auto face : faces)
	{
		TopoDS_Shape result;
		if (findFaceFromPosition(shape, face.getX(), face.getY(), face.getZ(), result))
		{
			algorithm.AddFaceToRemove(result);
		}
	}

	if (!algorithm.FacesToRemove().IsEmpty())
	{
		// We have something do do (at least one face was selected
		algorithm.Build();

		if (!algorithm.IsDone())
		{
			// We have some errors, the operation did not complete
			std::stringstream errors;
			algorithm.DumpErrors(errors);

			std::cout << errors.str() << std::endl;
			return false;
		}

		if (algorithm.HasWarnings())
		{
			// We have some warnings
			std::stringstream warnings;
			algorithm.DumpWarnings(warnings);

			std::cout << warnings.str() << std::endl;
			return false;
		}

		// Replace the brep with the new one and facet the entity (this will require new ot::UIDs for brep and facet entities)
		geometryEntity->resetBrep();
		geometryEntity->resetFacets();

		geometryEntity->getBrepEntity()->setEntityID(modelComponent->createEntityUID());
		geometryEntity->getFacets()->setEntityID(modelComponent->createEntityUID());

		// Add the new brep entity to the geometry entity
		geometryEntity->setBrep(algorithm.Shape());

		// Facet the geometry entity and store it
		geometryEntity->facetEntity(false);

		geometryEntity->storeBrep();

		ot::UID brepStorageVersion = geometryEntity->getBrepEntity()->getEntityStorageVersion();

		entityCache->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		geometryEntity->storeFacets();

		ot::UID facetsStorageVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		geometryEntity->releaseFacets();

		modifiedEntities.push_back(geometryEntity->getEntityID());

		// Store the information about the entities such that they can be added to the model

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_UpdateGeometryEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, geometryEntity->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Brep, (unsigned long long) geometryEntity->getBrepStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Facets, (unsigned long long) geometryEntity->getFacetsStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_OverrideGeometry, false, doc.GetAllocator());	// The modified entity was not yet written 

		ot::PropertyGridCfg cfg;
		geometryEntity->getProperties().addToConfiguration(nullptr, false, cfg);

		ot::JsonObject cfgObj;
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_MODEL_NewProperties, cfgObj, doc.GetAllocator());

		std::string tmp;
		modelComponent->sendMessage(false, doc, tmp);
	}
	else
	{
		return false;
	}

	return true;
}

bool ChamferEdges::findFaceFromPosition(TopoDS_Shape &shape, double x, double y, double z, TopoDS_Shape &face)
{
	BRepBuilderAPI_MakeVertex vertex(gp_Pnt(x, y, z));

	TopoDS_Shape result;
	bool success = false;

	double minDist = 1e30;

	TopExp_Explorer exp;
	for (exp.Init(shape, TopAbs_SHELL); exp.More(); exp.Next())
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
					face = minimumDist.SupportOnShape1(hit);
				}
			}
		}
	}

	return success;
}

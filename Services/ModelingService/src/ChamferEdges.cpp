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

	// Here we do not perform an operation, but keep the shape as is. The user can later on set the chamfer size which will then update the shape and
	// perform the operation
	TopoDS_Shape shape = brepEntity->getBrep();

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
	geometryEntity->setTreeIcons("ChamferEdgesVisible", "ChamferEdgesHidden");
	//geometryEntity->getBrepEntity()->setFaceNameMap(resultFaceNames);

	geometryEntity->getProperties() = baseEntity->getProperties();

	deletePropertyCategory(geometryEntity, "Dimensions");
	deletePropertyCategory(geometryEntity, "Edges");
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

	storeEdgeListInProperties(edgeList, geometryEntity->getProperties());

	// The geometry entity has two children: brep and facets. We need to assign ids to both of them (since we do not have a model state object here)
	geometryEntity->getBrepEntity()->setEntityID(brepID);
	geometryEntity->getFacets()->setEntityID(facetsID);

	std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames = brepEntity->getFaceNameMap();
	geometryEntity->getBrepEntity()->setFaceNameMap(allFaceNames);

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

	// Request a view refresh and release the user interface
	uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());
	uiComponent->unlockUI(lockFlags);
}

void ChamferEdges::storeEdgeListInProperties(std::list<ChamferEdgesData> &edgeList, EntityProperties &properties)
{
	EntityPropertiesInteger::createProperty("Edges", "Number of edges", (int) edgeList.size(), "", properties);

	size_t edgeCounter = 1;
	for (auto edge : edgeList)
	{
		std::string index;
		index = "(" + std::to_string(edgeCounter) + ")";

		EntityPropertiesString::createProperty("Edges", "Edge name " + index, edge.getEdgeName(), "", properties);
		EntityPropertiesDouble::createProperty("Edges", "Position X " + index, edge.getX(), "", properties);
		EntityPropertiesDouble::createProperty("Edges", "Position Y " + index, edge.getY(), "", properties);
		EntityPropertiesDouble::createProperty("Edges", "Position Z " + index, edge.getZ(), "", properties);

		edgeCounter++;
	}
}

std::list<ChamferEdgesData> ChamferEdges::readEdgeListFromProperties(EntityProperties& properties)
{
	std::list<ChamferEdgesData> edgeList;

	EntityPropertiesInteger* edgeCountProperty = dynamic_cast<EntityPropertiesInteger*>(properties.getProperty("Number of edges"));

	if (edgeCountProperty != nullptr)
	{
		for (size_t edgeCounter = 1; edgeCounter <= edgeCountProperty->getValue(); edgeCounter++)
		{
			std::string index;
			index = "(" + std::to_string(edgeCounter) + ")";

			ChamferEdgesData data;
			
			EntityPropertiesString* edgeNameProperty = dynamic_cast<EntityPropertiesString*>(properties.getProperty("Edge name " + index));
			EntityPropertiesDouble* edgePosX = dynamic_cast<EntityPropertiesDouble*>(properties.getProperty("Position X " + index));
			EntityPropertiesDouble* edgePosY = dynamic_cast<EntityPropertiesDouble*>(properties.getProperty("Position Y " + index));
			EntityPropertiesDouble* edgePosZ = dynamic_cast<EntityPropertiesDouble*>(properties.getProperty("Position Z " + index));

			if (edgeNameProperty != nullptr && edgePosX != nullptr && edgePosY != nullptr && edgePosZ != nullptr)
			{
				data.setEdgeName(edgeNameProperty->getValue());
				data.setPosition(edgePosX->getValue(), edgePosY->getValue(), edgePosZ->getValue());

				edgeList.push_back(data);
			}
		}
	}

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

		std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames = baseBrep->getFaceNameMap();
		geometryEntity->getBrepEntity()->setFaceNameMap(allFaceNames);
	}
	else
	{
		// Store information about input shape's face names
		std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames;

		TopTools_ListOfShape anArguments;
		anArguments.Append(baseBrep->getBrep());

		TopExp_Explorer exp;
		for (exp.Init(baseBrep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(exp.Current());

			if (baseBrep->getFaceNameMap().count(aFace.TShape()) != 0)
			{
				allFaceNames[aFace.TShape()] = baseBrep->getFaceNameMap().at(aFace.TShape());
			}
		}

		// Perform the fillet operation
		BRepFilletAPI_MakeFillet  MF(baseBrep->getBrep());

		// add all the edges  to fillet 
		
		std::map< const opencascade::handle<TopoDS_TShape>, std::list<std::string>> allEdgeNames;

		for (exp.Init(baseBrep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(exp.Current());

			TopExp_Explorer expE;
			for (expE.Init(aFace, TopAbs_EDGE); expE.More(); expE.Next())
			{
				TopoDS_Edge aEdge = TopoDS::Edge(expE.Current());

				if (baseBrep->getFaceNameMap().count(aFace.TShape()) != 0)
				{
					allEdgeNames[aEdge.TShape()].push_back(baseBrep->getFaceNameMap().at(aFace.TShape()));
				}
			}
		}

		std::map< std::string, const opencascade::handle<TopoDS_TShape>> allEdges;

		std::map<const opencascade::handle<TopoDS_TShape>, std::string> allEdgesFace1;
		std::map<const opencascade::handle<TopoDS_TShape>, std::string> allEdgesFace2;

		for (auto edge : allEdgeNames)
		{
			if (edge.second.size() == 2)
			{
				std::string face1 = edge.second.front();
				std::string face2 = edge.second.back();

				std::string edgeName;

				if (face1 <= face2)
				{
					edgeName = face1 + ";" + face2;

					allEdgesFace1[edge.first] = face1;
					allEdgesFace2[edge.first] = face2;
				}
				else
				{
					edgeName = face2 + ";" + face1;

					allEdgesFace1[edge.first] = face2;
					allEdgesFace2[edge.first] = face1;
				}

				allEdges.emplace(edgeName, edge.first);
			}
		}

		std::map< const opencascade::handle<TopoDS_TShape>, std::string> allEdgesForChamfer;

		for (auto chamferEdge : edgeList)
		{
			if (!chamferEdge.getEdgeName().empty())
			{
				if (allEdges.count(chamferEdge.getEdgeName()) != 0)
				{
					allEdgesForChamfer.emplace(allEdges[chamferEdge.getEdgeName()], chamferEdge.getEdgeName());
				}
			}
		}

		TopTools_ListOfShape listOfChamferEdges;

		for (exp.Init(baseBrep->getBrep(), TopAbs_EDGE); exp.More(); exp.Next())
		{
			TopoDS_Edge edge = TopoDS::Edge(exp.Current());

			if (allEdgesForChamfer.count(edge.TShape()) != 0)
			{
				MF.Add(chamferWidth, TopoDS::Edge(exp.Current()));
				listOfChamferEdges.Append(exp.Current());
			}
		}

		try
		{
			shape = MF.Shape();
		}
		catch (Standard_Failure)
		{
			shape = baseBrep->getBrep();

			std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames = baseBrep->getFaceNameMap();
			geometryEntity->getBrepEntity()->setFaceNameMap(allFaceNames);

			uiComponent->displayErrorPrompt("ERROR: The chamfer edges operation has failed for shape: " + geometryEntity->getName());

			return;
		}

		// Apply the face names
		BRepTools_History aHistory(anArguments, MF);
		std::map< const opencascade::handle<TopoDS_TShape>, std::string> resultFaceNames;

		// First, we assign the names to all unchanged faces
		for (exp.Init(shape, TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(exp.Current());

			if (allFaceNames.count(aFace.TShape()) != 0)
			{
				resultFaceNames[aFace.TShape()] = allFaceNames[aFace.TShape()];
			}
		}

		// Now check for all modified faces
		for (exp.Init(baseBrep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(exp.Current());

			for (auto newFace : aHistory.Modified(aFace))
			{
				resultFaceNames[newFace.TShape()] = allFaceNames[aFace.TShape()];
			}
		}

		// Now we label all the faces generated from the edge blends
		for (auto edge : listOfChamferEdges)
		{
			TopoDS_Edge aEdge = TopoDS::Edge(edge);

			TopTools_ListOfShape listOfGeneratedFaces = aHistory.Generated(edge);

			for (auto face : listOfGeneratedFaces)
			{
				TopoDS_Face aFace = TopoDS::Face(face);

				std::string edgeName = allEdgesFace1[aEdge.TShape()] + ":" + allEdgesFace2[aEdge.TShape()];

				resultFaceNames[aFace.TShape()] = edgeName;
			}
		}
		
		geometryEntity->getBrepEntity()->setFaceNameMap(resultFaceNames);
	}
}

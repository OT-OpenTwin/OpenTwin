// @otlicense

#include "EdgesOperationBase.h"
#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "DataBase.h"
#include "EntityCache.h"
#include "UpdateManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "OTModelAPI/ModelServiceAPI.h"

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepAlgoAPI_Defeaturing.hxx"
#include <BRepFilletAPI_MakeFillet.hxx>

#include <map>
#include <set>

void EdgesOperationBase::enterSelectEdgesMode(void)
{
	std::map<std::string, std::string> options;

	uiComponent->enterEntitySelectionMode(ot::ModelServiceAPI::getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::EDGE, true, "", getSelectionAction(), getOperationDescription(), options, serviceID);
}

void EdgesOperationBase::addParametricProperty(EntityGeometry* geometryEntity, const std::string &name, double value)
{
	EntityPropertiesDouble* doubleProp = new EntityPropertiesDouble("#" + name, value);
	doubleProp->setVisible(false);
	geometryEntity->getProperties().createProperty(doubleProp, "Dimensions", true);

	EntityPropertiesString* stringProp = new EntityPropertiesString(name, std::to_string(value));
	geometryEntity->getProperties().createProperty(stringProp, "Dimensions", true);
}

void EdgesOperationBase::performOperation(const std::string &selectionInfo)
{
	ot::LockTypes lockFlags(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);
	uiComponent->lockUI(lockFlags);

	ot::JsonDocument doc;
	doc.fromJson(selectionInfo);

	ot::ConstJsonArray modelID = ot::json::getArray(doc, "modelID");
	ot::ConstJsonArray edgeName = ot::json::getArray(doc, "edgeName");

	// Get a list of all shapes which are involved in this operation for prefetching
	std::set<ot::UID> affectedEntitiesSet;
	std::list<EdgesData> edgeList;

	for (unsigned int i = 0; i < modelID.Size(); i++)
	{
		ot::UID entityID = modelID[i].GetUint64();
		
		affectedEntitiesSet.insert(entityID);

		EdgesData edgeData;
		edgeData.setEntityID(entityID);
		edgeData.setEdgeName(edgeName[i].GetString());
		edgeList.push_back(edgeData);
	}

	// Check whether all edges belong to the same entity
	if (affectedEntitiesSet.size() != 1)
	{
		uiComponent->displayErrorPrompt("All selected edges must belong to the same shape.");

		// Request a view refresh and release the user interface
		uiComponent->refreshSelection(ot::ModelServiceAPI::getCurrentVisualizationModelID());
		uiComponent->unlockUI(lockFlags);

		return;
	}

	// Now get the current versions for the entity
	std::list<ot::UID> affectedEntities(affectedEntitiesSet.begin(), affectedEntitiesSet.end());
	assert(affectedEntities.size() == 1);

	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation(affectedEntities, entityInfo);

	// Prefetch the geometry entity
	entityCache->prefetchEntities(entityInfo);

	// Now we prefetch all breps for these entities
	std::list<std::pair<ot::UID, ot::UID>> prefetchList;

	//entityVersionMap[entity.getID()] = entity.getVersion();

	EntityGeometry *baseEntity = dynamic_cast<EntityGeometry *>(entityCache->getEntity(entityInfo.front().getEntityID(), entityInfo.front().getEntityVersion()));
	ot::UID entityBrepID = baseEntity->getBrepStorageObjectID();

	std::list<ot::EntityInformation> entityBrepInfo;
	ot::ModelServiceAPI::getEntityInformation(std::list<ot::UID>{entityBrepID}, entityBrepInfo);

	// Prefetch the brep entity
	entityCache->prefetchEntities(entityBrepInfo);

	EntityBrep* brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(entityBrepInfo.front().getEntityID(), entityBrepInfo.front().getEntityVersion()));
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

	EntityGeometry* geometryEntity = new EntityGeometry(entityID, nullptr, nullptr, nullptr, serviceName);
	geometryEntity->setName(baseEntity->getName());
	geometryEntity->setEditable(true);
	geometryEntity->setSelectChildren(false);
	geometryEntity->setManageChildVisibility(false);
	geometryEntity->setBrep(shape);
	geometryEntity->setOldTreeIcons(getVisibleTreeItemName(), getHiddenTreeItemName());

	geometryEntity->getProperties() = baseEntity->getProperties();

	deleteNonStandardProperties(geometryEntity);

	addSpecificProperties(geometryEntity);

	EntityPropertiesString* baseShapeProperty = new EntityPropertiesString("baseShape", std::to_string(baseEntity->getEntityID()));
	baseShapeProperty->setVisible(false);
	geometryEntity->getProperties().createProperty(baseShapeProperty, "Internal");

	EntityPropertiesString* typeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("shapeType"));

	if (typeProperty != nullptr)
	{
		typeProperty->setValue(getShapeType());
	}
	else
	{
		typeProperty = new EntityPropertiesString("shapeType", getShapeType());
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
	geometryEntity->storeToDataBase();

	ot::UID entityVersion = geometryEntity->getEntityStorageVersion();
	ot::UID brepVersion   = geometryEntity->getBrepEntity()->getEntityStorageVersion();
	ot::UID facetsVersion = geometryEntity->getFacets()->getEntityStorageVersion();

	entityCache->cacheEntity(geometryEntity->getBrepEntity());
	geometryEntity->detachBrep();

	// Finally add the new entity to the model (data children need to come first)
	std::list<ot::UID> dataEntityIDList = { brepID, facetsID };
	std::list<ot::UID> dataEntityVersionList = { brepVersion , facetsVersion };
	std::list<ot::UID> dataEntityParentList = { entityID, entityID };

	ot::ModelServiceAPI::addGeometryOperation(entityID, entityVersion, baseEntity->getName(), dataEntityIDList, dataEntityVersionList, dataEntityParentList, entityNameList, "chamfer edges: " + geometryEntity->getName());

	delete geometryEntity;
	geometryEntity = nullptr;

	// Request a view refresh and release the user interface
	uiComponent->refreshSelection(ot::ModelServiceAPI::getCurrentVisualizationModelID());
	uiComponent->unlockUI(lockFlags);
}

void EdgesOperationBase::storeEdgeListInProperties(std::list<EdgesData> &edgeList, EntityProperties &properties)
{
	EntityPropertiesInteger::createProperty("Edges", "Number of edges", (int) edgeList.size(), "", properties);

	size_t edgeCounter = 1;
	for (auto edge : edgeList)
	{
		std::string index;
		index = "(" + std::to_string(edgeCounter) + ")";

		EntityPropertiesString::createProperty("Edges", "Edge name " + index, edge.getEdgeName(), "", properties);

		edgeCounter++;
	}
}

std::list<EdgesData> EdgesOperationBase::readEdgeListFromProperties(EntityProperties& properties)
{
	std::list<EdgesData> edgeList;

	EntityPropertiesInteger* edgeCountProperty = dynamic_cast<EntityPropertiesInteger*>(properties.getProperty("Number of edges"));

	if (edgeCountProperty != nullptr)
	{
		for (size_t edgeCounter = 1; edgeCounter <= edgeCountProperty->getValue(); edgeCounter++)
		{
			std::string index;
			index = "(" + std::to_string(edgeCounter) + ")";

			EdgesData data;
			
			EntityPropertiesString* edgeNameProperty = dynamic_cast<EntityPropertiesString*>(properties.getProperty("Edge name " + index));

			if (edgeNameProperty != nullptr)
			{
				data.setEdgeName(edgeNameProperty->getValue());
				edgeList.push_back(data);
			}
		}
	}

	return edgeList;
}

void EdgesOperationBase::updateShape(EntityGeometry* geometryEntity, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames)
{
	EntityPropertiesString* baseShapeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("baseShape"));
	assert(baseShapeProperty != nullptr);

	ot::UID baseShapeID = std::stoull(baseShapeProperty->getValue());

	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation(ot::UIDList{ baseShapeID }, entityInfo);
	ot::UID baseShapeVersion = entityInfo.front().getEntityVersion();

	EntityGeometry *baseGeometry = dynamic_cast<EntityGeometry*>(entityCache->getEntity(baseShapeID, baseShapeVersion));

	ot::UID brepID = baseGeometry->getBrepStorageObjectID();

	entityInfo.clear();
	ot::ModelServiceAPI::getEntityInformation(ot::UIDList{ brepID }, entityInfo);
	ot::UID brepVersion = entityInfo.front().getEntityVersion();

	EntityBrep* baseBrep = dynamic_cast<EntityBrep*>(entityCache->getEntity(brepID, brepVersion));

	performOperation(geometryEntity, baseBrep, shape, resultFaceNames);
}

void EdgesOperationBase::storeInputShapeFaceNames(EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allFaceNames)
{
	allFaceNames.clear();

	TopExp_Explorer exp;
	for (exp.Init(baseBrep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		if (baseBrep->getFaceNameMap().count(aFace.TShape()) != 0)
		{
			allFaceNames[aFace.TShape()] = baseBrep->getFaceNameMap().at(aFace.TShape());
		}
	}
}

void EdgesOperationBase::getAllEdgesFromInputShape(EntityBrep* baseBrep, std::map< std::string, const opencascade::handle<TopoDS_TShape>>& allEdges, 
											 std::map<const opencascade::handle<TopoDS_TShape>, std::string> &allEdgesFace1,
											 std::map<const opencascade::handle<TopoDS_TShape>, std::string> &allEdgesFace2)
{
	allEdges.clear();
	allEdgesFace1.clear();
	allEdgesFace2.clear();

	// In a first step, we get a list for all edge topologies containing their two adjacent face names
	std::map< const opencascade::handle<TopoDS_TShape>, std::list<std::string>> allEdgeNames;

	TopExp_Explorer exp;
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

	// In the second step, we determine the combined edge name and the names of the two neighboring faces
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
}

void EdgesOperationBase::getAllEdgesForOperation(std::list<EdgesData> &edgeList, std::map< std::string, const opencascade::handle<TopoDS_TShape>> &allEdges, std::map< const opencascade::handle<TopoDS_TShape>, std::string> &allEdgesForOperation)
{
	allEdgesForOperation.clear();
	
	for (auto selectedEdge : edgeList)
	{
		if (!selectedEdge.getEdgeName().empty())
		{
			if (allEdges.count(selectedEdge.getEdgeName()) != 0)
			{
				allEdgesForOperation.emplace(allEdges[selectedEdge.getEdgeName()], selectedEdge.getEdgeName());
			}
		}
	}
}

void EdgesOperationBase::performOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames)
{
	std::list<EdgesData> edgeList = readEdgeListFromProperties(geometryEntity->getProperties());

	shape = baseBrep->getBrep();

	std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames = baseBrep->getFaceNameMap();
	geometryEntity->getBrepEntity()->setFaceNameMap(allFaceNames);

	if (operationActive(geometryEntity))
	{
		// Store information about input shape's face names
		storeInputShapeFaceNames(baseBrep, allFaceNames);

		// Get all edges and their names from the input shape
		std::map< std::string, const opencascade::handle<TopoDS_TShape>> allEdges;
		std::map<const opencascade::handle<TopoDS_TShape>, std::string> allEdgesFace1;
		std::map<const opencascade::handle<TopoDS_TShape>, std::string> allEdgesFace2;

		getAllEdgesFromInputShape(baseBrep, allEdges, allEdgesFace1, allEdgesFace2);

		// Get all edges for which the operation shall be applied
		std::map< const opencascade::handle<TopoDS_TShape>, std::string> allEdgesForOperation;
		
		getAllEdgesForOperation(edgeList, allEdges, allEdgesForOperation);

		// Perform the actual operation on the selected edges
		TopTools_ListOfShape listOfProcessedEdges;
		BRepTools_History* history = nullptr;

		if (!performActualOperation(geometryEntity, baseBrep, allEdgesForOperation, shape, listOfProcessedEdges, history))
		{
			shape = baseBrep->getBrep();

			std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames = baseBrep->getFaceNameMap();
			geometryEntity->getBrepEntity()->setFaceNameMap(allFaceNames);

			uiComponent->displayErrorPrompt("ERROR: The " + getOperationDescription() + " edges operation has failed for shape: " + geometryEntity->getName());

			return;
		}

		// Apply the face names
		resultFaceNames.clear();

		// First, we assign the names to all unchanged faces
		TopExp_Explorer exp;
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

			for (auto newFace : history->Modified(aFace))
			{
				resultFaceNames[newFace.TShape()] = allFaceNames[aFace.TShape()];
			}
		}

		// Now we label all the faces generated from the edge operations
		for (auto edge : listOfProcessedEdges)
		{
			TopoDS_Edge aEdge = TopoDS::Edge(edge);

			TopTools_ListOfShape listOfGeneratedFaces = history->Generated(edge);

			for (auto face : listOfGeneratedFaces)
			{
				TopoDS_Face aFace = TopoDS::Face(face);

				std::string edgeName = allEdgesFace1[aEdge.TShape()] + ":" + allEdgesFace2[aEdge.TShape()];

				resultFaceNames[aFace.TShape()] = edgeName;
			}
		}

		// Now we label all the faces generated from an operation on a vertex
		for (exp.Init(baseBrep->getBrep(), TopAbs_VERTEX); exp.More(); exp.Next())
		{
			TopoDS_Vertex aVertex = TopoDS::Vertex(exp.Current());

			TopTools_ListOfShape listOfGeneratedFaces = history->Generated(aVertex);

			for (auto face : listOfGeneratedFaces)
			{
				TopoDS_Face aFace = TopoDS::Face(face);

				if (resultFaceNames.count(aFace.TShape()) == 0)
				{
					// Now search the edges for the vertex and therefore generate a list of all adjacent faces
					std::list<opencascade::handle<TopoDS_TShape>> allEdgesForVertex;
					getAllEdgesForVertex(baseBrep, aVertex, allEdgesForOperation, allEdgesForVertex);

					std::string vertexName = getVertexNameFromEdges(allEdgesForVertex, allEdgesFace1, allEdgesFace2);

					resultFaceNames[aFace.TShape()] = vertexName;
				}
			}
		}

		delete history;
		history = nullptr;
		
		geometryEntity->getBrepEntity()->setFaceNameMap(resultFaceNames);
	}
	else
	{
		resultFaceNames = allFaceNames; // We did not change the shape
	}
}

void EdgesOperationBase::getAllEdgesForVertex(EntityBrep* baseBrep, TopoDS_Vertex& aVertex, std::map<const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, std::list<opencascade::handle<TopoDS_TShape>>& allEdgesForVertex)
{
	TopExp_Explorer exp;
	for (exp.Init(baseBrep->getBrep(), TopAbs_EDGE); exp.More(); exp.Next())
	{
		TopoDS_Edge aEdge = TopoDS::Edge(exp.Current());

		if (allEdgesForOperation.count(aEdge.TShape()) > 0)
		{
			// This is an edge which is used for the operation, so we check whether the edge has the vertex we are searching for
			TopExp_Explorer expE;
			for (expE.Init(aEdge, TopAbs_VERTEX); expE.More(); expE.Next())
			{
				TopoDS_Vertex aEdgeVertex = TopoDS::Vertex(expE.Current());

				if (aEdgeVertex.TShape() == aVertex.TShape())
				{
					// This edge has the searched for vertex
					allEdgesForVertex.push_back(aEdge.TShape());
					break;
				}
			}
		}
	}
}

std::string EdgesOperationBase::getVertexNameFromEdges(std::list<opencascade::handle<TopoDS_TShape>>& allEdgesForVertex, std::map<const opencascade::handle<TopoDS_TShape>, std::string> &allEdgesFace1, std::map<const opencascade::handle<TopoDS_TShape>, std::string> &allEdgesFace2)
{
	std::set<std::string> faceNames;
	
	for (auto edge : allEdgesForVertex)
	{
		faceNames.emplace(allEdgesFace1[edge]);
		faceNames.emplace(allEdgesFace2[edge]);
	}

	std::string vertexName;

	for (auto face : faceNames)
	{
		if (vertexName.empty())
		{
			vertexName = face;

		}
		else
		{
			vertexName += ":" + face;
		}
	}

	return vertexName;
}

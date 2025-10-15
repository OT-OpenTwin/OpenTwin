#include "MeshWriter.h"

#include "Application.h"
#include "FaceAnnotationsManager.h"
#include "ProgressLogger.h"
#include "TmpFileManager.h"

#include "BoundingBox.h"
#include "GeometryOperations.h"
#include "EntityMeshTet.h"
#include "EntityMeshTetData.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetNodes.h"
#include "EntityMeshTetFaceData.h"
#include "EntityMeshTetItem.h"
#include "EntityMeshTetItemDataTets.h"
#include "EntityMeshTetItemDataTetedges.h"
#include "EntityAnnotation.h"
#include "EntityAnnotationData.h"
#include "EntityMeshTetInfo.h"
#include "EntityBinaryData.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <gmsh.h_cwrap>

#include <sstream>

MeshWriter::MeshWriter(Application *app, EntityMeshTet *mesh) : 
	application(app),
	entityMesh(mesh),
	totalNumberNodes(0),
	totalNumberTriangles(0),
	totalNumberTets(0)
{
}

void MeshWriter::convertAndStoreNodes(void)
{
	// The nodes will be stored by the mesh entity in an array. 
	// In addition, we build a map which allows us getting the corresponding index for a given node tag

	std::vector<size_t> nTags;
	std::vector<double> nCoords;
	std::vector<double> parametricCoord;

	gmsh::model::mesh::getNodes(nTags, nCoords, parametricCoord, -1, -1, false);

	// The tags should be ordered sequentially such that we are not loosing much memory here by using a vector
	size_t maxTag = 0;
	for (size_t i = 0; i < nTags.size(); i++) maxTag = std::max(maxTag, nTags[i]);

	nodeTagToNodeIndexMap.resize(maxTag + 1);

	totalNumberNodes = nTags.size();

	entityMesh->setNumberOfNodes(totalNumberNodes);

	for (size_t i = 0; i < totalNumberNodes; i++)
	{
		entityMesh->setNodeCoordX(i, nCoords[3 * i]);
		entityMesh->setNodeCoordY(i, nCoords[3 * i + 1]);
		entityMesh->setNodeCoordZ(i, nCoords[3 * i + 2]);

		nodeTagToNodeIndexMap[nTags[i]] = i;
	}

	// Now we store the nodes and add it to the list of new data entities
	EntityMeshTetNodes *meshNodes = entityMesh->getMeshData()->getMeshNodes();
	meshNodes->setEntityID(application->getModelComponent()->createEntityUID());
	entityMesh->getMeshData()->storeMeshNodes();

	application->getModelComponent()->addNewDataEntity(meshNodes->getEntityID(), meshNodes->getEntityStorageVersion(), entityMesh->getMeshData()->getEntityID());
}

void MeshWriter::convertFaces(void)
{
	// Get all faces from the mesh
	gmsh::vectorpair faceTags;
	gmsh::model::getEntities(faceTags, 2);

	// Loop though all faces tags, get the elements of the face and create a new EntityMeshTetFace data object for the face
	for (auto faceTag : faceTags)
	{
		assert(faceTag.first >= 0);
		assert(faceTag.second >= 0);

		std::vector<int> elementTypes2;
		std::vector<std::vector<size_t>> elementTags2;
		std::vector<std::vector<size_t>> nodeTags2;

		// Get all triangles for the current face
		gmsh::model::mesh::getElements(elementTypes2, elementTags2, nodeTags2, faceTag.first, faceTag.second);

		bool faceInverted = (faceTag.second < 0);
		assert(!faceInverted);

		// Create a new mesh face object to store the triangles
		EntityMeshTetFace *meshFace = new EntityMeshTetFace(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, application->getServiceName());

		meshFace->setSurfaceId(faceTag.second);

		BoundingBox faceBoundingBox;

		if (elementTypes2.size() == 1) // For this implementation, we expect all triangles to be of the same type (mixed types are not supported yet)
		{
			// Now we store the nodes of the face
			int numberOfNodes = getNumberOfNodesFromElementType(elementTypes2[0]);
			
			// Store the edges of the first triangle from this face as reference for this face
			storeReferenceTriangleEdges(elementTypes2[0], nodeTags2[0], faceTag.second);

			if (numberOfNodes > 0)
			{
				// Check and store the surface mesh triangles
				assert(elementTypes2.size() == 1);
				assert(elementTags2.size() == 1);
				assert(nodeTags2.size() == 1);

				size_t numberTriangles = elementTags2[0].size();

				meshFace->setNumberTriangles(numberTriangles);
				totalNumberTriangles += numberTriangles;

				size_t nodeArrayIndex = 0;

				for (size_t i = 0; i < numberTriangles; i++)
				{
					meshFace->getTriangle(i).setNumberOfNodes(numberOfNodes);

					for (int index = 0; index < numberOfNodes; index++)
					{
						size_t nodeIndex = nodeTagToNodeIndexMap[nodeTags2[0][nodeArrayIndex]];
						nodeArrayIndex++;

						meshFace->getTriangle(i).setNode(index, nodeIndex);

						faceBoundingBox.extend(entityMesh->getNodeCoordX(nodeIndex), entityMesh->getNodeCoordY(nodeIndex), entityMesh->getNodeCoordZ(nodeIndex));

						if (i == 0)
						{
							// We now store 
						}
					}
				}
			}
		}

		// Get all nodes which are on the boundary of the face
		std::vector<size_t> nodeFaceBoundaryTagsOrder1;
		gmsh::model::mesh::getElementEdgeNodes(2, nodeFaceBoundaryTagsOrder1, faceTag.second, true);

		std::vector<size_t> nodeFaceBoundaryTagsOrder2;
		gmsh::model::mesh::getElementEdgeNodes(9, nodeFaceBoundaryTagsOrder2, faceTag.second, true);

		std::vector<size_t> nodeFaceBoundaryTagsOrder3;
		gmsh::model::mesh::getElementEdgeNodes(21, nodeFaceBoundaryTagsOrder3, faceTag.second, true);

		assert(faceTagToBoundaryNodeTagsMap.count(faceTag.second) == 0);

		std::vector<size_t> nodeFaceBoundaryTags = nodeFaceBoundaryTagsOrder1;
		nodeFaceBoundaryTags.insert(nodeFaceBoundaryTags.end(), nodeFaceBoundaryTagsOrder2.begin(), nodeFaceBoundaryTagsOrder2.end());
		nodeFaceBoundaryTags.insert(nodeFaceBoundaryTags.end(), nodeFaceBoundaryTagsOrder3.begin(), nodeFaceBoundaryTagsOrder3.end());

		faceTagToBoundaryNodeTagsMap[faceTag.second] = nodeFaceBoundaryTags;

		meshFace->setBoundingBox(faceBoundingBox);

		// Set the face object for the mesh entity
		entityMesh->setFace(faceTag.second, meshFace);
	}

	// Assign a new entity id to the mesh data faces object (which was implicitly created when adding the faces above)
	entityMesh->getMeshData()->getMeshFaces()->setEntityID(application->getModelComponent()->createEntityUID());
}

void MeshWriter::storeFaces(void)
{
	// Now we store all mesh faces in the data base (IDs are already assigned to all objects)
	entityMesh->getMeshData()->storeMeshFaces();
	application->getModelComponent()->addNewDataEntity(entityMesh->getMeshData()->getMeshFaces()->getEntityID(), 
													entityMesh->getMeshData()->getMeshFaces()->getEntityStorageVersion(), 
													entityMesh->getMeshData()->getEntityID());

	for (auto face : entityMesh->getMeshData()->getMeshFaces()->getAllFaces())
	{
		application->getModelComponent()->addNewDataEntity(face.second->getEntityID(), 
														face.second->getEntityStorageVersion(), 
														entityMesh->getMeshData()->getMeshFaces()->getEntityID());
	}
}

void MeshWriter::storeMeshEntity(const std::string &entityName, EntityBase *entity, int entityTag, gmsh::vectorpair &entityFaceTags, bool isBackgroundMeshEntity, 
								 std::vector<std::list<size_t>> &nodeTagToTetIndexMap, std::map<int, gmsh::vectorpair> &volumeTagToFacesMap,
								const std::string &materialsFolder, ot::UID materialsFolderID,
								 FaceAnnotationsManager *faceAnnotationsManager, ProgressLogger *progressLogger)
{
	EntityMeshTetItem *meshItem = new EntityMeshTetItem(0, nullptr, nullptr, nullptr, application->getServiceName());
	meshItem->setName(entityName);
	meshItem->setMesh(entityMesh);

	allMeshItems.push_back(meshItem);

	for (int i = 0; i < nodeTagToTetIndexMap.size(); i++) nodeTagToTetIndexMap[i].clear();

	bool tetsSuccessfullyCreated = false;

	// In a first step, we store all tets for this entity
	try
	{
		// Get the mesh data for the current volume
		std::vector<int> elementTypes3;
		std::vector<std::vector<size_t>> elementTags3;
		std::vector<std::vector<size_t>> nodeTags3;

		gmsh::model::mesh::getElements(elementTypes3, elementTags3, nodeTags3, 3, entityTag);

		// Check and store the volume mesh 
		if (elementTypes3.size() == 1) // For this implementation, we expect all tetrahedrons to be of the same type (mixed types are not supported yet)
		{
			int numberOfNodes = getNumberOfNodesFromElementType(elementTypes3[0]);

			if (numberOfNodes > 0)
			{
				assert(elementTags3.size() == 1);
				assert(elementTags3.size() == 1);
				assert(nodeTags3.size() == 1);

				size_t numberTets = elementTags3[0].size();

				meshItem->setNumberTets(numberTets);
				totalNumberTets += numberTets;

				size_t nodeArrayIndex = 0;

				for (size_t i = 0; i < numberTets; i++)
				{
					meshItem->setNumberOfTetNodes(i, numberOfNodes);

					for (int index = 0; index < numberOfNodes; index++)
					{
						size_t nodeIndex = nodeTagToNodeIndexMap[nodeTags3[0][nodeArrayIndex]];
						nodeArrayIndex++;

						meshItem->setTetNode(i, index, nodeIndex);
						nodeTagToTetIndexMap[nodeIndex].push_back(i);
					}
				}

				tetsSuccessfullyCreated = true;
			}
		}
	}
	catch (std::string)
	{
		application->getUiComponent()->displayMessage("ERROR: No tetrahedrons generated for shape: " + entityName + "\n");
	}
	catch (int)
	{
	}

	// Now we try to get all the internal tet edges and store them in the entity 
	// Here we consider all the internal edges to be straight edges, so we consider only the first and the last points.

	try
	{
		std::vector<size_t> nodeEdgeTagsOrder1, nodeEdgeTagsOrder2, nodeEdgeTagsOrder3;
		gmsh::model::mesh::getElementEdgeNodes( 4, nodeEdgeTagsOrder1, entityTag, true);
		gmsh::model::mesh::getElementEdgeNodes(11, nodeEdgeTagsOrder2, entityTag, true);
		gmsh::model::mesh::getElementEdgeNodes(29, nodeEdgeTagsOrder3, entityTag, true);

		if (nodeEdgeTagsOrder1.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder1, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}

		if (nodeEdgeTagsOrder2.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder2, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}

		if (nodeEdgeTagsOrder3.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder3, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}
	}
	catch (std::string)
	{
		// We have already reported the error above
	}
	catch (int)
	{
	}

	// The next step is to write the information about the mesh faces to the entity
	size_t numberFaces = entityFaceTags.size();
	meshItem->setNumberFaces(numberFaces);

	// Check for any invalid face meshes
	checkForInvalidFaceMeshes(entityName, entityFaceTags, progressLogger);

	// Finally, store the references to the object's faces in the mesh item (with proper orientation)
	for (int nF = 0; nF < numberFaces; nF++)
	{
		int faceTag = volumeTagToFacesMap[entityTag][nF].second;
		bool sameOrientation = !tetsSuccessfullyCreated ? true : checkFaceAndVolumeForSameOrientation(faceTag, meshItem, nodeTagToTetIndexMap);

		meshItem->setFace(nF, (sameOrientation ? 1 : -1) * entityFaceTags[nF].second);

		for (auto annotationID : faceAnnotationsManager->getAnnotationIDsforFace(faceTag))
		{
			entityMesh->getFace(faceTag)->addAnnotation(sameOrientation, annotationID);
		}
	}

	// Append the error string if no tets were created for this entity
	if (!tetsSuccessfullyCreated && entity != nullptr)
	{
		meshElementsWithoutTets += "     " + entity->getName() + "\n";
	}

	// Copy all properties from the "Solver" property grop (including the material) and set them as editable
	copySolverProperties(entity->getProperties(), meshItem->getProperties());

	// Assign the material information for the entity (if missing)
	EntityPropertiesEntityList *material = dynamic_cast<EntityPropertiesEntityList*>(meshItem->getProperties().getProperty("Material"));

	if (material == nullptr)
	{
		EntityPropertiesEntityList::createProperty("Solver", "Material", materialsFolder, materialsFolderID, "", -1, "", meshItem->getProperties());
	}

	// Now create the display (color) information for the entity
	EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(entity->getProperties().getProperty("Color"));
	assert(color != nullptr);

	int colorR = (int)(color->getColorR() * 255 + 0.5);
	int colorG = (int)(color->getColorG() * 255 + 0.5);
	int colorB = (int)(color->getColorB() * 255 + 0.5);

	EntityPropertiesColor::createProperty("Appearance", "Color", { colorR, colorG, colorB }, "", meshItem->getProperties());

	meshItem->setColor(color->getColorR(), color->getColorG(), color->getColorB());
	meshItem->setInitiallyHidden(true);

	// Finally, we store the new entity

	// Update the statistics of the mesh entity
	meshItem->updateStatistics();

	// Assign entities to the new object and its childs
	meshItem->setEntityID(application->getModelComponent()->createEntityUID());

	if (meshItem->getMeshDataTets() != nullptr)
	{
		meshItem->getMeshDataTets()->setEntityID(application->getModelComponent()->createEntityUID());
	}

	if (meshItem->getMeshDataTetEdges() != nullptr)
	{
		meshItem->getMeshDataTetEdges()->setEntityID(application->getModelComponent()->createEntityUID());
	}

	// Store the new object and its childs
	meshItem->storeToDataBase();

	// and finally add the new entities to the lists
	application->getModelComponent()->addNewTopologyEntity(meshItem->getEntityID(), meshItem->getEntityStorageVersion(), !isBackgroundMeshEntity);

	if (meshItem->getMeshDataTets() != nullptr)
	{
		application->getModelComponent()->addNewDataEntity(meshItem->getMeshDataTets()->getEntityID(),
														meshItem->getMeshDataTets()->getEntityStorageVersion(),
														meshItem->getEntityID());
	}

	if (meshItem->getMeshDataTetEdges() != nullptr)
	{
		application->getModelComponent()->addNewDataEntity(meshItem->getMeshDataTetEdges()->getEntityID(),
														meshItem->getMeshDataTetEdges()->getEntityStorageVersion(),
														meshItem->getEntityID());
	}
}

void MeshWriter::copySolverProperties(EntityProperties& source, EntityProperties& dest)
{
	std::list<std::string> solverProperties = source.getListOfPropertiesForGroup("Solver");

	for (auto prop : solverProperties)
	{
		EntityPropertiesBase* item = source.getProperty(prop)->createCopy();
		item->setReadOnly(false);

		dest.createProperty(item, "Solver");
	}
}

bool MeshWriter::checkFaceAndVolumeForSameOrientation(int faceTag, EntityMeshTetItem *meshItem, std::vector<std::list<size_t>> &nodeToTetMap)
{
	bool sameOrientation = false;

	// Get the orientation from the first triangle. Here we only need to check the cornerpoints of the triangle, since curvatures are not relevant
	// for the orientation

	size_t triangleNodes[3];
	triangleNodes[0] = entityMesh->getFace(faceTag)->getTriangle(0).getNode(0);
	triangleNodes[1] = entityMesh->getFace(faceTag)->getTriangle(0).getNode(1);
	triangleNodes[2] = entityMesh->getFace(faceTag)->getTriangle(0).getNode(2);

	// Now we loop through the list of all tets which are connected to the first node
	for (auto tet : nodeToTetMap[triangleNodes[0]])
	{
		// Find which tet has a common face to the boundary face (Here we only need to check the cornerpoints of the tets, since curvatures are not
		// relevant for the orientation

		size_t tetNodes[4];
		tetNodes[0] = meshItem->getTetNode(tet, 0);
		tetNodes[1] = meshItem->getTetNode(tet, 1);
		tetNodes[2] = meshItem->getTetNode(tet, 2);
		tetNodes[3] = meshItem->getTetNode(tet, 3);

		if (   (triangleNodes[0] == tetNodes[0] || triangleNodes[0] == tetNodes[2] || triangleNodes[0] == tetNodes[3])
			&& (triangleNodes[1] == tetNodes[0] || triangleNodes[1] == tetNodes[2] || triangleNodes[1] == tetNodes[3])
			&& (triangleNodes[2] == tetNodes[0] || triangleNodes[2] == tetNodes[2] || triangleNodes[2] == tetNodes[3]))
		{
			// We have a common face with the tetrahedrons 0-3-2 face

			sameOrientation = checkTrianglesForSameOrientation(triangleNodes[0], triangleNodes[1], triangleNodes[2], tetNodes[0], tetNodes[3], tetNodes[2]);

			break;
		}
		else if (   (triangleNodes[0] == tetNodes[1] || triangleNodes[0] == tetNodes[2] || triangleNodes[0] == tetNodes[3])
			     && (triangleNodes[1] == tetNodes[1] || triangleNodes[1] == tetNodes[2] || triangleNodes[1] == tetNodes[3])
			     && (triangleNodes[2] == tetNodes[1] || triangleNodes[2] == tetNodes[2] || triangleNodes[2] == tetNodes[3]))
		{
			// We have a common face with the tetrahedrons 1-2-3 face

			sameOrientation = checkTrianglesForSameOrientation(triangleNodes[0], triangleNodes[1], triangleNodes[2], tetNodes[1], tetNodes[2], tetNodes[3]);

			break;
		}
		else if (   (triangleNodes[0] == tetNodes[1] || triangleNodes[0] == tetNodes[2] || triangleNodes[0] == tetNodes[0])
			     && (triangleNodes[1] == tetNodes[1] || triangleNodes[1] == tetNodes[2] || triangleNodes[1] == tetNodes[0])
			     && (triangleNodes[2] == tetNodes[1] || triangleNodes[2] == tetNodes[2] || triangleNodes[2] == tetNodes[0]))
		{
			// We have a common face with the tetrahedrons 2-1-0 face

			sameOrientation = checkTrianglesForSameOrientation(triangleNodes[0], triangleNodes[1], triangleNodes[2], tetNodes[2], tetNodes[1], tetNodes[0]);

			break;
		}
		else if (   (triangleNodes[0] == tetNodes[0] || triangleNodes[0] == tetNodes[1] || triangleNodes[0] == tetNodes[3])
			     && (triangleNodes[1] == tetNodes[0] || triangleNodes[1] == tetNodes[1] || triangleNodes[1] == tetNodes[3])
			     && (triangleNodes[2] == tetNodes[0] || triangleNodes[2] == tetNodes[1] || triangleNodes[2] == tetNodes[3]))
		{
			// We have a common face with the tetrahedrons 0-1-3 face

			sameOrientation = checkTrianglesForSameOrientation(triangleNodes[0], triangleNodes[1], triangleNodes[2], tetNodes[0], tetNodes[1], tetNodes[3]);

			break;
		}
	}

	return sameOrientation;
}

bool MeshWriter::checkTrianglesForSameOrientation(size_t t0, size_t t1, size_t t2, size_t u0, size_t u1, size_t u2)
{
	if (t0 == u0 && t1 == u1 && t2 == u2) return true;
	if (t0 == u1 && t1 == u2 && t2 == u0) return true;
	if (t0 == u2 && t1 == u0 && t2 == u1) return true;

	return false;
}

void MeshWriter::storeInternalTetEdges(std::vector<size_t> &nodeVolumeTags, std::map<size_t, std::vector<size_t>> &nodeBoundaryTags, EntityMeshTetItem *meshItem, std::vector<size_t> &nodeTagToIdMap)
{
	// First we determine the maximum number of a point id
	size_t maxPoint = 0;
	for (auto id : nodeVolumeTags) maxPoint = std::max(maxPoint, id);

	// Now we reserve an array which is large enough to store all node tags
	std::vector<std::list<size_t>> volumeEdges;
	volumeEdges.resize(maxPoint+1);

	// Loop through all edges and store the edge in the array. We always store the lower point id first.
	for (size_t index = 0; index < nodeVolumeTags.size() - 1; index += 2)
	{
		size_t n1 = nodeVolumeTags[index  ];
		size_t n2 = nodeVolumeTags[index+1];

		if (n1 > n2)
		{
			if (std::find(std::begin(volumeEdges[n2]), std::end(volumeEdges[n2]), n1) == std::end(volumeEdges[n2]))
			{
				volumeEdges[n2].push_back(n1);
			}
		}
		else
		{
			if (std::find(std::begin(volumeEdges[n1]), std::end(volumeEdges[n1]), n2) == std::end(volumeEdges[n1]))
			{
				volumeEdges[n1].push_back(n2);
			}
		}
	}

	// Now we remove all the edges which are located at the boundary
	for (auto &faceBoundaryTags : nodeBoundaryTags)
	{
		for (size_t index = 0; index < faceBoundaryTags.second.size() - 1; index += 2)
		{
			size_t n1 = faceBoundaryTags.second[index  ];
			size_t n2 = faceBoundaryTags.second[index+1];

			if (n1 <= maxPoint && n2 <= maxPoint) // We need to ensure that the boundary point is on the current entity
			{
				if (n1 > n2)
				{
					auto pos = std::find(std::begin(volumeEdges[n2]), std::end(volumeEdges[n2]), n1);

					if (pos != std::end(volumeEdges[n2]))
					{
						// We need to remove this edge
						volumeEdges[n2].erase(pos);
					}
				}
				else
				{
					auto pos = std::find(std::begin(volumeEdges[n1]), std::end(volumeEdges[n1]), n2);

					if (pos != std::end(volumeEdges[n1]))
					{
						// We need to remove this edge
						volumeEdges[n1].erase(pos);
					}
				}
			}
		}
	}

	// Finally count the total number of edges
	size_t numberOfEdges = 0;
	for (size_t index = 0; index < volumeEdges.size(); index++)
	{
		numberOfEdges += volumeEdges[index].size();
	}

	// Any store the edges in the entity
	meshItem->setNumberTetEdges(numberOfEdges);

	size_t edgeIndex = 0;
	for (size_t index = 0; index < volumeEdges.size(); index++)
	{
		for (auto n2 : volumeEdges[index])
		{
			size_t nodes[2];
			nodes[0] = nodeTagToIdMap[index];
			nodes[1] = nodeTagToIdMap[n2];

			meshItem->setTetEdgeNodes(edgeIndex, nodes);
			edgeIndex++;
		}
	}
}

void MeshWriter::checkForInvalidFaceMeshes(const std::string &entityName, gmsh::vectorpair &entityFaceTags, ProgressLogger *progressLogger)
{
	bool invalidFaceMesh = false;
	std::string invalidFaceList;

	std::string prefix = entityMesh->getName() + "/Mesh";
	std::string plainName = entityName.substr(prefix.length());

	for (auto face : entityFaceTags)
	{
		int faceTag = face.second;

		if (progressLogger->isInvalidSurface(faceTag))
		{
			bool faceIsValid = false;

			// This faceId is part of the error list
			// This might have been corrected through the meshing operation meanwhile. Therefore, we now check
			// whether the face itself has triangles associated and the associated volumes all have tetrahedrons. If this
			// is the case, the error was fixed already and does not need to be reported here.
			std::vector<int> elementTypesT;
			std::vector<std::vector<size_t>> elementTagsT;
			std::vector<std::vector<size_t>> nodeTagsT;

			gmsh::model::mesh::getElements(elementTypesT, elementTagsT, nodeTagsT, 2, faceTag);
			if (elementTypesT.size() > 0)
			{
				// We have found triangles for the face
				std::vector<int> volumes, edges;
				gmsh::model::getAdjacencies(2, faceTag, volumes, edges);

				faceIsValid = true;
				// Now we check whether each attached volume has tetrahedrons
				for (auto vol : volumes)
				{
					gmsh::model::mesh::getElements(elementTypesT, elementTagsT, nodeTagsT, 3, vol);
					if (elementTypesT.size() == 0)
					{
						// We have an associated volume without tetrahedrons
						faceIsValid = false;
					}
				}
			}

			if (!faceIsValid)
			{
				invalidFaceMesh = true;

				if (!invalidFaceList.empty()) invalidFaceList += ", " + std::to_string(faceTag);
				else invalidFaceList = std::to_string(faceTag);

				// Create an error annotation
				EntityAnnotation *annotation = new EntityAnnotation(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, application->getServiceName());
				annotation->getAnnotationData()->setEntityID(application->getModelComponent()->createEntityUID());

				std::vector<int> elementTypes2;
				std::vector<std::vector<size_t>> elementTags2;
				std::vector<std::vector<size_t>> nodeTags2;

				gmsh::model::mesh::getElements(elementTypes2, elementTags2, nodeTags2, 2, faceTag);

				if (elementTypes2.size() > 0)
				{
					size_t numberTriangles = elementTags2[0].size();

					if (nodeTags2[0].size() == 3 * numberTriangles)
					{
						size_t triangleNodes[3];
						for (size_t i = 0; i < numberTriangles; i++)
						{
							triangleNodes[0] = nodeTagToNodeIndexMap[nodeTags2[0][3 * i]];
							triangleNodes[1] = nodeTagToNodeIndexMap[nodeTags2[0][3 * i + 1]];
							triangleNodes[2] = nodeTagToNodeIndexMap[nodeTags2[0][3 * i + 2]];

							annotation->addTriangle(
								entityMesh->getNodeCoordX(triangleNodes[0]), entityMesh->getNodeCoordY(triangleNodes[0]), entityMesh->getNodeCoordZ(triangleNodes[0]),
								entityMesh->getNodeCoordX(triangleNodes[1]), entityMesh->getNodeCoordY(triangleNodes[1]), entityMesh->getNodeCoordZ(triangleNodes[1]),
								entityMesh->getNodeCoordX(triangleNodes[2]), entityMesh->getNodeCoordY(triangleNodes[2]), entityMesh->getNodeCoordZ(triangleNodes[2]),
								1.0, 0.65, 0.0);
						}
					}
				}

				if (progressLogger->hasInvalidSurfacePoint(faceTag))
				{
					// An error point was stored for this face
					std::vector<std::string> pointList = progressLogger->getInvalidSurfacePoints(faceTag);

					for (auto point : pointList)
					{
						double x = 0.0, y = 0.0, z = 0.0;

						if (getPointCoords(point, x, y, z))
						{
							annotation->addPoint(x, y, z, 1.0, 0.0, 0.0);

							std::array<double, 3> point{ x, y, z };

							if (errorPoints.end() == std::find(errorPoints.begin(), errorPoints.end(), point))
							{
								errorPoints.push_back(point);
							}
						}
					}
				}
				else if (elementTypes2.size() == 0)
				{
					// We have no information about the error entity.
					// Find a point in the center of the bounding box and project it onto the entity

					double xmin{ 0.0 }, ymin{ 0.0 }, zmin{ 0.0 }, xmax{ 0.0 }, ymax{ 0.0 }, zmax{ 0.0 };
					gmsh::model::getBoundingBox(2, faceTag, xmin, ymin, zmin, xmax, ymax, zmax);

					std::vector<double> boxCenter = { 0.5 * (xmin + xmax) , 0.5 * (ymin + ymax) , 0.5 * (zmin + zmax) };
					std::vector<double> pointOnFace;
					std::vector<double> paramOnFace;

					try
					{
						gmsh::model::getClosestPoint(2, faceTag, boxCenter, pointOnFace, paramOnFace);
					}
					catch (...)
					{
						// We can not determine the closes point on the face. Therefore we will just use the bounding box center
						pointOnFace = boxCenter;
					}

					annotation->addPoint(pointOnFace[0], pointOnFace[1], pointOnFace[2], 1.0, 0.0, 0.0);
				}

				annotation->setName(entityMesh->getName() + "/Mesh Errors" + plainName + "/Error surface " + std::to_string(faceTag));
				annotation->setInitiallyHidden(true);
				annotation->storeToDataBase();

				application->getModelComponent()->addNewTopologyEntity(annotation->getEntityID(), annotation->getEntityStorageVersion(), true);
				application->getModelComponent()->addNewDataEntity(annotation->getAnnotationData()->getEntityID(), 
																annotation->getAnnotationData()->getEntityStorageVersion(), 
																annotation->getEntityID());			
			}
		}
	}

	if (invalidFaceMesh)
	{
		application->getUiComponent()->displayMessage("ERROR: Invalid face mesh for shape: " + entityName + " (" + invalidFaceList + ")\n");
	}
}

bool MeshWriter::getPointCoords(std::string point, double &x, double &y, double &z)
{
	for (size_t i = 0; i < point.size(); i++) if (point[i] == '(' || point[i] == ')' || point[i] == ',') point[i] = ' ';

	std::stringstream sspoint(point);

	sspoint >> x >> y >> z;

	return true;
}

void MeshWriter::displayShapesWithoutTets(void)
{
	if (meshElementsWithoutTets != "")
	{
		application->getUiComponent()->displayMessage("  Mesh objects without tetrahedrons: \n" + meshElementsWithoutTets + "\n\n");
	}
}

void MeshWriter::analyzeErrorPoints(BoundingBox *boundingBox)
{
	std::string message;

	double tolerance = boundingBox->getDiagonal() * 1e-5;

	// Check all mesh items and search for the surfaces intersecting with the points

	for (auto point : errorPoints)
	{
		double x = point[0];
		double y = point[1];
		double z = point[2];

		// Now check for all mesh items whether the point intersects
		std::list<int> intersectingSurfaces;

		for (auto meshItem : allMeshItems)
		{
			getIntersectingSurfaces(meshItem, x, y, z, intersectingSurfaces, tolerance);
		}

		if (intersectingSurfaces.size() > 0)
		{
			std::stringstream ssline;
			ssline << "The point (" << x << ", " << y << ", " << z << ") intersects with the following surfaces: ";
			for (auto surface : intersectingSurfaces)
			{
				ssline << surface << ", ";
			}
			std::string line = ssline.str();
			message += line.substr(0, line.size() - 2) + "\n";
		}
	}

	if (!message.empty())
	{
		application->getUiComponent()->displayMessage(message);
	}
}

void MeshWriter::getIntersectingSurfaces(EntityMeshTetItem *meshItem, double x, double y, double z, std::list<int> &intersectingSurfaces, double tolerance)
{
	// Check whether the point is inside the bounding box of the mesh item
	if (!meshItem->getBoundingBox().testPointInside(x, y, z, tolerance)) return;

	// Now check the intersection for each face
	for (size_t nF = 0; nF < meshItem->getNumberFaces(); nF++)
	{
		if (getFaceIntersectsPoint(meshItem->getFace(nF), x, y, z, entityMesh, tolerance))
		{
			if (intersectingSurfaces.end() == std::find(intersectingSurfaces.begin(), intersectingSurfaces.end(), meshItem->getFace(nF)->getSurfaceId()))
			{
				intersectingSurfaces.push_back(meshItem->getFace(nF)->getSurfaceId());
			}
		}
	}
}

bool MeshWriter::getFaceIntersectsPoint(EntityMeshTetFace *face, double x, double y, double z, EntityMeshTet *mesh, double tolerance)
{
	if (!face->getBoundingBox().testPointInside(x, y, z, tolerance)) return false;

	// Now check each triangle for an intersection with the point

	for (size_t nT = 0; nT < face->getNumberTriangles(); nT++)
	{
		size_t nodes[3];

		// Here we only consider the intersection of the non-curved triangle with the point. We assume that the curving has not been performed in case of a meshing error.
		nodes[0] = face->getTriangle(nT).getNode(0);
		nodes[1] = face->getTriangle(nT).getNode(1);
		nodes[2] = face->getTriangle(nT).getNode(2);

		double nodeCoord1[3], nodeCoord2[3], nodeCoord3[3];

		mesh->getNodeCoords(nodes[0], nodeCoord1);
		mesh->getNodeCoords(nodes[1], nodeCoord2);
		mesh->getNodeCoords(nodes[2], nodeCoord3);

		if (GeometryOperations::checkPointInTriangle(x, y, z, nodeCoord1, nodeCoord2, nodeCoord3, tolerance))
		{
			return true;
		}
	}

	return false;
}

bool MeshWriter::storeMeshFile(void)
{
	// We first need to create a temporary working directory. Afterward, we ask gmsh to store the mesh file into this directory. 
	// Next we read the file from there and write it into the database
	// Finally we delete the temporary directory again

	// Create the temporary directory
	std::string tmpDir = TmpFileManager::createTmpDir();
	std::string meshFileName = tmpDir + "\\out.msh";

	// Save the msh file
	gmsh::option::setNumber("Mesh.Binary", 0);  // For better compatibility
	gmsh::write(meshFileName);

	// Save the file to the database
	EntityBinaryData *fileData = new EntityBinaryData(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, application->getServiceName());

	// Read the file content to the entity
	readMeshFile(meshFileName, fileData);

	// Store the data and add it to the data model
	fileData->storeToDataBase();

	application->getModelComponent()->addNewDataEntity(fileData->getEntityID(),
													fileData->getEntityStorageVersion(),
													entityMesh->getMeshData()->getEntityID());

	entityMesh->getMeshData()->setGmshDataStorageId(fileData->getEntityID());

	// Delete the temp dir
	TmpFileManager::deleteDirectory(tmpDir);

	return true;
}

void MeshWriter::readMeshFile(const std::string &meshFileName, EntityBinaryData *fileData)
{
	// open the file
	std::ifstream file(meshFileName, std::ios::binary);
	file.unsetf(std::ios::skipws);

	// Determine the file size
	std::streampos file_size;
	file.seekg(0, std::ios::end);
	file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Allocate a buffer for reading the file
	char *buffer = new char[file_size];

	// Read the file content for the buffer
	file.read(buffer, file_size);

	// Now store the buffer 
	fileData->setData(buffer, file_size);

	// Clear the buffer again
	delete[] buffer;
	buffer = nullptr;
}

int MeshWriter::getNumberOfNodesFromElementType(int type)
{
	int numberOfNodes = 0;

	switch (type)
	{
	case 2:  // First order triangle
		numberOfNodes = 3;
		break;
	case 9:  // Second order triangle
		numberOfNodes = 6;
		break;
	case 21: // Third order triangle
		numberOfNodes = 10;
		break;
	case 4:  // First order tetrahedron
		numberOfNodes = 4;
		break;
	case 11: // Second order tetrahedron
		numberOfNodes = 10;
		break;
	case 29: // Third order tetrahedron
		numberOfNodes = 20;
		break;
	default:
		assert(0); // Unknown element type
	}

	return numberOfNodes;
}

void MeshWriter::storeMeshEntityFromPhysicalGroup(const std::string& entityName, int entityTag, double colorR, double colorG, double colorB, const std::string& materialsFolder, ot::UID materialsFolderID)
{
	// Create a new mesh entity
	EntityMeshTetItem* meshItem = new EntityMeshTetItem(0, nullptr, nullptr, nullptr, application->getServiceName());
	meshItem->setName(entityName);
	meshItem->setMesh(entityMesh);

	allMeshItems.push_back(meshItem);

	bool tetsSuccessfullyCreated = false;

	std::list<int> neighbourFaces;

	// In a first step, we store all tets for this entity
	try
	{
		// Now we get all 3D mesh elements (tetrahedrons) for this entity
		std::vector<int> elementTypes3;
		std::vector<std::vector<size_t>> elementTags3;
		std::vector<std::vector<size_t>> nodeTags3;

		gmsh::model::mesh::getElements(elementTypes3, elementTags3, nodeTags3, 3, entityTag);

		// Check and store the volume mesh 
		if (elementTypes3.size() == 1) // For this implementation, we expect all tetrahedrons to be of the same type (mixed types are not supported yet)
		{
			int numberOfNodes = getNumberOfNodesFromElementType(elementTypes3[0]);

			if (numberOfNodes > 0)
			{
				assert(elementTags3.size() == 1);
				assert(elementTags3.size() == 1);
				assert(nodeTags3.size() == 1);

				size_t numberTets = elementTags3[0].size();

				meshItem->setNumberTets(numberTets);
				totalNumberTets += numberTets;

				size_t nodeArrayIndex = 0;

				determineConnectedFaces(numberOfNodes, nodeTags3[0], neighbourFaces);

				for (size_t i = 0; i < numberTets; i++)
				{
					meshItem->setNumberOfTetNodes(i, numberOfNodes);

					for (int index = 0; index < numberOfNodes; index++)
					{
						size_t nodeIndex = nodeTagToNodeIndexMap[nodeTags3[0][nodeArrayIndex]];
						nodeArrayIndex++;

						meshItem->setTetNode(i, index, nodeIndex);
					}
				}

				tetsSuccessfullyCreated = true;
			}
		}
	}
	catch (std::string)
	{
		application->getUiComponent()->displayMessage("ERROR: No tetrahedrons available for shape: " + entityName + "\n");
	}
	catch (int)
	{
	}

	// Now we try to get all the internal tet edges and store them in the entity 
	// Here we consider all the internal edges to be straight edges, so we consider only the first and the last points.

	try
	{
		std::vector<size_t> nodeEdgeTagsOrder1, nodeEdgeTagsOrder2, nodeEdgeTagsOrder3;
		gmsh::model::mesh::getElementEdgeNodes(4, nodeEdgeTagsOrder1, entityTag, true);
		gmsh::model::mesh::getElementEdgeNodes(11, nodeEdgeTagsOrder2, entityTag, true);
		gmsh::model::mesh::getElementEdgeNodes(29, nodeEdgeTagsOrder3, entityTag, true);

		if (nodeEdgeTagsOrder1.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder1, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}

		if (nodeEdgeTagsOrder2.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder2, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}

		if (nodeEdgeTagsOrder3.size() > 0)
		{
			// Store the internal edges of the tetrahedons
			storeInternalTetEdges(nodeEdgeTagsOrder3, faceTagToBoundaryNodeTagsMap, meshItem, nodeTagToNodeIndexMap);
		}
	}
	catch (std::string)
	{
		// We have already reported the error above
	}
	catch (int)
	{
	}

	// The next step is to write the information about the mesh faces to the entity
	size_t numberFaces = neighbourFaces.size();
	meshItem->setNumberFaces(numberFaces);

	// Finally, store the references to the object's faces in the mesh item (with proper orientation)
	size_t nF = 0;
	for (auto faceTag : neighbourFaces)
	{
		meshItem->setFace(nF, faceTag);
		nF++;
	}

	EntityPropertiesEntityList::createProperty("Solver", "Material", materialsFolder, materialsFolderID, "", -1, "", meshItem->getProperties());

	// Now create the display (color) information for the entity
	EntityPropertiesColor::createProperty("Appearance", "Color", { (int) (colorR * 255 + 0.5),  (int)(colorG * 255 + 0.5),  (int)(colorB * 255 + 0.5) }, "", meshItem->getProperties());

	meshItem->setColor(colorR, colorG, colorB);
	meshItem->setInitiallyHidden(true);

	// Finally, we store the new entity

	// Update the statistics of the mesh entity
	meshItem->updateStatistics();

	// Assign entities to the new object and its childs
	meshItem->setEntityID(application->getModelComponent()->createEntityUID());

	if (meshItem->getMeshDataTets() != nullptr)
	{
		meshItem->getMeshDataTets()->setEntityID(application->getModelComponent()->createEntityUID());
	}

	if (meshItem->getMeshDataTetEdges() != nullptr)
	{
		meshItem->getMeshDataTetEdges()->setEntityID(application->getModelComponent()->createEntityUID());
	}

	// Store the new object and its childs
	meshItem->storeToDataBase();

	// and finally add the new entities to the lists
	application->getModelComponent()->addNewTopologyEntity(meshItem->getEntityID(), meshItem->getEntityStorageVersion(), true);

	if (meshItem->getMeshDataTets() != nullptr)
	{
		application->getModelComponent()->addNewDataEntity(meshItem->getMeshDataTets()->getEntityID(),
			meshItem->getMeshDataTets()->getEntityStorageVersion(),
			meshItem->getEntityID());
	}

	if (meshItem->getMeshDataTetEdges() != nullptr)
	{
		application->getModelComponent()->addNewDataEntity(meshItem->getMeshDataTetEdges()->getEntityID(),
			meshItem->getMeshDataTetEdges()->getEntityStorageVersion(),
			meshItem->getEntityID());
	}
}

void MeshWriter::storeReferenceTriangleEdges(int triangleType, std::vector<size_t> &triangleNodeTags, size_t faceTag)
{
	// We always store the primary nodes only. 
	std::list<size_t> nodeList;

	nodeList.push_back(triangleNodeTags[0]);
	nodeList.push_back(triangleNodeTags[1]);
	nodeList.push_back(triangleNodeTags[2]);
	
	faceReferenceTriangleMap[faceTag] = nodeList;
}

void MeshWriter::determineConnectedFaces(int numberOfNodes, std::vector<size_t>& tetPoints, std::list<int> &neighbourFaces)
{	
	// Loop through all tets and check for each face whether it is the reference triangle of a face (forward or backward)
	for (size_t index = 0; index < tetPoints.size() - 1; index += numberOfNodes)
	{
		size_t n0 = tetPoints[index];
		size_t n1 = tetPoints[index + 1];
		size_t n2 = tetPoints[index + 2];
		size_t n3 = tetPoints[index + 3];

		int face1 = getNeighborFaceForTriangle(n0, n1, n3);
		int face2 = getNeighborFaceForTriangle(n1, n2, n3);
		int face3 = getNeighborFaceForTriangle(n0, n3, n2);
		int face4 = getNeighborFaceForTriangle(n0, n2, n1);

		if (face1 != 0) neighbourFaces.push_back(face1);
		if (face2 != 0) neighbourFaces.push_back(face2);
		if (face3 != 0) neighbourFaces.push_back(face3);
		if (face4 != 0) neighbourFaces.push_back(face4);
	}
}

int MeshWriter::getNeighborFaceForTriangle(size_t n0, size_t n1, size_t n2)
{
	for (auto& face : faceReferenceTriangleMap)
	{
		size_t faceTag = face.first;
		auto point = face.second.begin();

		size_t n[3];
		int nP = 0;

		assert(face.second.size() == 3);

		for (auto point : face.second)
		{
			n[nP] = point;
			nP++;
		}

		if (trianglesAreSame(n0, n1, n2, n[0], n[1], n[2]))
		{
			return (int) faceTag;
		}

		if (trianglesAreSame(n0, n2, n1, n[0], n[1], n[2]))
		{
			return -1 * (int) faceTag;
		}
	}

	return 0;
}

bool MeshWriter::trianglesAreSame(size_t a0, size_t a1, size_t a2, size_t b0, size_t b1, size_t b2)
{
	if (a0 == b0 && a1 == b1 && a2 == b2) return true;
	if (a1 == b0 && a2 == b1 && a0 == b2) return true;
	if (a2 == b0 && a0 == b1 && a1 == b2) return true;

	return false;
}

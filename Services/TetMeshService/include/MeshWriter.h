// @otlicense

#pragma once

class Application;
class FaceAnnotationsManager;
class ProgressLogger;
class EntityAnnotation;

class BoundingBox;
class EntityMeshTet;
class EntityMeshTetFace;
class EntityMeshTetItem;
class EntityBase;
class EntityBinaryData;
class EntityProperties;

#include <vector>
#include <map>
#include <list>
#include <array>
#include <string>

#include "OldTreeIcon.h"

namespace gmsh
{
	typedef std::vector<std::pair<int, int> > vectorpair;
}

class MeshWriter
{
public:
	MeshWriter(Application *app, EntityMeshTet *mesh);
	~MeshWriter() {};

	void convertAndStoreNodes(void);
	void convertFaces(void);
	void storeFaces(void);
	bool storeMeshFile(void);

	void storeMeshEntity(const std::string &entityName, EntityBase *entity, int entityTag, gmsh::vectorpair &entityFaceTags, bool isBackgroundMeshEntity, 
						 std::vector<std::list<size_t>> &nodeTagToTetIndexMap, std::map<int, gmsh::vectorpair> &volumeTagToFacesMap, 
						 const std::string& materialsFolder, ot::UID materialsFolderID,
						 FaceAnnotationsManager *faceAnnotationsManager, ProgressLogger *progressLogger);
	void storeMeshEntityFromPhysicalGroup(const std::string& entityName, int entityTag, double colorR, double colorG, double colorB, const std::string& materialsFolder, ot::UID materialsFolderID);

	size_t getNumberOfNodes(void) { return totalNumberNodes; }
	size_t getNumberOfTriangles(void) { return totalNumberTriangles; }
	size_t getNumberOfTets(void) { return totalNumberTets; }

	void displayShapesWithoutTets(void);
	void analyzeErrorPoints(BoundingBox *boundingBox);

private:
	void storeInternalTetEdges(std::vector<size_t> &nodeVolumeTags, std::map<size_t, std::vector<size_t>> &nodeBoundaryTags, EntityMeshTetItem *meshItem, std::vector<size_t> &nodeTagToIdMap);
	bool checkFaceAndVolumeForSameOrientation(int faceTag, EntityMeshTetItem *meshItem, std::vector<std::list<size_t>> &nodeToTetMap);
	bool checkTrianglesForSameOrientation(size_t t0, size_t t1, size_t t2, size_t u0, size_t u1, size_t u2);
	void checkForInvalidFaceMeshes(const std::string &entityName, gmsh::vectorpair &entityFaceTags, ProgressLogger *progressLogger);
	bool getPointCoords(std::string point, double &x, double &y, double &z);
	void getIntersectingSurfaces(EntityMeshTetItem *meshItem, double x, double y, double z, std::list<int> &intersectingSurfaces, double tolerance);
	bool getFaceIntersectsPoint(EntityMeshTetFace *face, double x, double y, double z, EntityMeshTet *mesh, double tolerance);
	void readMeshFile(const std::string &meshFileName, EntityBinaryData *fileData);
	int	 getNumberOfNodesFromElementType(int type);
	void storeReferenceTriangleEdges(int triangleType, std::vector<size_t>& triangleNodeTags, size_t faceTag);
	void determineConnectedFaces(int numberOfNodes, std::vector<size_t>& tetPoints, std::list<int>& neighbourFaces);
	int getNeighborFaceForTriangle(size_t n0, size_t n1, size_t n2);
	bool trianglesAreSame(size_t a0, size_t a1, size_t a2, size_t b0, size_t b1, size_t b2);
	void copySolverProperties(EntityProperties& source, EntityProperties& dest);

	Application *application;
	EntityMeshTet *entityMesh;

	size_t totalNumberNodes;
	size_t totalNumberTriangles;
	size_t totalNumberTets;
	std::string meshElementsWithoutTets;
	std::vector<size_t> nodeTagToNodeIndexMap;
	std::map<size_t, std::vector<size_t>> faceTagToBoundaryNodeTagsMap;
	std::list<EntityMeshTetItem *> allMeshItems;
	std::list<std::array<double, 3>> errorPoints;
	std::map<size_t, std::list<size_t>> faceReferenceTriangleMap;
};

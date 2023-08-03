#pragma once

class Application;
class ModelState;
class EntityBase;
class EntityMeshCartesian;
class EntityMeshCartesianData;
class EntityGeometry;
class ClassFactory;
class EntityMeshCartesianFace;
class CartesianMeshTree;
class EntityFacetData;
class EntityMeshCartesianNodes;
class EntityCartesianVector;
class EntityMaterial;

#include "Geometry.h"
#include "EntityResultBase.h"

#include <vector>
#include <string>
#include <list>
#include <map>
#include <ctime>
#include <algorithm>

#include <embree3/rtcore.h>

class CartesianMeshMaterial
{
public:
	CartesianMeshMaterial() : isPEC(false), epsilon(1.0), mu(1.0), sigma(0.0), priority(0.0) {};
	virtual ~CartesianMeshMaterial() {};

	void setIsPEC(bool flag) { isPEC = flag; }
	void setEpsilon(double value) { epsilon = value; }
	void setMu(double value) { mu = value; }
	void setSigma(double value) { sigma = value; }

	void setPriority(double value) { priority = value; }

	bool   getIsPEC(void) { return isPEC; }
	double getEpsilon(void) { return epsilon; }
	double getMu(void) { return mu; }
	double getSigma(void) { return sigma; }

	double getPriority(void) { return priority; }

private:
	bool isPEC;
	double epsilon;
	double mu;
	double sigma;

	double priority;
};

class CartesianMeshFillFront
{
public:
	CartesianMeshFillFront() : ix(0), iy(0), iz(0) {}
	virtual ~CartesianMeshFillFront() {}

	unsigned int getX(void) { return ix; }
	unsigned int getY(void) { return iy; }
	unsigned int getZ(void) { return iz; }

	void setX(unsigned int x) { ix = x; }
	void setY(unsigned int y) { iy = y; }
	void setZ(unsigned int z) { iz = z; }

private:
	unsigned int ix;
	unsigned int iy;
	unsigned int iz;
};

class CartesianMeshBoundaryFacets
{
public: 
	CartesianMeshBoundaryFacets() : faceEntity(nullptr) {};
	~CartesianMeshBoundaryFacets() {};

	void addFacet(int direction, long long index) { facets[direction].push_back(index); }
	std::list<long long> &getFacetList(int direction) { return facets[direction]; }

	void setFaceEntity(EntityMeshCartesianFace *entity) { faceEntity = entity; }
	EntityMeshCartesianFace *getFaceEntity(void) { return faceEntity; }

private:
	std::list<long long> facets[3];
	EntityMeshCartesianFace *faceEntity;
};

class CartesianMeshBoundaryFaceConformal
{
public: 
	CartesianMeshBoundaryFaceConformal() : numberOfPoints(0), pointIndices { 0 }, cellIndexFront(-1), cellIndexBack(-1) {}
	~CartesianMeshBoundaryFaceConformal() {}

	void setPoint(int point, size_t pointIndex) { pointIndices[point] = pointIndex; }
	size_t getPoint(int point) { return pointIndices[point]; }
	long long getCellIndexFront(void) { return cellIndexFront; }
	long long getCellIndexBack(void) { return cellIndexBack; }

	bool isValid(void) { return numberOfPoints > 2; }
	int getNumberOfPoints(void) { return numberOfPoints; }

	void removeUnecessaryPoints(void)
	{
		int index = 1;
		while (index < numberOfPoints)
		{
			if (pointIndices[index] == pointIndices[index - 1])
			{
				// Two subsequent points are the same
				for (int index2 = index; index2 < numberOfPoints - 1; index2++)
				{
					pointIndices[index2] = pointIndices[index2 + 1];
				}

				numberOfPoints--;
			}
			else
			{
				index++;
			}
		}

		if (numberOfPoints > 1)
		{
			if (pointIndices[numberOfPoints-1] == pointIndices[0])
			{
				// The last point is a duplicate
				numberOfPoints--;
			}
		}
	}

	void setPoints(size_t p0, size_t p1, size_t p2, size_t p3) { numberOfPoints = 4; pointIndices[0] = p0;  pointIndices[1] = p1;  pointIndices[2] = p2;  pointIndices[3] = p3; }
	void setFrontBackCellIndex(long long front, long long back) { cellIndexFront = front; cellIndexBack = back; }
		
private:
	int numberOfPoints;
	size_t pointIndices[4];
	long long cellIndexFront, cellIndexBack;
};

class CartesianMeshBoundaryFacetsConformal
{
public: 
	CartesianMeshBoundaryFacetsConformal() : faceEntity(nullptr) {};
	~CartesianMeshBoundaryFacetsConformal() {};

	void addFacet(CartesianMeshBoundaryFaceConformal &face) { facets.push_back(face); }
	std::list<CartesianMeshBoundaryFaceConformal> &getFacetList(void) { return facets; }

	void setFaceEntity(EntityMeshCartesianFace *entity) { faceEntity = entity; }
	EntityMeshCartesianFace *getFaceEntity(void) { return faceEntity; }

private:
	std::list<CartesianMeshBoundaryFaceConformal> facets;
	EntityMeshCartesianFace *faceEntity;
};

struct CartesianMeshIntersectionData 
{
	unsigned int hitCount;
	float lastDistance;
	unsigned int lastPrimID;
};

class CartesianMeshBoundaryPointData
{
public:
	CartesianMeshBoundaryPointData() : ix(0), iy(0), iz(0), fixed(false) { for (int i = 0; i < 6; i++) intersect[i] = FLT_MAX; }
	~CartesianMeshBoundaryPointData() {}

	void resetIntersections(void) { for (int i = 0; i < 6; i++) intersect[i] = FLT_MAX; } 

	void setIndex(int _ix, int _iy, int _iz) { ix = _ix; iy = _iy; iz = _iz; }
	int getIndexX(void) { return ix; }
	int getIndexY(void) { return iy; }
	int getIndexZ(void) { return iz; }

	void setFixed(void) { fixed = true; }
	bool isFixed(void) { return fixed; }

	// Directions are interpreted as follows:
	// 0: negative x direction
	// 1: positive x direction
	// 2: negative y direction
	// 3: positive y direction
	// 4: negative z direction
	// 5: positive z direction

	bool hasIntersection(int direction) { return intersect[direction] != FLT_MAX; }

	void setIntersection(int direction, float coord) { intersect[direction] = coord; }
	float getIntersection(int direction) { return intersect[direction]; }

	bool hasAnyIntersection(void) { for (int i = 0; i < 6; i++) if (intersect[i] != FLT_MAX) return true; return false; }

	void attachFacet(CartesianMeshBoundaryFaceConformal *facet) { if (std::find(facets.begin(), facets.end(), facet) == facets.end()) facets.push_back(facet); };
	void removeFacet(CartesianMeshBoundaryFaceConformal *facet) { facets.remove(facet); };

	std::list<CartesianMeshBoundaryFaceConformal *> &getFacetList(void) { return facets; }

private:	
	int ix, iy, iz;
	float intersect[6];
	bool fixed;
	std::list<CartesianMeshBoundaryFaceConformal *> facets;
};

class vector3
{
public:
	vector3() : x(0.0), y(0.0), z(0.0) {};
	~vector3() {};

	void setX(float v) { x = v; }
	void setY(float v) { y = v; }
	void setZ(float v) { z = v; }

	float getX(void) const { return x; }
	float getY(void) const { return y; }
	float getZ(void) const { return z; }

	float dot(const vector3 &other) const { return x * other.x + y * other.y + z * other.z; }

	vector3 operator+(const vector3 &other) const
	{
		vector3 result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		return result;
	}

	vector3 operator-(const vector3 &other) const
	{
		vector3 result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		return result;
	}

private:
	float x, y, z;
};

vector3 operator*(float lhs, const vector3 &rhs);

class MeshGeometryItem
{
public:
	MeshGeometryItem() {};
	~MeshGeometryItem() {};

	void setPriority(double p) { priority = p; }
	void setGeometryItem(EntityGeometry *g) { geometryItem = g; }

	double getPriority(void) { return priority; }
	EntityGeometry *getGeometryItem(void) { return geometryItem; }

	bool operator>(const MeshGeometryItem &other) { return priority > other.priority; }
	bool operator<(const MeshGeometryItem &other) { return priority < other.priority; }
	bool operator==(const MeshGeometryItem &other) { return priority == other.priority; }

private:
	double priority;
	EntityGeometry *geometryItem = nullptr;
};

class CartesianMeshCreation
{
public:
	CartesianMeshCreation();
	virtual ~CartesianMeshCreation();

	void updateMesh(Application *app, EntityBase *meshEntity);

private:
	Application *getApplication(void) { return application; }
	void setApplication(Application *app) { application = app; }
	EntityMeshCartesian *getEntityMesh(void) { return entityMesh; }
	void setEntityMesh(EntityMeshCartesian *mesh) { entityMesh = mesh; }
	void deleteMesh(void);
	void reportTime(const std::string &message, std::time_t &timer);
	std::list<ot::UID> getAllGeometryEntitiesForMeshing(void);
	EntityMeshCartesianData *determineMeshLines(const std::list<EntityBase *> &meshEntities, double maximumEdgeLength, double stepsAlongDiagonalProperty);
	void determineBoundingBoxExtension(double deltaX, double deltaY, double deltaZ, double &offsetXmin, double &offsetXmax, double &offsetYmin, double &offsetYmax, double &offsetZmin, double &offsetZmax);
	void determineMeshLinesOneDirection(double min, double max, double step, std::vector<double> &coords);
	EntityGeometry *addBackgroundCubeTopology(void);
	void addBackgroundCubeGeometry(EntityGeometry *backgroundCube, EntityMeshCartesianData *meshData, double stepWidth);
	void determineVolumeFill(std::list<EntityGeometry *> &geometryEntities, EntityMeshCartesianData *meshData, bool conformalMeshing);
	void fillSingleShape(EntityGeometry *shape, std::vector<EntityGeometry *> &meshFill, std::vector<char> &shapeFill, EntityMeshCartesianData *meshData, 
						 std::map<EntityGeometry*, double> &shapePriorities, std::map<std::pair<EntityGeometry*, EntityGeometry*>, bool> &overlappingShapes,
						 std::vector<CartesianMeshFillFront> &fillFrontElements, std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation);
	void extractInterfaces(std::vector<EntityGeometry *> meshFill, EntityMeshCartesianData *meshData,
						   std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets);
	void extractInterfacesW(std::vector<EntityGeometry *> meshFill, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets,
							size_t nu, size_t nv, size_t nw, size_t mu, size_t mv, size_t mw, int dirW);
	void addBoundaryFacet(int direction, size_t facetIndex, EntityGeometry *from, EntityGeometry *to, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets);
	void storeBoundaryFaces(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets);
	void storeBoundaryFacesList(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets, EntityMeshCartesianData *meshData);
	void fillCell(EntityGeometry *shape, std::vector<EntityGeometry *> &meshFill, size_t index, 
				  std::map<EntityGeometry*, double> &shapePriorities, std::map<std::pair<EntityGeometry*, EntityGeometry*>, bool> &overlappingShapes);
	void fillSingleShapeVolume(size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill, EntityFacetData *facetData,
							   std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
							   std::vector<CartesianMeshFillFront> &fillFrontElements);
	bool findUndefinedCell(size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill, unsigned int &ix, unsigned int &iy, unsigned int &iz);
	void fillNeighbouringVolume(unsigned int ix, unsigned int iy, unsigned int iz, char fillCode, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill,std::vector<CartesianMeshFillFront> &fillFrontElements);
	void updateFront(std::vector<CartesianMeshFillFront> &fillFrontElements, size_t &nFrontElements, char fillCode, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill);
	void expandFront(unsigned int ix, unsigned int iy, unsigned int iz, std::vector<CartesianMeshFillFront> &fillFrontElements, size_t &nFrontElements, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill);
	bool testPointInside(double xpos, double ypos, double zpos);
	void setupEmbree(EntityFacetData *facetData, bool multipleIntersections);
	void terminateEmbree(void);
	std::string getLargeNumberString(size_t number);
	void extractAndStoreMesh(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData, std::time_t &timer, std::list<EntityGeometry *> &geometryEntities);
	void createMatrices(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData);
	void extractInterfacesConformal(std::vector<EntityGeometry *> meshFill, EntityMeshCartesianData *meshData,
									std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets);
	void extractInterfacesWConformal(std::vector<EntityGeometry *> meshFill, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets,
									 size_t nu, size_t nv, size_t nw, size_t mu, size_t mv, size_t mw, int dirW);
	void addBoundaryFacetConformal(int direction, size_t iu, size_t iv, size_t iw, size_t mu, size_t mv, size_t mw, EntityGeometry *from, EntityGeometry *to, size_t indexFrom, size_t indexTo, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets);
	void storeBoundaryFacesConformal(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets);
	void storeBoundaryFacesListConformal(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets, EntityMeshCartesianData *meshData);
	void createNodes(unsigned int nx, unsigned int ny, unsigned int nz, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets, EntityMeshCartesianData *meshData,
				     CartesianMeshBoundaryPointData *&boundaryPointInfo);
	void extractAndStoreMeshConformal(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData, std::time_t &timer, std::list<EntityGeometry *> &geometryEntities, 
									  std::map<EntityGeometry*, double> shapePriorities, std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> &triangleInCellInformation);
	bool movePointToClosestPointOnGeometry(size_t pointIndex, CartesianMeshBoundaryPointData *boundaryPointInfo, int nx, int ny, int nz, std::vector<EntityGeometry *> &meshFill, EntityGeometry *geometryEntity, EntityMeshCartesianNodes *meshNodes, EntityFacetData *facets, std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> &triangleInCellInformation);
	float clamp(float value, float min, float max);
	vector3 closestPointOnTriangle(const vector3 *triangle, const vector3 &sourcePosition);
	void determine2DFill(std::list<EntityGeometry *> &geometryEntities, EntityMeshCartesianData *meshData);
	void setProgress(int percentage);
	void displayMessage(std::string message);
	void setProgressInformation(std::string message, bool continuous);
	void closeProgressInformation(void);
	EntityCartesianVector* getDsMatrix(void) { return matrixDs; };
	EntityCartesianVector* getDualDsMatrix(void) { return matrixDualDs; };
	EntityCartesianVector* getDaMatrix(void) { return matrixDa; };
	EntityCartesianVector* getDualDaMatrix(void) { return matrixDualDa; };
	EntityCartesianVector* getDepsMatrix(void) { return matrixDeps; };
	EntityCartesianVector* getDmuMatrix(void) { return matrixDmu; };
	EntityCartesianVector* getDsigmaMatrix(void) { return matrixDsigma; };
	std::string readMaterialInformation(const std::list<EntityGeometry *> &geometryEntities);
	void clearMaterialInformation(const std::list<EntityGeometry *> &geometryEntities);
	void calcDsW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, EntityGeometry **meshFill);
	void calcDaW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, EntityGeometry **meshFill);
	void calcDmuW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesW, EntityGeometry **meshFill);
	void calcDepsW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesU,double *meshLinesV, EntityGeometry **meshFill);
	void calcDsigmaW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesU, double *meshLinesV, EntityGeometry **meshFill);
	void addMatrixPlot(EntityResultBase::tResultType resultType, const std::string &plotName, EntityCartesianVector *matrix, EntityMeshCartesianData *mesh, std::list<EntityBase *> &entityList, std::map<ot::UID,bool> &topologyEntityForceVisible);

	enum lockType {ANY_OPERATION, MODEL_CHANGE};
	void setUILock(bool flag, lockType type);

	// Attributes
	Application *application;
	EntityMeshCartesian *entityMesh;
	size_t numberPointInsideTests;
	RTCDevice device;
	RTCScene scene;
	RTCGeometry geom;
	CartesianMeshIntersectionData intersectionData;
	std::list<EntityBase *> newTopologyEntities;
	std::list<EntityBase *> newDataEntities;
	std::list<CartesianMeshMaterial *> materialList;
	std::string backgroundShapeName;
	std::string backgroundMeshName;

	EntityCartesianVector *matrixDs;
	EntityCartesianVector *matrixDualDs;
	EntityCartesianVector *matrixDa;
	EntityCartesianVector *matrixDualDa;
	EntityCartesianVector *matrixDeps;
	EntityCartesianVector *matrixDmu;
	EntityCartesianVector *matrixDsigma;
};

#pragma once

namespace osg
{
	class Node;
	class Switch;
	class Geometry;
}

#include "Geometry.h"
#include "SceneNodeContainer.h"
#include "DataBase.h"

#include <string>
#include <map>

#include <osg/Array>

class Model;
class SceneNodeMeshItem;
class SceneNodeCartesianMeshItem;

struct CartesianMeshFaceStatus
{
	bool visible;
	bool forward;
	bool doublesided;
	bool transparent;
	bool wireframe;
	double r;
	double g;
	double b;
};

class SceneNodeCartesianMesh : public SceneNodeContainer
{
public:
	SceneNodeCartesianMesh();
	virtual ~SceneNodeCartesianMesh();

	void setMeshLines(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z);
	void setNeedsInitialization(void) { needsInitialization = true;}; 
	
	std::string getProjectName(void) { return projectName; };

	void setEdgeColor(double color[3]) { edgeColorRGB[0] = color[0]; edgeColorRGB[1] = color[1]; edgeColorRGB[2] = color[2]; };
	void setMeshLineColor(double color[3]) { meshLineColorRGB[0] = color[0]; meshLineColorRGB[1] = color[1]; meshLineColorRGB[2] = color[2]; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	void addOwner(SceneNodeCartesianMeshItem *item, int faceID);
	void removeOwner(SceneNodeCartesianMeshItem *item, const std::vector<int> &faceID);
	void updateFaceStatus(const std::vector<int> &faceID);

	void ensureDataLoaded(void);

	virtual bool isItem3D(void) { return true; };

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void showMeshLines(bool showMesh);

	void setFaceListStorage(const std::string &proj, unsigned long long id, unsigned long long version) { projectName = proj; faceListEntityID = id, faceListEntityVersion = version; }
	void setNodeListStorage(unsigned long long id, unsigned long long version) { nodeListEntityID = id, nodeListEntityVersion = version; }

private:
	osg::Node *createOSGNodeFromCoordinates(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z);
	void get1DBounds(const std::vector<double> &coords, double &min, double &max);
	void addQuadsX(const std::vector<double> &x, double ymin, double ymax, double zmin, double zmax, osg::Vec3Array *vertices, size_t &quadIndex);
	void addQuadsY(const std::vector<double> &y, double xmin, double xmax, double zmin, double zmax, osg::Vec3Array *vertices, size_t &quadIndex);
	void addQuadsZ(const std::vector<double> &z, double xmin, double xmax, double ymin, double ymax, osg::Vec3Array *vertices, size_t &quadIndex);

	void loadNodes(unsigned long long meshNodesID, unsigned long long meshNodesVersion, size_t &numberPoints, double *&coordX, double *&coordY, double *&coordZ);
	void loadFaces(unsigned long long meshFacesID, unsigned long long meshFacesVersion, size_t numberPoints, double *coordX, double *coordY, double *coordZ);
	void loadFace(unsigned long long faceStorageID, unsigned long long faceStorageVersion, osg::Node *&faceNode, osg::Node *&edgeNode, size_t numberPoints, double *coordX, double *coordY, double *coordZ);
	osg::Node *createFaceNode(osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals);
	osg::Node *createEdgeNode(osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals);
	void setFaceStatus(int face, bool visible, bool forward, bool doublesided, bool transparent, bool wireframe, double r, double g, double b, SceneNodeBase *owner);
	void setTransparent(osg::Geometry *faceGeometry, bool transparent);
	void createFaceFromIndices(bsoncxx::document::view view, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals);
	void createFaceFromPoints(bsoncxx::document::view view, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t numberPoints, double *coordX, double *coordY, double *coordZ);
	void calculateNormal(double normal[3], double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2);

	std::map<int, osg::Node *> faceTriangles;
	std::map<int, osg::Node *> faceEdges;

	std::string projectName;
	unsigned long long faceListEntityID;
	unsigned long long faceListEntityVersion;
	unsigned long long nodeListEntityID;
	unsigned long long nodeListEntityVersion;
	bool needsInitialization;
	bool showMeshLinesFlag;

	double edgeColorRGB[3];
	double meshLineColorRGB[3];

	Model *model;

	std::vector<double> meshCoords[3];

	osg::Node *meshLines;

	std::map<int, std::vector < std::pair<bool, SceneNodeCartesianMeshItem*> >> faceOwners;

	std::map<int, CartesianMeshFaceStatus> faceStatusCache;
};


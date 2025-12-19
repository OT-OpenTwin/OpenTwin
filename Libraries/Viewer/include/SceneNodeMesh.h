// @otlicense
// File: SceneNodeMesh.h
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

struct FaceStatus
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

class SceneNodeMesh : public SceneNodeContainer
{
public:
	SceneNodeMesh();
	virtual ~SceneNodeMesh();

	void setStorage(const std::string &proj, unsigned long long id, unsigned long long version) { projectName = proj; entityID = id, entityVersion = version; };
	void setNeedsInitialization(void) { needsInitialization = true;}; 
	
	std::string getProjectName(void) { return projectName; };

	void setEdgeColor(double color[3]) { edgeColorRGB[0] = color[0]; edgeColorRGB[1] = color[1]; edgeColorRGB[2] = color[2]; };

	void addOwner(SceneNodeMeshItem *item, int faceID);
	void removeOwner(SceneNodeMeshItem *item, const std::vector<int> &faceID);
	void updateFaceStatus(const std::vector<int> &faceID, bool edgeColorChanged);

	void ensureDataLoaded(void);

	virtual bool isItem3D(void) const override { return true; };

	double getNodeX(size_t index) { return coordX[index]; }
	double getNodeY(size_t index) { return coordY[index]; }
	double getNodeZ(size_t index) { return coordZ[index]; }

	bool getDisplayTetEdges(void) { return displayTetEdges; }
	void setDisplayTetEdges(bool displayEdges);

	virtual void setTransparency(double value) override;

	void updateEdgeColor();

private:
	void loadMeshData(bsoncxx::builder::basic::document &doc);
	void loadCoordinates(unsigned long long meshNodesID, unsigned long long meshNodesVersion, double *&coordX, double *&coordY, double *&coordZ);
	void loadFaces(unsigned long long meshFacesID, unsigned long long meshFacesVersion, double *coordX, double *coordY, double *coordZ);
	void loadFace(unsigned long long faceStorageID, unsigned long long faceStorageVersion, double *coordX, double *coordY, double *coordZ, osg::Node *&faceNode, osg::Node *&edgeNode);
	osg::Node *createFaceNode(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ);
	osg::Node *createFaceNodeBackwardCompatible(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ);
	osg::Node *createEdgeNode(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ);
	osg::Node *createEdgeNodeBackwardCompatible(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ);
	void addEdge(size_t p1, size_t p2, std::map<std::pair<size_t, size_t>, bool> &edgeMap);
	void setFaceStatus(int face, bool visible, bool forward, bool doublesided, bool transparent, bool wireframe, double r, double g, double b, bool edgeColorChanged, SceneNodeBase *owner);
	void setTransparent(osg::Geometry *faceGeometry, bool transparent);
	void recursivelySetDisplayEdges(bool displayEdges, SceneNodeBase *root);
	void getPrefetchForAllChildNodes(SceneNodeBase *root, std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	void addTriangleVerticesAndNormals(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal);
	void addTriangleVerticesAndNormalsOrder1(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal);
	void addTriangleVerticesAndNormalsOrder2(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal);
	void addTriangleVerticesAndNormalsOrder3(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal);
	void addTriangleEdges(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap);
	void addTriangleEdgesOrder1(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap);
	void addTriangleEdgesOrder2(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap);
	void addTriangleEdgesOrder3(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap);
	void addTriangle(size_t n1Index, size_t n2Index, size_t n3Index, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal);
	void averageNormals(size_t n1Index, size_t n2Index, size_t n3Index, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nNormalBase);
	void averageNormals(size_t n1Index, size_t n2Index, size_t n3Index, size_t n4Index, size_t n5Index, size_t n6Index, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nNormalBase);
	double getActualEdgeColor(const double color[], int index);

	std::map<int, osg::Node *> faceTriangles;
	std::map<int, osg::Node *> faceEdges;

	std::string projectName;
	unsigned long long entityID;
	unsigned long long entityVersion;
	bool needsInitialization;

	bool displayTetEdges;

	double edgeColorRGB[3];
	double *coordX, *coordY, *coordZ;

	std::map<int, std::vector < std::pair<bool, SceneNodeMeshItem*> >> faceOwners;

	std::map<int, FaceStatus> faceStatusCache;
};


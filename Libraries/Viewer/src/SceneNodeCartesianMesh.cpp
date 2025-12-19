// @otlicense
// File: SceneNodeCartesianMesh.cpp
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

#include "stdafx.h"

#include "SceneNodeCartesianMesh.h"
#include "SceneNodeCartesianMeshItem.h"
#include "SceneNodeMaterial.h"
#include "Geometry.h"
#include "ViewerSettings.h"

#include "DataBase.h"
#include "Model.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/Depth>

SceneNodeCartesianMesh::SceneNodeCartesianMesh() :
	edgeColorRGB{ 0.0, 0.0, 0.0 },
	meshLineColorRGB{ 0.0, 0.0, 0.0 },
	meshLines(nullptr),
	showMeshLinesFlag(true),
	faceListEntityID(0),
	faceListEntityVersion(0),
	nodeListEntityID(0),
	nodeListEntityVersion(0),
	needsInitialization(true)
{
	m_transparency = ViewerSettings::instance()->geometrySelectionTransparency;
}

SceneNodeCartesianMesh::~SceneNodeCartesianMesh()
{
	// We first need to recursively delete all children, since the osg nodes will also recursively delete their children as well.
	// This deletion would happen in the base class destructor as well, but at this point this mesh object will already have been
	// partially deleted. Some children may then still have pointers to this mesh which are invalid.
	// The child list will be empty after this operation, so it does not harm to perform the (then empty) loop again.

	std::list<SceneNodeBase*> currentChilds = getChildren();
	for (auto child : currentChilds)
	{
		delete child;
	}

	// Remove all face scene nodes
	for (const auto& face : faceTriangles)
	{
		getModel()->forgetShapeNode(face.second);
	}

	for (const auto& edge : faceEdges)
	{
		getModel()->forgetShapeNode(edge.second);
	}
}

void SceneNodeCartesianMesh::setMeshLines(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z)
{
	meshCoords[0] = x;
	meshCoords[1] = y;
	meshCoords[2] = z;

	// Now create the display objects
	meshLines = createOSGNodeFromCoordinates(x, y, z);

	// Add the triangle and edge nodes to the group node and add the group node to the root
	if (meshLines != nullptr)
	{
		meshLines->setNodeMask(meshLines->getNodeMask() & ~3); // Reset the first and second bit of the node mask to exclude the mesh lines from picking
		m_shapeNode->addChild(meshLines);
	}
}

void SceneNodeCartesianMesh::setTransparent(bool t)
{
	// The cartesian mesh lines are ignoring the transparent state. Therefore, they become clearly visible when the wireframe mode is switched on.
}

void SceneNodeCartesianMesh::setWireframe(bool w)
{
	// The cartesian mesh lines are ignoring the wireframe state. Therefore, they become clearly visible when the wireframe mode is switched on.
}

void SceneNodeCartesianMesh::setVisible(bool v)
{
	if (isVisible() == v) return;  // No change necessary

	SceneNodeBase::setVisible(v);

	if (isVisible())
	{
		if (meshLines != nullptr) getShapeNode()->setChildValue(meshLines, showMeshLinesFlag);
	}
	else
	{
		if (meshLines != nullptr) getShapeNode()->setChildValue(meshLines, false);
	}
}

void SceneNodeCartesianMesh::setHighlighted(bool h)
{
	// The cartesian mesh lines are ignoring the highlighted state. Therefore, they become clearly visible when the wireframe mode is switched on.
}

osg::Node *SceneNodeCartesianMesh::createOSGNodeFromCoordinates(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(meshLineColorRGB[0], meshLineColorRGB[1], meshLineColorRGB[2], 1.0f));

	unsigned long long nQuads = x.size() + y.size() + z.size();
	if (nQuads == 0) return nullptr;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nQuads * 4);
	size_t quadIndex = 0;

	double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0, zmin = 0.0, zmax = 0.0;
	get1DBounds(x, xmin, xmax);
	get1DBounds(y, ymin, ymax);
	get1DBounds(z, zmin, zmax);

	addQuadsX(x, ymin, ymax, zmin, zmax, vertices, quadIndex);
	addQuadsY(y, xmin, xmax, zmin, zmax, vertices, quadIndex);
	addQuadsZ(z, xmin, xmax, ymin, ymax, vertices, quadIndex);

	assert(quadIndex <= nQuads * 4);
	
	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->setVertexArray(vertices);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, quadIndex));

	// Now create the geometry node and assign the drawable
	osg::Geode *linesNode = new osg::Geode;
	linesNode->addDrawable(newGeometry);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = linesNode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	return linesNode;
}

void SceneNodeCartesianMesh::get1DBounds(const std::vector<double> &coords, double &min, double &max)
{
	min = coords[0];
	max = coords[coords.size() - 1];
}

void SceneNodeCartesianMesh::addQuadsX(const std::vector<double> &x, double ymin, double ymax, double zmin, double zmax, osg::Vec3Array *vertices, size_t &quadIndex)
{
	for (int ix = 0; ix < x.size(); ix++)
	{
		vertices->at(quadIndex++).set(x[ix], ymin, zmin);
		vertices->at(quadIndex++).set(x[ix], ymin, zmax);
		vertices->at(quadIndex++).set(x[ix], ymax, zmax);
		vertices->at(quadIndex++).set(x[ix], ymax, zmin);
	}
}

void SceneNodeCartesianMesh::addQuadsY(const std::vector<double> &y, double xmin, double xmax, double zmin, double zmax, osg::Vec3Array *vertices, size_t &quadIndex)
{
	for (int iy = 0; iy < y.size(); iy++)
	{
		vertices->at(quadIndex++).set(xmin, y[iy], zmin);
		vertices->at(quadIndex++).set(xmin, y[iy], zmax);
		vertices->at(quadIndex++).set(xmax, y[iy], zmax);
		vertices->at(quadIndex++).set(xmax, y[iy], zmin);
	}
}

void SceneNodeCartesianMesh::addQuadsZ(const std::vector<double> &z, double xmin, double xmax, double ymin, double ymax, osg::Vec3Array *vertices, size_t &quadIndex)
{
	for (int iz = 0; iz < z.size(); iz++)
	{
		vertices->at(quadIndex++).set(xmin, ymin, z[iz]);
		vertices->at(quadIndex++).set(xmin, ymax, z[iz]);
		vertices->at(quadIndex++).set(xmax, ymax, z[iz]);
		vertices->at(quadIndex++).set(xmax, ymin, z[iz]);
	}
}

void SceneNodeCartesianMesh::showMeshLines(bool showMesh)
{
	showMeshLinesFlag = showMesh;

	if (isVisible())
	{
		if (meshLines != nullptr) getShapeNode()->setChildValue(meshLines, showMeshLinesFlag);
	}
}

void SceneNodeCartesianMesh::ensureDataLoaded(void)
{
	if (!needsInitialization) return;
	needsInitialization = false;

	// Now read the mesh faces 

	//auto t1 = std::chrono::high_resolution_clock::now();

	double *coordX = nullptr, *coordY = nullptr, *coordZ = nullptr;
	size_t numberCoords = 0;

	if (nodeListEntityID != 0)
	{
		loadNodes(nodeListEntityID, nodeListEntityVersion, numberCoords, coordX, coordY, coordZ);
	}

	loadFaces(faceListEntityID, faceListEntityVersion, numberCoords, coordX, coordY, coordZ);

	//std::cout << "load faces ended" << std::endl;
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << "total time: " << duration / 1.0e6 << std::endl;
}

void SceneNodeCartesianMesh::loadNodes(unsigned long long meshNodesID, unsigned long long meshNodesVersion, size_t &numberPoints, double *&coordX, double *&coordY, double *&coordZ)
{
	// First open the mesh nodes document
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(meshNodesID, meshNodesVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshCartesianNodes")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshCartesianNodes");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Now read the coordinates

	auto pointX = doc_view["CoordX"].get_array().value;
	auto pointY = doc_view["CoordY"].get_array().value;
	auto pointZ = doc_view["CoordZ"].get_array().value;

	numberPoints = std::distance(pointX.begin(), pointX.end());
	assert(numberPoints == std::distance(pointY.begin(), pointY.end()));
	assert(numberPoints == std::distance(pointZ.begin(), pointZ.end()));

	auto px = pointX.begin();
	auto py = pointY.begin();
	auto pz = pointZ.begin();

	coordX = new double[numberPoints];
	coordY = new double[numberPoints];
	coordZ = new double[numberPoints];

	for (unsigned long index = 0; index < numberPoints; index++)
	{
		coordX[index] = px->get_double();
		coordY[index] = py->get_double();
		coordZ[index] = pz->get_double();

		px++;
		py++;
		pz++;
	}
}



void SceneNodeCartesianMesh::loadFaces(unsigned long long meshFacesID, unsigned long long meshFacesVersion, size_t numberPoints, double *coordX, double *coordY, double *coordZ)
{
	// First open the mesh nodes document
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(meshFacesID, meshFacesVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshCartesianFaceList")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshCartesianFaceList");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Now read the faces

	auto faceIndex = doc_view["MeshFacesIndex"].get_array().value;
	auto faceId = doc_view["MeshFacesID"].get_array().value;
	auto faceVersion = doc_view["MeshFacesVersion"].get_array().value;

	size_t numberFaces = std::distance(faceIndex.begin(), faceIndex.end());
	assert(numberFaces == std::distance(faceId.begin(), faceId.end()));
	assert(numberFaces == std::distance(faceVersion.begin(), faceVersion.end()));

	std::list< std::pair<unsigned long long, unsigned long long>> faceIdList;

	auto findex = faceIndex.begin();
	auto fid = faceId.begin();	
	auto fversion = faceVersion.begin();

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		faceIdList.push_back(std::pair<unsigned long long, unsigned long long>(fid->get_int64(), fversion->get_int64()));

		findex++;
		fid++;
		fversion++;
	}

	//std::cout << "Start prefetching data" << std::endl;
	//auto t1 = std::chrono::high_resolution_clock::now();
	DataBase::instance().prefetchDocumentsFromStorage(faceIdList);
	//auto t2 = std::chrono::high_resolution_clock::now();
	//std::cout << "Finished prefetching data" << std::endl;

	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << "total time for prefetching: " << duration / 1.0e6 << std::endl;

	findex = faceIndex.begin();

	for (auto faceStorageID : faceIdList)
	{
		osg::Node *faceNode = nullptr, *edgeNode = nullptr;

		loadFace(faceStorageID.first, faceStorageID.second, faceNode, edgeNode, numberPoints, coordX, coordY, coordZ);

		faceTriangles[DataBase::getIntFromArrayViewIterator(findex)] = faceNode;
		faceEdges[DataBase::getIntFromArrayViewIterator(findex)] = edgeNode;

		if (faceNode != nullptr)
		{
			m_shapeNode->addChild(faceNode);
			m_shapeNode->setChildValue(faceNode, false);
		}

		if (edgeNode != nullptr)
		{
			m_shapeNode->addChild(edgeNode);
			m_shapeNode->setChildValue(edgeNode, false);
		}

		findex++;
	}
}

void SceneNodeCartesianMesh::loadFace(unsigned long long faceStorageID, unsigned long long faceStorageVersion, osg::Node *&faceNode, osg::Node *&edgeNode,
								      size_t numberPoints, double *coordX, double *coordY, double *coordZ)
{
	// First open the mesh faces document
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(faceStorageID, faceStorageVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshCartesianFace")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshCartesianFace");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	bool pointStorage = false;
	
	try
	{
		pointStorage = (doc_view["Type"].get_int32() == 1);
	}
	catch (std::exception)
	{
	}

	osg::ref_ptr<osg::Vec3Array> vertices, normals;

	if (pointStorage)
	{
		createFaceFromPoints(doc_view, vertices, normals, numberPoints, coordX, coordY, coordZ);
	}
	else
	{
		assert(numberPoints == 0);

		createFaceFromIndices(doc_view, vertices, normals);
	}

	// Now create the osg node for the face triangles
	faceNode = createFaceNode(vertices, normals);

	// Finally create the osg node for the edges
	edgeNode = createEdgeNode(vertices, normals);

	// Remove potentially prefetched data
	DataBase::instance().removePrefetchedDocument(faceStorageID);
}

void SceneNodeCartesianMesh::createFaceFromIndices(bsoncxx::document::view view, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals)
{
	auto facesX = view["CellFacesX"].get_array().value;
	auto facesY = view["CellFacesY"].get_array().value;
	auto facesZ = view["CellFacesZ"].get_array().value;

	size_t numberFacesX = std::distance(facesX.begin(), facesX.end());
	size_t numberFacesY = std::distance(facesY.begin(), facesY.end());
	size_t numberFacesZ = std::distance(facesZ.begin(), facesZ.end());

	size_t numberFaces = numberFacesX + numberFacesY + numberFacesZ;

	auto fx = facesX.begin();
	auto fy = facesY.begin();
	auto fz = facesZ.begin();

	vertices = new osg::Vec3Array(numberFaces * 4);
	normals = new osg::Vec3Array(numberFaces * 4);

	// Now store the triangle vertices in the nodes and normals array

	unsigned long long nVertex = 0;
	unsigned long long nNormal = 0;

	size_t nx = meshCoords[0].size();
	size_t ny = meshCoords[1].size();
	size_t nz = meshCoords[2].size();

	for (unsigned long index = 0; index < numberFacesX; index++)
	{
		long long faceIndex = DataBase::getIntFromArrayViewIterator(fx);
		double normalFactor = faceIndex < 0 ? -1 : 1;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx);
		assert(iy >= 0 && iy < ny - 1);
		assert(iz >= 0 && iz < nz - 1);

		double x = meshCoords[0][ix];
		double ymin = meshCoords[1][iy];
		double ymax = meshCoords[1][iy + 1];
		double zmin = meshCoords[2][iz];
		double zmax = meshCoords[2][iz + 1];

		if (normalFactor > 0.0)
		{
			vertices->at(nVertex).set(x, ymin, zmin);
			vertices->at(nVertex + 1).set(x, ymax, zmin);
			vertices->at(nVertex + 2).set(x, ymax, zmax);
			vertices->at(nVertex + 3).set(x, ymin, zmax);
		}
		else
		{
			vertices->at(nVertex + 3).set(x, ymin, zmin);
			vertices->at(nVertex + 2).set(x, ymax, zmin);
			vertices->at(nVertex + 1).set(x, ymax, zmax);
			vertices->at(nVertex).set(x, ymin, zmax);
		}

		nVertex += 4;

		normals->at(nNormal).set(normalFactor, 0.0, 0.0);
		normals->at(nNormal + 1).set(normalFactor, 0.0, 0.0);
		normals->at(nNormal + 2).set(normalFactor, 0.0, 0.0);
		normals->at(nNormal + 3).set(normalFactor, 0.0, 0.0);

		nNormal += 4;

		fx++;
	}

	for (unsigned long index = 0; index < numberFacesY; index++)
	{
		long long faceIndex = DataBase::getIntFromArrayViewIterator(fy);
		double normalFactor = faceIndex < 0 ? -1 : 1;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx - 1);
		assert(iy >= 0 && iy < ny);
		assert(iz >= 0 && iz < nz - 1);

		double y = meshCoords[1][iy];
		double xmin = meshCoords[0][ix];
		double xmax = meshCoords[0][ix + 1];
		double zmin = meshCoords[2][iz];
		double zmax = meshCoords[2][iz + 1];

		if (normalFactor < 0.0)
		{
			vertices->at(nVertex).set(xmin, y, zmin);
			vertices->at(nVertex + 1).set(xmax, y, zmin);
			vertices->at(nVertex + 2).set(xmax, y, zmax);
			vertices->at(nVertex + 3).set(xmin, y, zmax);
		}
		else
		{
			vertices->at(nVertex + 3).set(xmin, y, zmin);
			vertices->at(nVertex + 2).set(xmax, y, zmin);
			vertices->at(nVertex + 1).set(xmax, y, zmax);
			vertices->at(nVertex).set(xmin, y, zmax);
		}

		nVertex += 4;

		normals->at(nNormal).set(0.0, normalFactor, 0.0);
		normals->at(nNormal + 1).set(0.0, normalFactor, 0.0);
		normals->at(nNormal + 2).set(0.0, normalFactor, 0.0);
		normals->at(nNormal + 3).set(0.0, normalFactor, 0.0);

		nNormal += 4;

		fy++;
	}

	for (unsigned long index = 0; index < numberFacesZ; index++)
	{
		long long faceIndex = DataBase::getIntFromArrayViewIterator(fz);
		double normalFactor = faceIndex < 0 ? -1 : 1;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx - 1);
		assert(iy >= 0 && iy < ny - 1);
		assert(iz >= 0 && iz < nz);

		double z = meshCoords[2][iz];
		double xmin = meshCoords[0][ix];
		double xmax = meshCoords[0][ix + 1];
		double ymin = meshCoords[1][iy];
		double ymax = meshCoords[1][iy + 1];

		if (normalFactor > 0.0)
		{
			vertices->at(nVertex).set(xmin, ymin, z);
			vertices->at(nVertex + 1).set(xmax, ymin, z);
			vertices->at(nVertex + 2).set(xmax, ymax, z);
			vertices->at(nVertex + 3).set(xmin, ymax, z);
		}
		else
		{
			vertices->at(nVertex + 3).set(xmin, ymin, z);
			vertices->at(nVertex + 2).set(xmax, ymin, z);
			vertices->at(nVertex + 1).set(xmax, ymax, z);
			vertices->at(nVertex).set(xmin, ymax, z);
		}

		nVertex += 4;

		normals->at(nNormal).set(0.0, 0.0, normalFactor);
		normals->at(nNormal + 1).set(0.0, 0.0, normalFactor);
		normals->at(nNormal + 2).set(0.0, 0.0, normalFactor);
		normals->at(nNormal + 3).set(0.0, 0.0, normalFactor);

		nNormal += 4;

		fz++;
	}
}

void SceneNodeCartesianMesh::createFaceFromPoints(bsoncxx::document::view view, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals,
												  size_t numberPoints, double *coordX, double *coordY, double *coordZ)
{
	auto p0 = view["P0"].get_array().value;
	auto p1 = view["P1"].get_array().value;
	auto p2 = view["P2"].get_array().value;
	auto p3 = view["P3"].get_array().value;

	size_t numberFaces = std::distance(p0.begin(), p0.end());
	assert(numberFaces == std::distance(p1.begin(), p1.end()));
	assert(numberFaces == std::distance(p2.begin(), p2.end()));
	assert(numberFaces == std::distance(p3.begin(), p3.end()));

	auto i0 = p0.begin();
	auto i1 = p1.begin();
	auto i2 = p2.begin();
	auto i3 = p3.begin();

	vertices = new osg::Vec3Array(numberFaces * 4);
	normals = new osg::Vec3Array(numberFaces * 4);

	// Now store the triangle vertices in the nodes and normals array

	unsigned long long nVertex = 0;
	unsigned long long nNormal = 0;

	size_t nx = meshCoords[0].size();
	size_t ny = meshCoords[1].size();
	size_t nz = meshCoords[2].size();

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		long long index0 = DataBase::getIntFromArrayViewIterator(i0);
		long long index1 = DataBase::getIntFromArrayViewIterator(i1);
		long long index2 = DataBase::getIntFromArrayViewIterator(i2);
		long long index3 = DataBase::getIntFromArrayViewIterator(i3);

		i0++;
		i1++;
		i2++;
		i3++;

		assert(index0 < numberPoints);
		assert(index1 < numberPoints);
		assert(index2 < numberPoints);

		if (index3 < 0)
		{
			// We have a triangle
			double x0 = coordX[index0];
			double y0 = coordY[index0];
			double z0 = coordZ[index0];

			double x1 = coordX[index1];
			double y1 = coordY[index1];
			double z1 = coordZ[index1];

			double x2 = coordX[index2];
			double y2 = coordY[index2];
			double z2 = coordZ[index2];

			vertices->at(nVertex).set(x0, y0, z0);
			vertices->at(nVertex + 1).set(x1, y1, z1);
			vertices->at(nVertex + 2).set(x2, y2, z2);
			vertices->at(nVertex + 3).set(x2, y2, z2);

			nVertex += 4;

			double normal[3];
			calculateNormal(normal, x0, y0, z0, x1, y1, z1, x2, y2, z2);

			normals->at(nNormal).set(normal[0], normal[1], normal[2]);
			normals->at(nNormal + 1).set(normal[0], normal[1], normal[2]);
			normals->at(nNormal + 2).set(normal[0], normal[1], normal[2]);
			normals->at(nNormal + 3).set(normal[0], normal[1], normal[2]);

			nNormal += 4;
		}
		else
		{
			assert(index3 < numberPoints);

			// We have a quad
			double x0 = coordX[index0];
			double y0 = coordY[index0];
			double z0 = coordZ[index0];

			double x1 = coordX[index1];
			double y1 = coordY[index1];
			double z1 = coordZ[index1];

			double x2 = coordX[index2];
			double y2 = coordY[index2];
			double z2 = coordZ[index2];

			double x3 = coordX[index3];
			double y3 = coordY[index3];
			double z3 = coordZ[index3];

			vertices->at(nVertex).set(x0, y0, z0);
			vertices->at(nVertex + 1).set(x1, y1, z1);
			vertices->at(nVertex + 2).set(x2, y2, z2);
			vertices->at(nVertex + 3).set(x3, y3, z3);

			nVertex += 4;

			double normal[3];
			calculateNormal(normal, x0, y0, z0, x1, y1, z1, x3, y3, z3);
			normals->at(nNormal).set(normal[0], normal[1], normal[2]);

			calculateNormal(normal, x1, y1, z1, x2, y2, z2, x0, y0, z0);
			normals->at(nNormal + 1).set(normal[0], normal[1], normal[2]);

			calculateNormal(normal, x2, y2, z2, x3, y3, z3, x1, y1, z1);
			normals->at(nNormal + 2).set(normal[0], normal[1], normal[2]);

			calculateNormal(normal, x3, y3, z3, x0, y0, z0, x2, y2, z2);
			normals->at(nNormal + 3).set(normal[0], normal[1], normal[2]);

			nNormal += 4;
		}
	}
}

void SceneNodeCartesianMesh::calculateNormal(double normal[3], double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2)
{
	// Calculate the cross product first

	double v1x = x1 - x0;
	double v1y = y1 - y0;
	double v1z = z1 - z0;

	double v2x = x2 - x0;
	double v2y = y2 - y0;
	double v2z = z2 - z0;

	normal[0] = v1y * v2z - v1z * v2y;
	normal[1] = v1z * v2x - v1x * v2z;
	normal[2] = v1x * v2y - v1y * v2x;

	// Now normalize the vector

	double fact = 1.0 / sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	normal[0] *= fact;
	normal[1] *= fact;
	normal[2] *= fact;
}

osg::Node *SceneNodeCartesianMesh::createFaceNode(osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals)
{
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel *p = new osg::LightModel;
	p->setTwoSided(true);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(p);

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	newGeometry->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

	// Store the color in a color array (the color will be shared among all nodes, so only one entry is needed)
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.5, 0.5, 0.5, 1.0-m_transparency));

	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(2.0f, 2.0f));

	newGeometry->setVertexArray(vertices);

	newGeometry->setNormalArray(normals);
	newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, vertices->size()));

	// Now create the geometry node and assign the drawable
	osg::Geode *triangleNode = new osg::Geode;
	triangleNode->addDrawable(newGeometry);

	return triangleNode;
}

osg::Node *SceneNodeCartesianMesh::createEdgeNode(osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals)
{
	// First create a list with all edges. Each edge is represented as a pait with the smaller coordinate index first and the larger
	// coordinate index second

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(getActualEdgeColor(edgeColorRGB, 0), getActualEdgeColor(edgeColorRGB, 1), getActualEdgeColor(edgeColorRGB, 2), 1.0f));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));

	newGeometry->setVertexArray(vertices);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

	// Now create the geometry node and assign the drawable
	osg::Geode *edgesNode = new osg::Geode;
	edgesNode->addDrawable(newGeometry);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = edgesNode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	//ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	return edgesNode;
}

void SceneNodeCartesianMesh::addOwner(SceneNodeCartesianMeshItem *item, int faceID)
{
	bool forward = (faceID > 0);
	faceID = abs(faceID);

	std::pair<bool, SceneNodeCartesianMeshItem*> info(forward, item);

	faceOwners[faceID].push_back(info);
}

void SceneNodeCartesianMesh::updateFaceStatus(const std::vector<int> &faceID, bool edgeColorChanged)
{
	if (needsInitialization) return;

	for (auto signedFace : faceID)
	{
		int face = abs(signedFace);

		// Here we need to check the visibility status for each face
		assert(faceOwners.count(face) >= 0);

		if (faceOwners[face].size() == 0)
		{
			// This face has no owner -> it belongs to a mesh item which was deleted -> the face is hidden
			setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, edgeColorChanged, nullptr);
		}
		else if (faceOwners[face].size() == 1)
		{
			// The face has one owner
			bool forward            = faceOwners[face].at(0).first;
			SceneNodeCartesianMeshItem *item = faceOwners[face].at(0).second;

			// The face is visible of the owner is visible
			setFaceStatus(face, item->isVisible(), forward, !item->getIsVolume(), item->isTransparent(), item->isWireframe(), item->getColorR(), item->getColorG(), item->getColorB(), edgeColorChanged, item);
		}
		else if (faceOwners[face].size() == 2)
		{
			bool forward1 = faceOwners[face].at(0).first;
			SceneNodeCartesianMeshItem *item1 = faceOwners[face].at(0).second;

			bool forward2 = faceOwners[face].at(1).first;
			SceneNodeCartesianMeshItem *item2 = faceOwners[face].at(1).second;

			bool drawOpaqueItem1 = (item1->isVisible() && !item1->isTransparent());
			bool drawOpaqueItem2 = (item2->isVisible() && !item2->isTransparent());

			if (drawOpaqueItem1 && drawOpaqueItem2)
			{
				// Both items are visible -> the face is hidden
				setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, edgeColorChanged, nullptr);
			}
			else if (drawOpaqueItem1)
			{
				setFaceStatus(face, item1->isVisible(), forward1, !item1->getIsVolume(), false, item1->isWireframe(), item1->getColorR(), item1->getColorG(), item1->getColorB(), edgeColorChanged, item1);
			}
			else if (drawOpaqueItem2)
			{
				setFaceStatus(face, item2->isVisible(), forward2, !item2->getIsVolume(), false, item2->isWireframe(), item2->getColorR(), item2->getColorG(), item2->getColorB(), edgeColorChanged, item2);
			}
			else
			{
				// Here it could be that both faces are visible. In this case, we do not draw the interior face. Otherwise we draw the face transparently

				if (!item1->isVisible() && !item2->isVisible())
				{
					// None of the two items is visible -> hide the face
					setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, edgeColorChanged, nullptr);
				}
				else if (item1->isVisible() && item2->isVisible())
				{
					// Both items are visible -> hide the face
					setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, edgeColorChanged, nullptr);
				}
				else
				{
					if (item1->isVisible())
					{
						setFaceStatus(face, true, forward1, !item1->getIsVolume(), true, item1->isWireframe(), item1->getColorR(), item1->getColorG(), item1->getColorB(), edgeColorChanged, item1);
					}
					else if (item2->isVisible())
					{
						setFaceStatus(face, true, forward2, !item2->getIsVolume(), true, item2->isWireframe(), item2->getColorR(), item2->getColorG(), item2->getColorB(), edgeColorChanged, item2);
					}
				}
			}
		}
		else
		{
			assert(0); // This face does not have one or two owners
		}
	}
}

void SceneNodeCartesianMesh::setFaceStatus(int face, bool visible, bool forward, bool doublesided, bool transparent, bool wireframe, double r, double g, double b, bool edgeColorChanged, SceneNodeBase* owner)
{
	// Check whether face status has really changed

	bool visibilityChanged = true;
	bool forwardChanged = true;
	bool doublesidedChanged = true;
	bool transparentChanged = true;
	bool wireframeChanged = true;
	bool colorChanged = true;

	if (faceStatusCache.count(face) > 0)
	{
		CartesianMeshFaceStatus statusCache = faceStatusCache[face];

		if (!statusCache.visible && !visible) return;  // The face was not visible and is still not visible

		if (statusCache.visible == visible) visibilityChanged = false;
		if (statusCache.forward == forward) forwardChanged = false;
		if (statusCache.doublesided == doublesided) doublesidedChanged = false;
		if (statusCache.transparent == transparent) transparentChanged = false;
		if (statusCache.wireframe == wireframe) wireframeChanged = false;
		if (fabs(statusCache.r - r) < 1e-10 && fabs(statusCache.g - g) < 1e-10 && fabs(statusCache.b - b) < 1e-10) colorChanged = false;

		if (!visibilityChanged && !forwardChanged && !doublesidedChanged && !transparentChanged && !wireframeChanged && !colorChanged && !edgeColorChanged)
		{
			// The face has not changed
			return;
		}
	}

	// The face has changed. Cache the new settings

	CartesianMeshFaceStatus newStatusCache;
	newStatusCache.visible = visible;
	newStatusCache.forward = forward;
	newStatusCache.doublesided = doublesided;
	newStatusCache.transparent = transparent;
	newStatusCache.wireframe = wireframe;
	newStatusCache.r = r;
	newStatusCache.g = g;
	newStatusCache.b = b;

	faceStatusCache[face] = newStatusCache;

	// Now modify the face status according to the new settings

	osg::Node* faceNode = faceTriangles[face];
	assert(faceNode != nullptr);

	osg::Node* edgeNode = faceEdges[face];
	assert(edgeNode != nullptr);

	// Set the owner
	if (visible)
	{
		if (owner == nullptr)
		{
			getModel()->forgetShapeNode(faceNode);
			getModel()->forgetShapeNode(edgeNode);
		}
		else
		{
			getModel()->storeShapeNode(faceNode, owner);
			getModel()->storeShapeNode(edgeNode, owner);
		}
	}

	bool showFaces = visible && (!wireframe || transparent);
	bool showEdges = visible && (!transparent);

	m_shapeNode->setChildValue(faceNode, showFaces);
	m_shapeNode->setChildValue(edgeNode, showEdges);

	// Now get the face geometry node for modification
	osg::Geode *faceGeode = dynamic_cast<osg::Geode *>(faceNode);
	assert(faceGeode != nullptr);
	osg::Geode *edgeGeode = dynamic_cast<osg::Geode *>(edgeNode);
	assert(edgeGeode != nullptr);

	osg::Geometry *faceGeometry = dynamic_cast<osg::Geometry *>(faceGeode->getDrawable(0));
	assert(faceGeometry != nullptr);
	osg::Geometry *edgeGeometry = dynamic_cast<osg::Geometry *>(edgeGeode->getDrawable(0));
	assert(edgeGeometry != nullptr);

	bool faceRefreshNeeded = false;
	bool edgeRefreshNeeded = false;

	if (colorChanged)
	{
		// Modify the color of the face
		osg::ref_ptr<osg::Material> material = new osg::Material;

		SceneNodeMaterial sceneNodeMaterial;
		sceneNodeMaterial.setMaterial(material, "Rough", r, g, b, 1.0-m_transparency);

		faceGeometry->getOrCreateStateSet()->setAttribute(material);

		faceRefreshNeeded = true;
	}

	if (colorChanged || wireframeChanged || edgeColorChanged)
	{
		// Modify the color of the edge
		osg::Vec4Array *edgeColorArray = dynamic_cast<osg::Vec4Array *>(edgeGeometry->getColorArray());
		edgeColorArray->at(0) = wireframe ? osg::Vec4(r, g, b, 1.0f) : osg::Vec4(getActualEdgeColor(edgeColorRGB, 0), getActualEdgeColor(edgeColorRGB, 1), getActualEdgeColor(edgeColorRGB, 2), 1.0f);
		edgeColorArray->dirty();
		edgeRefreshNeeded = true;
	}

	if (transparentChanged)
	{
		// Set the m_transparency state of the face 
		setTransparent(faceGeometry, transparent);
		faceRefreshNeeded = true;
	}

	if (forwardChanged || doublesidedChanged)
	{
		// Modify the culling state (forward / backward / node)
		if (doublesided)
		{
			faceGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			faceRefreshNeeded = true;
		}
		else if (forward)
		{
			faceGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			faceGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
			faceRefreshNeeded = true;
		}
		else
		{
			faceGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			faceGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT));
			faceRefreshNeeded = true;
		}
	}

	// Force a re-creation of the display list on next redraw
	if (faceRefreshNeeded) faceGeometry->dirtyGLObjects();
	if (edgeRefreshNeeded) edgeGeometry->dirtyGLObjects();
}

void SceneNodeCartesianMesh::setTransparent(osg::Geometry *faceGeometry, bool transparent)
{
	if (transparent)
	{
		// Set node to transparent state
		osg::StateSet *stateset = faceGeometry->getOrCreateStateSet();

		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	}
	else
	{
		// Set node to opaque state
		osg::StateSet *stateset = faceGeometry->getOrCreateStateSet();

		stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(true);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	}
}

void SceneNodeCartesianMesh::removeOwner(SceneNodeCartesianMeshItem *item, const std::vector<int> &faceID)
{
	for (auto face : faceID)
	{
		// Remove this owner from each face
		if (faceOwners.find(abs(face)) != faceOwners.end())
		{
			std::vector < std::pair<bool, SceneNodeCartesianMeshItem*> > ownerList = faceOwners[abs(face)];

			std::vector < std::pair<bool, SceneNodeCartesianMeshItem*> > newOwnerList;

			for (auto p : ownerList)
			{
				if (p.second != item) newOwnerList.push_back(p);
			}

			faceOwners[abs(face)] = newOwnerList;
		}
	}

	updateFaceStatus(faceID, false);
}

void SceneNodeCartesianMesh::setTransparency(double value)
{
	SceneNodeBase::setTransparency(value);

	// Reset the transparency for all triangles
	for (auto& face : faceTriangles)
	{
		osg::Geode* faceGeode = dynamic_cast<osg::Geode*>(face.second);
		assert(faceGeode != nullptr);

		osg::Geometry* faceGeometry = dynamic_cast<osg::Geometry*>(faceGeode->getDrawable(0));
		assert(faceGeometry != nullptr);

		osg::Material* material = dynamic_cast<osg::Material*>(faceGeometry->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		assert(material != nullptr);

		material->setAlpha(osg::Material::FRONT_AND_BACK, 1.0 - m_transparency);

		faceGeometry->dirtyGLObjects();
	}
}

double SceneNodeCartesianMesh::getActualEdgeColor(const double colorRGB[], int index)
{
	double color[] = { ViewerSettings::instance()->meshEdgeColor.r() / 255.0,
						ViewerSettings::instance()->meshEdgeColor.g() / 255.0,
						ViewerSettings::instance()->meshEdgeColor.b() / 255.0 };
	return color[index];
}

void SceneNodeCartesianMesh::updateEdgeColor()
{
	std::vector<int> faceIDList;

	for (auto& face : faceTriangles)
	{
		faceIDList.push_back(face.first);
	}

	updateFaceStatus(faceIDList, true);
}

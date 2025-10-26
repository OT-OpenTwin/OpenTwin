#include "stdafx.h"

#include "SceneNodeMesh.h"
#include "SceneNodeMeshItem.h"
#include "Geometry.h"
#include "ViewerSettings.h"

#include "DataBase.h"
#include "Model.h"
#include "SceneNodeMaterial.h"

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

SceneNodeMesh::SceneNodeMesh() :
	entityID(0),
	entityVersion(0),
	needsInitialization(true),
	edgeColorRGB{ 0.0, 0.0, 0.0 },
	model(nullptr),
	coordX(nullptr),
	coordY(nullptr),
	coordZ(nullptr),
	displayTetEdges(false)
{

}


SceneNodeMesh::~SceneNodeMesh()
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

	// Release the coordinates
	delete[] coordX;
	delete[] coordY;
	delete[] coordZ;

	coordX = coordY = coordZ = nullptr;
}

void SceneNodeMesh::setDisplayTetEdges(bool displayEdges)
{
	displayTetEdges = displayEdges;

	if (needsInitialization) return; // In this case, everything will be loaded once the mesh is visualized for the first time

	if (displayEdges)
	{
		// We prefetch the data will will be needed to build the edges for the mesh display
		std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;

		getPrefetchForAllChildNodes(this, projectName, prefetchIDs);

		if (!prefetchIDs.empty())
		{
			DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);
		}
	}

	// Now recursively set the mesh edges display for all children (this will also load the edge data if needed)
	recursivelySetDisplayEdges(displayEdges, this);
}

void SceneNodeMesh::recursivelySetDisplayEdges(bool displayEdges, SceneNodeBase *root)
{
	SceneNodeMeshItem *item = dynamic_cast<SceneNodeMeshItem *>(root);

	if (item != nullptr)
	{
		item->setDisplayTetEdges(displayEdges);
	}

	for (auto child : root->getChildren())
	{
		recursivelySetDisplayEdges(displayEdges, child);
	}
}

void SceneNodeMesh::getPrefetchForAllChildNodes(SceneNodeBase *root, std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	root->getPrefetch(projectName, prefetchIDs);

	for (auto child : root->getChildren())
	{
		getPrefetchForAllChildNodes(child, projectName, prefetchIDs);
	}
}

void SceneNodeMesh::ensureDataLoaded(void)
{
	if (!needsInitialization) return;
	needsInitialization = false;

	// Create the shape node
	//m_shapeNode = new osg::Switch;

	// Now add the current nodes osg node to the parent's osg node
	getParent()->getShapeNode()->addChild(getShapeNode());
	getModel()->storeShapeNode(this);
	
	// First load the mesh data document
	auto doc = bsoncxx::builder::basic::document{};

	loadMeshData(doc);

	auto doc_view = doc.view()["Found"].get_document().view();

	// Now load the mesh nodes

	unsigned long long meshNodesID = doc_view["MeshNodesID"].get_int64();
	unsigned long long meshNodesVersion = doc_view["MeshNodesVersion"].get_int64();

	assert(coordX == nullptr && coordY == nullptr && coordZ == nullptr);
	loadCoordinates(meshNodesID, meshNodesVersion, coordX, coordY, coordZ);

	// Now read the mesh faces one by one

	unsigned long long meshFacesID = doc_view["MeshFacesID"].get_int64();
	unsigned long long meshFacesVersion = doc_view["MeshFacesVersion"].get_int64();

	//auto t1 = std::chrono::high_resolution_clock::now();

	loadFaces(meshFacesID, meshFacesVersion, coordX, coordY, coordZ);

	//std::cout << "load faces ended" << std::endl;
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << "total time: " << duration / 1.0e6 << std::endl;
}

void SceneNodeMesh::loadMeshData(bsoncxx::builder::basic::document &doc)
{
	if (!DataBase::instance().getDocumentFromEntityIDandVersion(entityID, entityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshTetData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshTetData");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}
}

void SceneNodeMesh::loadFaces(unsigned long long meshFacesID, unsigned long long meshFacesVersion, double *coordX, double *coordY, double *coordZ)
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

	if (entityType != "EntityMeshTetFaceData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshTetFaceData");
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

		loadFace(faceStorageID.first, faceStorageID.second, coordX, coordY, coordZ, faceNode, edgeNode);

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

void SceneNodeMesh::loadCoordinates(unsigned long long meshNodesID, unsigned long long meshNodesVersion, double *&coordX, double *&coordY, double *&coordZ)
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

	if (entityType != "EntityMeshTetNodes")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshTetNodes");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Now read the coordinates

	auto pointX = doc_view["CoordX"].get_array().value;
	auto pointY = doc_view["CoordY"].get_array().value;
	auto pointZ = doc_view["CoordZ"].get_array().value;

	size_t numberPoints = std::distance(pointX.begin(), pointX.end());
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

void SceneNodeMesh::loadFace(unsigned long long faceStorageID, unsigned long long faceStorageVersion, double *coordX, double *coordY, double *coordZ, osg::Node *&faceNode, osg::Node *&edgeNode)
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

	if (entityType != "EntityMeshTetFace")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityMeshTetFace");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Now create the osg node for the face triangles
	faceNode = createFaceNode(doc_view, coordX, coordY, coordZ);

	// Finally create the osg node for the edges
	edgeNode = createEdgeNode(doc_view, coordX, coordY, coordZ);

	// Remove potentially prefetched data
	DataBase::instance().removePrefetchedDocument(faceStorageID);
}

osg::Node *SceneNodeMesh::createFaceNode(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ)
{
	try
	{
		auto triangles = view["Triangles"].get_document().view();

		auto nodes = triangles["Nodes"].get_array().value;

		// Read the number of triangles
		auto item = nodes.begin();
		size_t numberOfTriangles = DataBase::getIntFromArrayViewIterator(item);
		item++;

		// Now we need to read all triangles and determine how many subdivided triangles we need depending on the triangle type and refinement level
		size_t numberOfSubdividedTriangles = 0;

		for (unsigned long index = 0; index < numberOfTriangles; index++)
		{
			size_t triangleNodeCount = DataBase::getIntFromArrayViewIterator(item);
			item++;

			// Now skip all the nodes
			for (int i = 0; i < triangleNodeCount; i++) item++;

			switch (triangleNodeCount)
			{
			case 3: // First order triangle, no subdivision necessary
				numberOfSubdividedTriangles += 1;
				break;
			case 6: // Second order triangle, number of new triangles depent on subdivision (each triangle is split into four for each refinementLevel > 0)
				numberOfSubdividedTriangles += 4;
				break;
			case 10: // Third order triangle, number of new triangles depend on subdivision (each triangle is split into four for each refinementLevel > 0)
				numberOfSubdividedTriangles += 9;
				break;
			default:
				assert(0); // Unknown triangle order
				return nullptr;
			}
		}

		// Allocate memory for nodes and normals, three per triangle
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numberOfSubdividedTriangles * 3);
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(numberOfSubdividedTriangles * 3);

		// Now go through the triangles and add their nodes

		item = nodes.begin();
		item++; // Skip the triangle count

		size_t nVertex = 0;
		size_t nNormal = 0;

		for (unsigned long index = 0; index < numberOfTriangles; index++)
		{
			addTriangleVerticesAndNormals(item, vertices, normals, nVertex, nNormal);
		}

		// Based on the vertices and normals, finally create the scene graph node consisting of the triangle list

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
		colors->push_back(osg::Vec4(0.5, 0.5, 0.5, m_transparency));

		newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(2.0f, 2.0f));

		newGeometry->setVertexArray(vertices);

		newGeometry->setNormalArray(normals);
		newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		newGeometry->setColorArray(colors);
		newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, numberOfSubdividedTriangles * 3));

		// Now create the geometry node and assign the drawable
		osg::Geode *triangleNode = new osg::Geode;
		triangleNode->addDrawable(newGeometry);

		return triangleNode;
	}
	catch (std::exception)
	{
		return createFaceNodeBackwardCompatible(view, coordX, coordY, coordZ);
	}

	return nullptr;
}

void SceneNodeMesh::addTriangleVerticesAndNormals(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal)
{
	size_t triangleNodeCount = DataBase::getIntFromArrayViewIterator(item);
	item++;

	switch (triangleNodeCount)
	{
	case 3: 
		addTriangleVerticesAndNormalsOrder1(item, vertices, normals, nVertex, nNormal);
		break;
	case 6:
		addTriangleVerticesAndNormalsOrder2(item, vertices, normals, nVertex, nNormal);
		break;
	case 10:
		addTriangleVerticesAndNormalsOrder3(item, vertices, normals, nVertex, nNormal);
		break;
	}
}

void SceneNodeMesh::addTriangleEdges(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap)
{
	size_t triangleNodeCount = DataBase::getIntFromArrayViewIterator(item);
	item++;

	switch (triangleNodeCount)
	{
	case 3:
		addTriangleEdgesOrder1(item, edgeMap);
		break;
	case 6:
		addTriangleEdgesOrder2(item, edgeMap);
		break;
	case 10:
		addTriangleEdgesOrder3(item, edgeMap);
		break;
	}
}

void SceneNodeMesh::addTriangle(size_t n1Index, size_t n2Index, size_t n3Index, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal)
{
	vertices->at(nVertex).set(coordX[n1Index], coordY[n1Index], coordZ[n1Index]);
	vertices->at(nVertex + 1).set(coordX[n2Index], coordY[n2Index], coordZ[n2Index]);
	vertices->at(nVertex + 2).set(coordX[n3Index], coordY[n3Index], coordZ[n3Index]);

	nVertex += 3;

	double vector1[3] = { coordX[n2Index] - coordX[n1Index],  coordY[n2Index] - coordY[n1Index] , coordZ[n2Index] - coordZ[n1Index] };
	double vector2[3] = { coordX[n3Index] - coordX[n1Index],  coordY[n3Index] - coordY[n1Index] , coordZ[n3Index] - coordZ[n1Index] };

	double nx = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	double ny = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	double nz = vector1[0] * vector2[1] - vector1[1] * vector2[0];

	double nabs = sqrt(nx * nx + ny * ny + nz * nz);

	nx /= nabs;
	ny /= nabs;
	nz /= nabs;

	normals->at(nNormal).set(nx, ny, nz);
	normals->at(nNormal + 1).set(nx, ny, nz);
	normals->at(nNormal + 2).set(nx, ny, nz);

	nNormal += 3;
}

void SceneNodeMesh::averageNormals(size_t n1Index, size_t n2Index, size_t n3Index, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nNormalBase)
{
	osg::Vec3 normal = (normals->at(nNormalBase + n1Index) + normals->at(nNormalBase + n2Index) + normals->at(nNormalBase + n3Index)) / 3.0;

	normals->at(nNormalBase + n1Index).set(normal);
	normals->at(nNormalBase + n2Index).set(normal);
	normals->at(nNormalBase + n3Index).set(normal);
}

void SceneNodeMesh::averageNormals(size_t n1Index, size_t n2Index, size_t n3Index, size_t n4Index, size_t n5Index, size_t n6Index, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nNormalBase)
{
	osg::Vec3 normal = (  normals->at(nNormalBase + n1Index) + normals->at(nNormalBase + n2Index) + normals->at(nNormalBase + n3Index)
		                + normals->at(nNormalBase + n4Index) + normals->at(nNormalBase + n5Index) + normals->at(nNormalBase + n6Index)) / 6.0;

	normals->at(nNormalBase + n1Index).set(normal);
	normals->at(nNormalBase + n2Index).set(normal);
	normals->at(nNormalBase + n3Index).set(normal);
	normals->at(nNormalBase + n4Index).set(normal);
	normals->at(nNormalBase + n5Index).set(normal);
	normals->at(nNormalBase + n6Index).set(normal);
}

void SceneNodeMesh::addTriangleVerticesAndNormalsOrder1(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal)
{
	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addTriangle(n1Index, n2Index, n3Index, vertices, normals, nVertex, nNormal);
}

void SceneNodeMesh::addTriangleVerticesAndNormalsOrder2(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal)
{
	size_t nNormalBase = nNormal;

	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n4Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n5Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n6Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addTriangle(n1Index, n4Index, n6Index, vertices, normals, nVertex, nNormal);
	addTriangle(n4Index, n2Index, n5Index, vertices, normals, nVertex, nNormal);
	addTriangle(n4Index, n5Index, n6Index, vertices, normals, nVertex, nNormal);
	addTriangle(n6Index, n5Index, n3Index, vertices, normals, nVertex, nNormal);

	averageNormals(1, 3,  6, normals, nNormalBase);
	averageNormals(5, 7, 10, normals, nNormalBase);
	averageNormals(2, 8,  9, normals, nNormalBase);
}

void SceneNodeMesh::addTriangleVerticesAndNormalsOrder3(bsoncxx::array::view::const_iterator &item, osg::ref_ptr<osg::Vec3Array> &vertices, osg::ref_ptr<osg::Vec3Array> &normals, size_t &nVertex, size_t &nNormal)
{
	size_t nNormalBase = nNormal;

	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n4Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n5Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n6Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n7Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n8Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n9Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n10Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addTriangle(n1Index, n4Index, n9Index, vertices, normals, nVertex, nNormal);
	addTriangle(n4Index, n5Index, n10Index, vertices, normals, nVertex, nNormal);
	addTriangle(n5Index, n2Index, n6Index, vertices, normals, nVertex, nNormal);
	addTriangle(n4Index, n10Index, n9Index, vertices, normals, nVertex, nNormal);
	addTriangle(n5Index, n6Index, n10Index, vertices, normals, nVertex, nNormal);
	addTriangle(n9Index, n10Index, n8Index, vertices, normals, nVertex, nNormal);
	addTriangle(n10Index, n6Index, n7Index, vertices, normals, nVertex, nNormal);
	addTriangle(n10Index, n7Index, n8Index, vertices, normals, nVertex, nNormal);
	addTriangle(n8Index, n7Index, n3Index, vertices, normals, nVertex, nNormal);

	averageNormals(1, 3, 9, normals, nNormalBase);
	averageNormals(4, 6, 12, normals, nNormalBase);
	averageNormals(2, 11, 15, normals, nNormalBase);
	averageNormals(8, 13, 19, normals, nNormalBase);
	averageNormals(17, 23, 24, normals, nNormalBase);
	averageNormals(20, 22, 25, normals, nNormalBase);
	averageNormals(5, 10, 14, 16, 18, 21, normals, nNormalBase);
}

void SceneNodeMesh::addTriangleEdgesOrder1(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap)
{
	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addEdge(n1Index, n2Index, edgeMap);
	addEdge(n2Index, n3Index, edgeMap);
	addEdge(n3Index, n1Index, edgeMap);
}

void SceneNodeMesh::addTriangleEdgesOrder2(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap)
{
	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n4Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n5Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n6Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addEdge(n1Index, n4Index, edgeMap);
	addEdge(n4Index, n2Index, edgeMap);

	addEdge(n1Index, n6Index, edgeMap);
	addEdge(n6Index, n3Index, edgeMap);

	addEdge(n2Index, n5Index, edgeMap);
	addEdge(n5Index, n3Index, edgeMap);
}

void SceneNodeMesh::addTriangleEdgesOrder3(bsoncxx::array::view::const_iterator &item, std::map<std::pair<size_t, size_t>, bool> &edgeMap)
{
	unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n4Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n5Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n6Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n7Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n8Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n9Index = DataBase::getIntFromArrayViewIterator(item); item++;
	unsigned long long n10Index = DataBase::getIntFromArrayViewIterator(item); item++;

	addEdge(n1Index, n4Index, edgeMap);
	addEdge(n4Index, n5Index, edgeMap);
	addEdge(n5Index, n2Index, edgeMap);

	addEdge(n1Index, n9Index, edgeMap);
	addEdge(n9Index, n8Index, edgeMap);
	addEdge(n8Index, n3Index, edgeMap);

	addEdge(n2Index, n6Index, edgeMap);
	addEdge(n6Index, n7Index, edgeMap);
	addEdge(n7Index, n3Index, edgeMap);
}

osg::Node *SceneNodeMesh::createFaceNodeBackwardCompatible(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ)
{
	auto triangles = view["Triangles"].get_document().view();

	auto node1 = triangles["Node1"].get_array().value;
	auto node2 = triangles["Node2"].get_array().value;
	auto node3 = triangles["Node3"].get_array().value;

	size_t numberTriangles = std::distance(node1.begin(), node1.end());
	assert(numberTriangles == std::distance(node2.begin(), node2.end()));
	assert(numberTriangles == std::distance(node3.begin(), node3.end()));

	auto n1 = node1.begin();
	auto n2 = node2.begin();
	auto n3 = node3.begin();

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numberTriangles * 3);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(numberTriangles * 3);

	// Now store the triangle vertices in the nodes and normals array

	unsigned long long nVertex = 0;
	unsigned long long nNormal = 0;

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(n1);
		unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(n2);
		unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(n3);

		vertices->at(nVertex    ).set(coordX[n1Index], coordY[n1Index], coordZ[n1Index]);
		vertices->at(nVertex + 1).set(coordX[n2Index], coordY[n2Index], coordZ[n2Index]);
		vertices->at(nVertex + 2).set(coordX[n3Index], coordY[n3Index], coordZ[n3Index]);

		nVertex += 3;

		double vector1[3] = { coordX[n2Index] - coordX[n1Index],  coordY[n2Index] - coordY[n1Index] , coordZ[n2Index] - coordZ[n1Index] };
		double vector2[3] = { coordX[n3Index] - coordX[n1Index],  coordY[n3Index] - coordY[n1Index] , coordZ[n3Index] - coordZ[n1Index] };

		double nx = vector1[1] * vector2[2] - vector1[2] * vector2[1];
		double ny = vector1[2] * vector2[0] - vector1[0] * vector2[2];
		double nz = vector1[0] * vector2[1] - vector1[1] * vector2[0];

		double nabs = sqrt(nx * nx + ny * ny + nz * nz);

		nx /= nabs;
		ny /= nabs;
		nz /= nabs;

		normals->at(nNormal    ).set(nx, ny, nz);
		normals->at(nNormal + 1).set(nx, ny, nz);
		normals->at(nNormal + 2).set(nx, ny, nz);

		nNormal += 3;

		n1++;
		n2++;
		n3++;
	}

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
	colors->push_back(osg::Vec4(0.5, 0.5, 0.5, m_transparency));

	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(2.0f, 2.0f));

	newGeometry->setVertexArray(vertices);

	newGeometry->setNormalArray(normals);
	newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, numberTriangles * 3));

	// Now create the geometry node and assign the drawable
	osg::Geode *triangleNode = new osg::Geode;
	triangleNode->addDrawable(newGeometry);

	return triangleNode;
}

void SceneNodeMesh::addEdge(size_t p1, size_t p2, std::map<std::pair<size_t, size_t>, bool> &edgeMap)
{
	if (p1 > p2)
	{
		size_t help = p1;
		p1 = p2;
		p2 = help;
	}

	std::pair<size_t, size_t> edge(p1, p2);

	edgeMap[edge] = true;
}

osg::Node *SceneNodeMesh::createEdgeNode(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ)
{
	try
	{
		auto triangles = view["Triangles"].get_document().view();

		auto nodes = triangles["Nodes"].get_array().value;

		// Read the number of triangles
		auto item = nodes.begin();
		size_t numberOfTriangles = DataBase::getIntFromArrayViewIterator(item);
		item++;

		// Now we need to read all triangles and determine their subdivided edges
		std::map<std::pair<size_t, size_t>, bool> edgeMap;

		for (unsigned long index = 0; index < numberOfTriangles; index++)
		{
			addTriangleEdges(item, edgeMap);
		}

		// Based on the edges, finally create the scene graph node consisting of the edges

		size_t numberEdges = edgeMap.size();

		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numberEdges * 2);

		unsigned long long nVertex = 0;

		for (auto edge : edgeMap)
		{
			size_t p1 = edge.first.first;
			size_t p2 = edge.first.second;

			vertices->at(nVertex).set(coordX[p1], coordY[p1], coordZ[p1]);
			vertices->at(nVertex + 1).set(coordX[p2], coordY[p2], coordZ[p2]);

			nVertex += 2;
		}

		// Now create the osg node for the edges

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(edgeColorRGB[0], edgeColorRGB[1], edgeColorRGB[2], 1.0f));

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

		newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numberEdges * 2));

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
	catch (std::exception)
	{
		return createEdgeNodeBackwardCompatible(view, coordX, coordY, coordZ);
	}

	return nullptr;
}

osg::Node *SceneNodeMesh::createEdgeNodeBackwardCompatible(bsoncxx::document::view view, double *coordX, double *coordY, double *coordZ)
{
	// First create a list with all edges. Each edge is represented as a pait with the smaller coordinate index first and the larger
	// coordinate index second

	std::map<std::pair<size_t, size_t>, bool> edgeMap;

	auto triangles = view["Triangles"].get_document().view();

	auto node1 = triangles["Node1"].get_array().value;
	auto node2 = triangles["Node2"].get_array().value;
	auto node3 = triangles["Node3"].get_array().value;

	size_t numberTriangles = std::distance(node1.begin(), node1.end());
	assert(numberTriangles == std::distance(node2.begin(), node2.end()));
	assert(numberTriangles == std::distance(node3.begin(), node3.end()));

	auto n1 = node1.begin();
	auto n2 = node2.begin();
	auto n3 = node3.begin();

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		unsigned long long n1Index = DataBase::getIntFromArrayViewIterator(n1);
		unsigned long long n2Index = DataBase::getIntFromArrayViewIterator(n2);
		unsigned long long n3Index = DataBase::getIntFromArrayViewIterator(n3);

		addEdge(n1Index, n2Index, edgeMap);
		addEdge(n2Index, n3Index, edgeMap);
		addEdge(n3Index, n1Index, edgeMap);

		n1++;
		n2++;
		n3++;
	}

	size_t numberEdges = edgeMap.size();

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numberEdges * 2);

	unsigned long long nVertex = 0;

	for (auto edge : edgeMap)
	{
		size_t p1 = edge.first.first;
		size_t p2 = edge.first.second;

		vertices->at(nVertex    ).set(coordX[p1], coordY[p1], coordZ[p1]);
		vertices->at(nVertex + 1).set(coordX[p2], coordY[p2], coordZ[p2]);

		nVertex += 2;
	}

	// Now create the osg node for the edges

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(edgeColorRGB[0], edgeColorRGB[1], edgeColorRGB[2], 1.0f));

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

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numberEdges * 2));

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

void SceneNodeMesh::addOwner(SceneNodeMeshItem *item, int faceID)
{
	bool forward = (faceID > 0);
	faceID = abs(faceID);

	std::pair<bool, SceneNodeMeshItem*> info(forward, item);

	faceOwners[faceID].push_back(info);
}

void SceneNodeMesh::updateFaceStatus(const std::vector<int> &faceID)
{
	for (auto signedFace : faceID)
	{
		int face = abs(signedFace);

		// Here we need to check the visibility status for each face
		assert(faceOwners.count(face) >= 0);

		if (faceOwners[face].size() == 0)
		{
			// This face has no owner -> it belongs to a mesh item which was deleted -> the face is hidden
			setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, nullptr);
		}
		else if (faceOwners[face].size() == 1)
		{
			// The face has one owner
			bool forward            = faceOwners[face].at(0).first;
			SceneNodeMeshItem *item = faceOwners[face].at(0).second;

			// The face is visible of the owner is visible
			setFaceStatus(face, item->isVisible(), forward, !item->getIsVolume(), item->isTransparent(), item->isWireframe(), item->getColorR(), item->getColorG(), item->getColorB(), item);
		}
		else if (faceOwners[face].size() == 2)
		{
			bool forward1 = faceOwners[face].at(0).first;
			SceneNodeMeshItem *item1 = faceOwners[face].at(0).second;

			bool forward2 = faceOwners[face].at(1).first;
			SceneNodeMeshItem *item2 = faceOwners[face].at(1).second;

			bool drawOpaqueItem1 = (item1->isVisible() && !item1->isTransparent());
			bool drawOpaqueItem2 = (item2->isVisible() && !item2->isTransparent());

			if (drawOpaqueItem1 && drawOpaqueItem2)
			{
				// Both items are visible -> the face is hidden (unless we have a wireframe display)
				if (isWireframe())
				{
					// Here, we just draw the first face
					setFaceStatus(face, item1->isVisible(), forward1, !item1->getIsVolume(), false, item1->isWireframe(), item1->getColorR(), item1->getColorG(), item1->getColorB(), item1);
				}
				else
				{
					// The face is hidden inside the object and therefore invisible
					setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, nullptr);
				}
			}
			else if (drawOpaqueItem1)
			{
				setFaceStatus(face, item1->isVisible(), forward1, !item1->getIsVolume(), false, item1->isWireframe(), item1->getColorR(), item1->getColorG(), item1->getColorB(), item1);
			}
			else if (drawOpaqueItem2)
			{
				setFaceStatus(face, item2->isVisible(), forward2, !item2->getIsVolume(), false, item2->isWireframe(), item2->getColorR(), item2->getColorG(), item2->getColorB(), item2);
			}
			else
			{
				// Here it could be that both faces are visible. In this case, we do not draw the interior face. Otherwise we draw the face transparently

				if (!item1->isVisible() && !item2->isVisible())
				{
					// None of the two items is visible -> hide the face
					setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, nullptr);
				}
				else if (item1->isVisible() && item2->isVisible())
				{
					// Both items are visible -> hide the face
					setFaceStatus(face, false, false, false, false, false, 0.0, 0.0, 0.0, nullptr);
				}
				else
				{
					if (item1->isVisible())
					{
						setFaceStatus(face, true, forward1, !item1->getIsVolume(), true, item1->isWireframe(), item1->getColorR(), item1->getColorG(), item1->getColorB(), item1);
					}
					else if (item2->isVisible())
					{
						setFaceStatus(face, true, forward2, !item2->getIsVolume(), true, item2->isWireframe(), item2->getColorR(), item2->getColorG(), item2->getColorB(), item2);
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

void SceneNodeMesh::setFaceStatus(int face, bool visible, bool forward, bool doublesided, bool transparent, bool wireframe, double r, double g, double b, SceneNodeBase *owner)
{
	// Check whether face status has really changed

	bool visibilityChanged  = true;
	bool forwardChanged     = true;
	bool doublesidedChanged = true;
	bool transparentChanged = true;
	bool wireframeChanged   = true;
	bool colorChanged       = true;

	if (faceStatusCache.count(face) > 0)
	{
		FaceStatus statusCache = faceStatusCache[face];

		if (!statusCache.visible && !visible) return;  // The face was not visible and is still not visible

		if (statusCache.visible == visible) visibilityChanged = false;
		if (statusCache.forward == forward) forwardChanged = false;
		if (statusCache.doublesided == doublesided) doublesidedChanged = false;
		if (statusCache.transparent == transparent) transparentChanged = false;
		if (statusCache.wireframe == wireframe) wireframeChanged = false;
		if (fabs(statusCache.r - r) < 1e-10 && fabs(statusCache.g - g) < 1e-10 && fabs(statusCache.b - b) < 1e-10) colorChanged = false;

		if (!visibilityChanged && !forwardChanged && !doublesidedChanged && !transparentChanged && !wireframeChanged && !colorChanged)
		{
			// The face has not changed
			return;
		}
	}

	// The face has changed. Cache the new settings

	FaceStatus newStatusCache;
	newStatusCache.visible     = visible;
	newStatusCache.forward     = forward;
	newStatusCache.doublesided = doublesided;
	newStatusCache.transparent = transparent;
	newStatusCache.wireframe   = wireframe;
	newStatusCache.r           = r;
	newStatusCache.g           = g;
	newStatusCache.b           = b;

	faceStatusCache[face] = newStatusCache;

	// Now modify the face status according to the new settings

	osg::Node *faceNode = faceTriangles[face];
	assert(faceNode != nullptr);

	osg::Node *edgeNode = faceEdges[face];
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
		sceneNodeMaterial.setMaterial(material, "Rough", r, g, b, m_transparency);

		faceGeometry->getOrCreateStateSet()->setAttribute(material);

		faceRefreshNeeded = true;
	}

	if (colorChanged || wireframeChanged)
	{
		// Modify the color of the edge
		osg::Vec4Array *edgeColorArray = dynamic_cast<osg::Vec4Array *>(edgeGeometry->getColorArray());
		if (edgeColorArray != nullptr)
		{
			edgeColorArray->at(0) = wireframe ? osg::Vec4(r, g, b, 1.0f) : osg::Vec4(edgeColorRGB[0], edgeColorRGB[1], edgeColorRGB[2], 1.0f);
			edgeColorArray->dirty();
		}
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

void SceneNodeMesh::setTransparent(osg::Geometry *faceGeometry, bool transparent)
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

void SceneNodeMesh::removeOwner(SceneNodeMeshItem *item, const std::vector<int> &faceID)
{
	for (auto face : faceID)
	{
		// Remove this owner from each face
		if (faceOwners.find(abs(face)) != faceOwners.end())
		{
			std::vector < std::pair<bool, SceneNodeMeshItem*> > ownerList = faceOwners[abs(face)];

			std::vector < std::pair<bool, SceneNodeMeshItem*> > newOwnerList;

			for (auto p : ownerList)
			{
				if (p.second != item) newOwnerList.push_back(p);
			}

			faceOwners[abs(face)] = newOwnerList;
		}
	}

	updateFaceStatus(faceID);
}

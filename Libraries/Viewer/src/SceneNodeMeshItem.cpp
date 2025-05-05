#include "stdafx.h"

#include "SceneNodeMeshItem.h"
#include "SceneNodeMesh.h"
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

#undef min
#undef max 

#include "Connection\ConnectionAPI.h"

#include <mongocxx/client.hpp>

SceneNodeMeshItem::SceneNodeMeshItem() :
	entityID(0),
	entityVersion(0),
	needsInitialization(true),
	model(nullptr),
	mesh(nullptr),
	colorRGB{0.0, 0.0, 0.0},
	isVolume(false),
	meshDataTetEdgesID(0),
	meshDataTetEdgesVersion(0),
	tetEdges(nullptr)
{

}


SceneNodeMeshItem::~SceneNodeMeshItem()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	// loop through all parent nodes
	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group *parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		m_shapeNode = nullptr;
	}

	getMesh()->removeOwner(this, faceID);
}

void SceneNodeMeshItem::ensureDataLoaded(void)
{
	if (!needsInitialization) return;
	needsInitialization = false;

	assert(getMesh() != nullptr);
	getMesh()->ensureDataLoaded();

	// Now load the data of the item first

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, entityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshTetItem")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityMeshTetItem");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Set the color

	colorRGB[0] = doc_view["ColorR"].get_double();
	colorRGB[1] = doc_view["ColorG"].get_double();
	colorRGB[2] = doc_view["ColorB"].get_double();

	// Determine whether we have a volume
	isVolume = (DataBase::GetIntFromView(doc_view, "NumberTets") > 0);

	// Read information about the actual face storage

	auto faces = doc_view["Faces"].get_array().value;

	size_t numberFaces = std::distance(faces.begin(), faces.end());
	faceID.resize(numberFaces);

	auto face = faces.begin();

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		faceID[index] = DataBase::GetIntFromArrayViewIterator(face);
		getMesh()->addOwner(this, faceID[index]);

		face++;
	}

	// Read information about the internal edges storage
	meshDataTetEdgesID = DataBase::GetIntFromView(doc_view, "MeshDataTetEdgesID", 0);
	meshDataTetEdgesVersion = DataBase::GetIntFromView(doc_view, "MeshDataTetEdgesVersion", 0);

	if (getMesh()->getDisplayTetEdges() && meshDataTetEdgesID > 0)
	{
		loadEdgeData();
	}

	updateVisibility();
}

void SceneNodeMeshItem::updateVisibility(void)
{
	getMesh()->updateFaceStatus(faceID);
}

void SceneNodeMeshItem::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
	updateVisibility();

	if (tetEdges != nullptr)
	{
		getShapeNode()->setChildValue(tetEdges, getMesh()->getDisplayTetEdges() && isVisible() && !isTransparent() && isWireframe());
	}

}

void SceneNodeMeshItem::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
	updateVisibility();

	if (tetEdges != nullptr)
	{
		getShapeNode()->setChildValue(tetEdges, getMesh()->getDisplayTetEdges() && isVisible() && !isTransparent() && isWireframe());
	}
}

void SceneNodeMeshItem::setVisible(bool v)
{
	if (v == isVisible()) return;

	if (v)
	{
		ensureDataLoaded();
	}

	SceneNodeBase::setVisible(v);
	updateVisibility();

	if (tetEdges != nullptr)
	{
		getShapeNode()->setChildValue(tetEdges, getMesh()->getDisplayTetEdges() && isVisible() && !isTransparent() && isWireframe());
	}
}

void SceneNodeMeshItem::setHighlighted(bool h)
{
	return;
	if (h == isHighlighted()) return;

	SceneNodeBase::setHighlighted(h);
	updateVisibility();
}

void SceneNodeMeshItem::getPrefetch(std::string &projName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	SceneNodeBase::getPrefetch(projName, prefetchIDs);

	if (needsInitialization)
	{
		if (projName.empty())
		{
			projName = projectName;
		}
		else if (projName != projectName)
		{
			// We have a different project name here. No prefetching.
			assert(0);
			return;
		}

		prefetchIDs.push_back(std::pair<unsigned long long, unsigned long long>(entityID, entityVersion));
	}
	
	if (tetEdges == nullptr && getMesh()->getDisplayTetEdges() && meshDataTetEdgesID > 0)
	{
		prefetchIDs.push_back(std::pair<unsigned long long, unsigned long long>(meshDataTetEdgesID, meshDataTetEdgesVersion));
	}
}

void SceneNodeMeshItem::loadEdgeData(void)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(meshDataTetEdgesID, meshDataTetEdgesVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityMeshTetItemDataTetedges")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityMeshTetItemDataTetedges");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Check how the data is stored
	bool readGridFS = true;

	try
	{
		doc_view["FileId"].get_oid();
	}
	catch (std::exception)
	{
		readGridFS = false;
	}

	size_t numberEdges = 0;
	osg::ref_ptr<osg::Vec3Array> vertices;

	if (readGridFS)
	{
		numberEdges = doc_view["NumberEdges"].get_int64();
		auto fileId = doc_view["FileId"].get_value();

		auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
		auto bucket = db.gridfs_bucket();

		auto downloader = bucket.open_download_stream(fileId);
		size_t fileLength = downloader.file_length();

		assert(fileLength == 2 * sizeof(size_t) * numberEdges);

		size_t *buffer = new size_t[numberEdges];
		vertices = new osg::Vec3Array(numberEdges * 2);

		for (int nodeIndex = 0; nodeIndex < 2; nodeIndex++)
		{
			downloader.read((uint8_t *)buffer, numberEdges * sizeof(size_t) / sizeof(uint8_t));

			for (size_t index = 0; index < numberEdges; index++)
			{
				size_t node = buffer[index];
				vertices->at(2 * index + nodeIndex).set(getMesh()->getNodeX(node), getMesh()->getNodeY(node), getMesh()->getNodeZ(node));
			}
		}

		downloader.close();

		delete[] buffer;
		buffer = nullptr;
	}
	else
	{
		// Now load the nodes
		auto node1 = doc_view["Node1"].get_array().value;
		auto node2 = doc_view["Node2"].get_array().value;

		numberEdges = std::distance(node1.begin(), node1.end());
		assert(numberEdges == std::distance(node2.begin(), node2.end()));

		auto n1 = node1.begin();
		auto n2 = node2.begin();

		vertices = new osg::Vec3Array(numberEdges * 2);
		unsigned long long nVertex = 0;

		for (unsigned long eindex = 0; eindex < numberEdges; eindex++)
		{
			size_t node1 = (size_t)DataBase::GetIntFromArrayViewIterator(n1);
			size_t node2 = (size_t)DataBase::GetIntFromArrayViewIterator(n2);

			vertices->at(nVertex).set(getMesh()->getNodeX(node1), getMesh()->getNodeY(node1), getMesh()->getNodeZ(node1));
			vertices->at(nVertex + 1).set(getMesh()->getNodeX(node2), getMesh()->getNodeY(node2), getMesh()->getNodeZ(node2));

			nVertex += 2;

			n1++;
			n2++;
		}
	}

	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

	// Store the color in a color array (the color will be shared among all nodes, so only one entry is needed)
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(colorRGB[0], colorRGB[1], colorRGB[2], 1.0f));

	newGeometry->setVertexArray(vertices.get());

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numberEdges * 2));

	// Now create the geometry node and assign the drawable
	osg::Geode *edgesNode = new osg::Geode;
	edgesNode->addDrawable(newGeometry);

	osg::StateSet *ss = edgesNode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	assert(m_shapeNode == nullptr);
	m_shapeNode = new osg::Switch;

	// Now add the current nodes osg node to the parent's osg node
	getParent()->getShapeNode()->addChild(getShapeNode());
	getModel()->addSceneNode(this);

	m_shapeNode->addChild(edgesNode);

	tetEdges = edgesNode;
}

void SceneNodeMeshItem::setDisplayTetEdges(bool displayEdges)
{
	if (!displayEdges)
	{
		if (tetEdges != nullptr)
		{
			getShapeNode()->setChildValue(tetEdges, false);
		}
	}
	else
	{
		if (meshDataTetEdgesID > 0 && tetEdges == nullptr)
		{
			loadEdgeData();
		}

		if (tetEdges != nullptr)
		{
			getShapeNode()->setChildValue(tetEdges, isVisible() && !isTransparent() && isWireframe());
		}
	}
}

void SceneNodeMeshItem::updateObjectColor(double r, double g, double b) 
{
	colorRGB[0] = r; 
	colorRGB[1] = g; 
	colorRGB[2] = b;

	// First, update the color of the volume edges (if present)
	osg::Geode* edgesNode = dynamic_cast<osg::Geode*>(tetEdges);

	if (edgesNode != nullptr)
	{
		for (int i = 0; i < edgesNode->getNumDrawables(); i++)
		{
			osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(edgesNode->getDrawable(i));

			if (geometry != nullptr)
			{
				osg::Vec4Array* colorArray = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());
				assert(colorArray != nullptr);

				colorArray->at(0).set(colorRGB[0], colorRGB[1], colorRGB[2], 1.0f);
				geometry->dirtyGLObjects();
			}
		}
	}

	// Now update the colors of the faces
	if (mesh != nullptr)
	{
		mesh->updateFaceStatus(faceID);
	}
}

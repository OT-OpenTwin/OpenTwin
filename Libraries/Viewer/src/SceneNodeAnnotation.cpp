#include "stdafx.h"

#include "SceneNodeAnnotation.h"
#include "ViewerSettings.h"

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
#include <osg/Point>
#include <osg/Depth>

SceneNodeAnnotation::~SceneNodeAnnotation()
{

}

void SceneNodeAnnotation::setTransparent(bool t)
{
	if (isTransparent() == t) return;  // No change necessary

	SceneNodeBase::setTransparent(t);

	if (isTransparent())
	{
		// Set node to transparent state

		if (triangles != nullptr)
		{
			osg::StateSet *stateset = triangles->getOrCreateStateSet();
			stateset->clear();

			osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
			blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			stateset->setAttributeAndModes(blendFunc);
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			osg::ref_ptr<osg::Depth> depth = new osg::Depth;
			depth->setWriteMask(false);
			stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

			osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
			cull->setMode(osg::CullFace::BACK);
			stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);

			// Turn on triangle display (is shape is not hidden)
			if (isVisible())
			{
				getShapeNode()->setChildValue(triangles, true);
			}
		}

		// Turn off edge display
		if (edges != nullptr) getShapeNode()->setChildValue(edges, false);

		// Turn off vertex display
		if (vertices != nullptr) getShapeNode()->setChildValue(vertices, false);
	}
	else
	{
		// Set node to opaque state

		if (triangles != nullptr)
		{
			osg::StateSet *stateset = triangles->getOrCreateStateSet();
			stateset->clear();

			osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
			cull->setMode(osg::CullFace::BACK);
			stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);

			// Turn on triangle display (is shape is not hidden and we are not in wireframe state)
			getShapeNode()->setChildValue(triangles, isVisible() && !isWireframe());
		}

		// Turn on edge display (if visible)
		if (isVisible())
		{
			if (edges != nullptr) getShapeNode()->setChildValue(edges, true);
		}

		// Turn on vertex display (if visible)
		if (isVisible())
		{
			if (vertices != nullptr) getShapeNode()->setChildValue(vertices, true);
		}
	}
}

void SceneNodeAnnotation::setWireframe(bool w)
{
	// The annotations are ignoring the wireframe state. Therefore, they become clearly visible when the wireframe mode is switched on.
}

void SceneNodeAnnotation::setVisible(bool v)
{
	if (isVisible() == v) return;  // No change necessary

	SceneNodeBase::setVisible(v);

	if (isVisible())
	{
		// Set node to visible state

		if (!isTransparent())
		{
			if (edges    != nullptr) getShapeNode()->setChildValue(edges, true);
			if (vertices != nullptr) getShapeNode()->setChildValue(vertices, true);
		}

		if (!isWireframe())
		{
			if (triangles != nullptr) getShapeNode()->setChildValue(triangles, true);
		}
	}
	else
	{
		// Set node to hidden state
		if (edges     != nullptr) getShapeNode()->setChildValue(edges, false);
		if (vertices  != nullptr) getShapeNode()->setChildValue(vertices, false);
		if (triangles != nullptr) getShapeNode()->setChildValue(triangles, false);
	}
}

void SceneNodeAnnotation::setHighlighted(bool h)
{
	if (isHighlighted() == h) return;  // No change necessary

	if (edgesHighlighted    != nullptr) getShapeNode()->setChildValue(edgesHighlighted, h);
	if (verticesHighlighted != nullptr) getShapeNode()->setChildValue(verticesHighlighted, h);

	SceneNodeBase::setHighlighted(h);
}

void SceneNodeAnnotation::initializeFromData(const double edgeColorRGB[3], 
											 const std::vector<std::array<double, 3>> &points,
											 const std::vector<std::array<double, 3>> &points_rgb,
											 const std::vector<std::array<double, 3>> &triangle_p1,
											 const std::vector<std::array<double, 3>> &triangle_p2,
											 const std::vector<std::array<double, 3>> &triangle_p3,
											 const std::vector<std::array<double, 3>> &triangle_rgb)
{
	// Add a switch (group) node for the shape
	if (m_shapeNode == nullptr)
	{
		// Create an new m_shapeNode
		m_shapeNode = new osg::Switch;
	}
	else
	{
		// Delete the children of the shape node
		while (m_shapeNode->removeChild((unsigned int)0));
	}

	// Create the triangle node
	osg::Node *triangleNode = createOSGNodeFromTriangles(triangle_p1, triangle_p2, triangle_p3, triangle_rgb);

	// Create the edges node
	osg::Node *edgeNode(nullptr), *edgeHighlightedNode(nullptr);

	createOSGNodeFromEdges(edgeColorRGB, triangle_p1, triangle_p2, triangle_p3, edgeNode, edgeHighlightedNode);

	// Create the vertices node
	osg::Node *vertexNode(nullptr), *vertexHighlightedNode(nullptr);

	createOSGNodeFromVertices(points, points_rgb, vertexNode, vertexHighlightedNode);

	// Create a new entry in the shape map for this object

	triangles			= triangleNode;
	edges				= edgeNode;
	edgesHighlighted	= edgeHighlightedNode;
	vertices			= vertexNode;
	verticesHighlighted = vertexHighlightedNode;

	// Add the triangle and edge nodes to the group node and add the group node to the root
	if (triangleNode          != nullptr) m_shapeNode->addChild(triangleNode);
	if (edgeNode              != nullptr) m_shapeNode->addChild(edgeNode);
	if (edgeHighlightedNode   != nullptr) m_shapeNode->addChild(edgeHighlightedNode);
	if (vertexNode            != nullptr) m_shapeNode->addChild(vertexNode);
	if (vertexHighlightedNode != nullptr) m_shapeNode->addChild(vertexHighlightedNode);

	// Ensure that the triangles are drawn opaquely
	setTransparent(false);

	// Turn off highlighting
	if (edgeHighlightedNode   != nullptr) m_shapeNode->setChildValue(edgeHighlightedNode, false);
	if (vertexHighlightedNode != nullptr) m_shapeNode->setChildValue(vertexHighlightedNode, false);
}

osg::Node * SceneNodeAnnotation::createOSGNodeFromTriangles(const std::vector<std::array<double, 3>> &triangle_p1,
															const std::vector<std::array<double, 3>> &triangle_p2,
															const std::vector<std::array<double, 3>> &triangle_p3,
															const std::vector<std::array<double, 3>> &triangle_rgb)
{
	// Allocate and dimensions arrays for nodes and normals (each triangle has three nodes - the nodes are not shared between adjacent triangles)
	unsigned long long nTriangles = triangle_p1.size();

	if (nTriangles == 0) return nullptr;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nTriangles * 3);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(nTriangles * 3);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(nTriangles * 3);

	// Now store the triangle vertices in the nodes and normals array

	unsigned long long nVertex = 0;
	unsigned long long nNormal = 0;
	unsigned long long nColor  = 0;

	for (unsigned long long nT = 0; nT < nTriangles; nT++)
	{
		vertices->at(nVertex    ).set(triangle_p1[nT][0], triangle_p1[nT][1], triangle_p1[nT][2]);
		vertices->at(nVertex + 1).set(triangle_p2[nT][0], triangle_p2[nT][1], triangle_p2[nT][2]);
		vertices->at(nVertex + 2).set(triangle_p3[nT][0], triangle_p3[nT][1], triangle_p3[nT][2]);

		nVertex += 3;

		// Determine the normal of the triangle
		double vector1[3], vector2[3], normal[3];
		vector1[0] = triangle_p2[nT][0] - triangle_p1[nT][0];
		vector1[1] = triangle_p2[nT][1] - triangle_p1[nT][1];
		vector1[2] = triangle_p2[nT][2] - triangle_p1[nT][2];

		vector2[0] = triangle_p3[nT][0] - triangle_p1[nT][0];
		vector2[1] = triangle_p3[nT][1] - triangle_p1[nT][1];
		vector2[2] = triangle_p3[nT][2] - triangle_p1[nT][2];

		calculateCrossProduct(vector1, vector2, normal);

		double normalLength = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		if (normalLength == 0.0) normalLength = 1.0;

		normal[0] /= normalLength;
		normal[1] /= normalLength;
		normal[2] /= normalLength;

		normals->at(nNormal    ).set(normal[0], normal[1], normal[2]);
		normals->at(nNormal + 1).set(normal[0], normal[1], normal[2]);
		normals->at(nNormal + 2).set(normal[0], normal[1], normal[2]);

		nNormal += 3;

		// Store the color in a color array (the color will be set per vertex, so we need to store the same color three times)
		colors->at(nColor    ).set(triangle_rgb[nT][0], triangle_rgb[nT][1], triangle_rgb[nT][2], m_transparency);
		colors->at(nColor + 1).set(triangle_rgb[nT][0], triangle_rgb[nT][1], triangle_rgb[nT][2], m_transparency);
		colors->at(nColor + 2).set(triangle_rgb[nT][0], triangle_rgb[nT][1], triangle_rgb[nT][2], m_transparency);

		nColor += 3;
	}

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;
	
	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	// Enable double sided display
	newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel *p = new osg::LightModel;
	p->setTwoSided(true);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(p);

	// Enable polygon offset
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));

	// Now set the vertices, the normal and the colors
	newGeometry->setVertexArray(vertices);

	newGeometry->setNormalArray(normals);
	newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, nTriangles * 3));

	// Now create the geometry node and assign the drawable
	osg::Geode *triangleNode = new osg::Geode;
	triangleNode->addDrawable(newGeometry);

	return triangleNode;
}

void SceneNodeAnnotation::calculateCrossProduct(double vector1[3], double vector2[3], double normal[3])
{
	normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];
}

void SceneNodeAnnotation::createOSGNodeFromEdges(const double colorRGB[3], 
												 const std::vector<std::array<double, 3>> &triangle_p1,
												 const std::vector<std::array<double, 3>> &triangle_p2,
												 const std::vector<std::array<double, 3>> &triangle_p3,
												 osg::Node *&edgesNode, osg::Node *&edgesHighlightedNode)
{
	// Allocate and dimensions the array for nodes (each edge has two nodes - the nodes are not shared between adjacent edges)

	// Count the number of simple edge segments
	unsigned long long nTriangles = triangle_p1.size();
	unsigned long long nEdges = 3 * nTriangles;

	if (nTriangles == 0) return;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nEdges * 2);

	// Now store the edge vertices in the array
	unsigned long long nVertex = 0;

	for (unsigned long long nT = 0; nT < nTriangles; nT++)
	{
		vertices->at(nVertex    ).set(triangle_p1[nT][0], triangle_p1[nT][1], triangle_p1[nT][2]);
		vertices->at(nVertex + 1).set(triangle_p2[nT][0], triangle_p2[nT][1], triangle_p2[nT][2]);

		vertices->at(nVertex + 2).set(triangle_p2[nT][0], triangle_p2[nT][1], triangle_p2[nT][2]);
		vertices->at(nVertex + 3).set(triangle_p3[nT][0], triangle_p3[nT][1], triangle_p3[nT][2]);

		vertices->at(nVertex + 4).set(triangle_p3[nT][0], triangle_p3[nT][1], triangle_p3[nT][2]);
		vertices->at(nVertex + 5).set(triangle_p1[nT][0], triangle_p1[nT][1], triangle_p1[nT][2]);

		nVertex += 6;
	}

	// Now build the osg nodes for the edge and the selected edge
	edgesNode = buildEdgesOSGNode(nEdges, vertices, colorRGB[0], colorRGB[1], colorRGB[2], true);
	edgesHighlightedNode = buildEdgesOSGNode(nEdges, vertices, 1.0, 0.0, 0.0, false);
}

osg::Node *SceneNodeAnnotation::buildEdgesOSGNode(unsigned long long nEdges, osg::ref_ptr<osg::Vec3Array> &vertices, double r, double g, double b, bool depthTest)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(r, g, b, 1.0f));

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

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, nEdges * 2));

	// Now create the geometry node and assign the drawable
	osg::Geode *edgesNode = new osg::Geode;
	edgesNode->addDrawable(newGeometry);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = edgesNode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	if (!depthTest)
	{
		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ss->setAttributeAndModes(depth, osg::StateAttribute::ON);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	}

	return edgesNode;
}

void SceneNodeAnnotation::createOSGNodeFromVertices(const std::vector<std::array<double, 3>> &points,
													const std::vector<std::array<double, 3>> &points_rgb,
													osg::Node *&verticesNode, osg::Node *&verticesHighlightedNode)
{
	const double pointSize = 10.0;

	unsigned long long nVertices = points.size();

	if (nVertices == 0) return;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nVertices);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(nVertices);

	osg::ref_ptr<osg::Vec4Array> highlightColor = new osg::Vec4Array;
	highlightColor->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0f));

	for (unsigned long long nV = 0; nV < nVertices; nV++)
	{
		vertices->at(nV).set(points[nV][0], points[nV][1], points[nV][2]);
		colors->at(nV).set(points_rgb[nV][0], points_rgb[nV][1], points_rgb[nV][2], 1.0f);
	}

	// Build vertex geometry and node

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> vertexGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		vertexGeometry->setUseDisplayList(false);
		vertexGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	vertexGeometry->setVertexArray(vertices);

	vertexGeometry->setColorArray(colors);
	vertexGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	vertexGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, nVertices));

	// Now create the geometry node for the vertices and assign the drawable
	verticesNode = new osg::Geode;
	dynamic_cast<osg::Geode *>(verticesNode)->addDrawable(vertexGeometry);
	verticesNode->setCullingActive(false);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = verticesNode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::Point(pointSize), osg::StateAttribute::ON);

	// Now build the highlighted vertex geometry and node

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> highlightVertexGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		highlightVertexGeometry->setUseDisplayList(false);
		highlightVertexGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	highlightVertexGeometry->setVertexArray(vertices);

	highlightVertexGeometry->setColorArray(highlightColor);
	highlightVertexGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	highlightVertexGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, nVertices));

	// Now create the geometry node for the vertices and assign the drawable
	verticesHighlightedNode = new osg::Geode;
	dynamic_cast<osg::Geode *>(verticesHighlightedNode)->addDrawable(highlightVertexGeometry);
	verticesHighlightedNode->setCullingActive(false);

	// Set the display attributes for the edges geometry
	osg::StateSet *ssHighlight = verticesHighlightedNode->getOrCreateStateSet();

	ssHighlight->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ssHighlight->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssHighlight->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
	ssHighlight->setAttribute(new osg::Point(pointSize), osg::StateAttribute::ON);

	ssHighlight->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	ssHighlight->setAttributeAndModes(depth, osg::StateAttribute::ON);
	ssHighlight->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

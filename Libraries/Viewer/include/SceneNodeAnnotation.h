#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "SceneNodeBase.h"

#include <string>
#include <array>

#include <osg/Array>

class SceneNodeAnnotation : public SceneNodeBase
{
public:
	SceneNodeAnnotation() : triangles(nullptr), edges(nullptr), edgesHighlighted(nullptr), vertices(nullptr), verticesHighlighted(nullptr) {};
	virtual ~SceneNodeAnnotation();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void initializeFromData(const double edgeColorRGB[3], 
							const std::vector<std::array<double, 3>> &points,
							const std::vector<std::array<double, 3>> &points_rgb,
							const std::vector<std::array<double, 3>> &triangle_p1,
							const std::vector<std::array<double, 3>> &triangle_p2,
							const std::vector<std::array<double, 3>> &triangle_p3,
							const std::vector<std::array<double, 3>> &triangle_rgb);

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return true; };

private:
	osg::Node * createOSGNodeFromTriangles(const std::vector<std::array<double, 3>> &triangle_p1,
										   const std::vector<std::array<double, 3>> &triangle_p2,
										   const std::vector<std::array<double, 3>> &triangle_p3,
										   const std::vector<std::array<double, 3>> &triangle_rgb);
	void calculateCrossProduct(double vector1[3], double vector2[3], double normal[3]);
	void createOSGNodeFromEdges(const double colorRGB[3],
								const std::vector<std::array<double, 3>> &triangle_p1,
								const std::vector<std::array<double, 3>> &triangle_p2,
								const std::vector<std::array<double, 3>> &triangle_p3,
							    osg::Node *&edgesNode, osg::Node *&edgesHighlightedNode);
	osg::Node *buildEdgesOSGNode(unsigned long long nEdges, osg::Vec3Array *vertices, double r, double g, double b, bool depthTest);
	void createOSGNodeFromVertices(const std::vector<std::array<double, 3>> &points,
								   const std::vector<std::array<double, 3>> &points_rgb,
								   osg::Node *&verticesNode, osg::Node *&verticesHighlightedNode);

	osg::Node   *triangles;
	osg::Node   *edges;
	osg::Node   *edgesHighlighted;
	osg::Node   *vertices;
	osg::Node   *verticesHighlighted;
};


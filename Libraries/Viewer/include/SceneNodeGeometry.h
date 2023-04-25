#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "Viewer.h"
#include "SceneNodeBase.h"
#include <osg/Material>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/Matrix>

#include <string>
#include <osg/Array>

//#include "SceneNodeMaterial.h"

class Model;
class Viewer;
//class SceneNodeMaterial;
//class SceneNodeMaterialConcrete;

class SceneNodeGeometry : public SceneNodeBase
{
public:
	SceneNodeGeometry();
	virtual ~SceneNodeGeometry();

	void setTriangles(osg::Node *t) { triangles = t; };
	void setEdges(osg::Node *e) { edges = e; };
	void setEdgesHighlighted(osg::Node *e) { edgesHighlighted = e; };
	void setFaceEdgesHighlighted(osg::Switch *e) { faceEdgesHighlightNode = e; };
	void setMaterialType(const std::string &material) { materialType = material; }
	void setTextureType(const std::string &texture, bool reflect) { textureType = texture; reflective = reflect; }
	void setMaterialProperties(osg::ref_ptr<osg::Material>& mat,double r , double g, double b);
	void setTransformation(std::vector<double> &transformation);

	osg::Matrix getTransformation(void);
	osg::Matrix getParentTransformation(void);
	osg::Matrix getOwnTransformation(void) { return transformationMatrix; }
	void		setOwnTransformation(const osg::Matrix &matrix) { transformationMatrix = matrix; }

	void applyTransform(osg::Matrix matrix);

	osg::Node *getTriangles(void) { return triangles; };
	osg::Node *getEdges(void) { return edges; };
	osg::Node *getEdgesHighlighted(void) { return edgesHighlighted; };
	osg::Switch *getFaceEdgesHighlight(void) { return faceEdgesHighlightNode; };

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;
	virtual void setSelected(bool selected) override;

	void setShowWhenSelected(bool flag) { showWhenSelected = flag; };

	void initializeFromFacetData(std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges);
	//void assignMaterial(const std::string & materialType);
	void updateObjectColor(double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective);
	void updateObjectFacetsFromDataBase(unsigned long long entityID, unsigned long long entityVersion);

	void setEdgeHighlight(unsigned long long faceId, bool h, double thickness);
	unsigned long long getFaceIdFromTriangleIndex(unsigned long long index) { return triangleToFaceId[index]; };

	void setSurfaceColorRGB(double color[3]) { surfaceColorRGB[0] = color[0]; surfaceColorRGB[1] = color[1]; surfaceColorRGB[2] = color[2]; };
	void setEdgeColorRGB(double color[3]) { edgeColorRGB[0] = color[0]; edgeColorRGB[1] = color[1]; edgeColorRGB[2] = color[2]; };
	void setBackFaceCulling(bool b) { backFaceCulling = b; };
	void setOffsetFactor(double o) { offsetFactor = o; };
	void setStorage(const std::string &proj, unsigned long long id, unsigned long long version) { projectName = proj; entityID = id, entityVersion = version; };
	void setNeedsInitialization(void) { needsInitialization = true; };
	
	std::string getProjectName(void) { return projectName; };

	void initializeFromDataStorage(void);

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return true; };

private:
	void deleteShapeNode(void);
	osg::Node *createOSGNodeFromTriangles(double colorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, bool backFaceCulling, double offsetFactor, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles);
	void       createOSGNodeFromEdges(double colorRGB[3], std::list<Geometry::Edge> &edges, osg::Node *&edgesNode, osg::Node *&edgesHighlightedNode, osg::Switch *&faceEdgesHighlightNode);
	osg::Node *buildEdgesOSGNode(unsigned long long nEdges, osg::Vec3Array *vertices, double r, double g, double b, double transp, bool depthTest);
	void assignTexture(osg::Geometry *geometry, const std::string &textureType, bool reflective, double r, double g, double b);
	void updateWireframeState(bool visible, bool wireframe, bool transparent);
	void setVisibleState(bool visible, bool transparent, bool wireframe);
	void updateTransparentState(bool visible, bool transparent, bool wireframe);
	void applyParentTransform(void);

	osg::Node   *      triangles;
	osg::Node   *      edges;
	osg::Node   *      edgesHighlighted;
	osg::Switch *      faceEdgesHighlightNode;

	std::map<unsigned long long, osg::Node *> faceEdgesHighlight;
	std::vector<unsigned long long> triangleToFaceId;

	double surfaceColorRGB[3];
	double edgeColorRGB[3];
	bool backFaceCulling;
	double offsetFactor;
	std::string materialType;
	std::string textureType;
	bool reflective;
	bool showWhenSelected;
	std::string projectName;
	unsigned long long entityID;
	unsigned long long entityVersion;
	bool needsInitialization;
	Model *model;
	osg::StateAttribute *textureAttribute;
	osg::TexGen *textureAttributeGen;
	osg::TexEnv *textureAttributeEnv;
	double edgeTranspacency;
	osg::Matrix transformationMatrix;
};


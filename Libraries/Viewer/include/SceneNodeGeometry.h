// @otlicense
// File: SceneNodeGeometry.h
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
}

#include "Geometry.h"
#include "Viewer.h"
#include "SceneNodeBase.h"

#include "OTCore/Color.h"

#include <osg/Material>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/Matrix>
#include <osgText/Text>

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

	void setTriangles(osg::Node *t) { m_triangles = t; };
	void setEdges(osg::Node* e) { m_edges = e; };
	void setEdgesHighlighted(osg::Node *e) { m_edgesHighlighted = e; };
	void setFaceEdgesHighlighted(osg::Switch *e) { m_faceEdgesHighlightNode = e; };
	void setMaterialType(const std::string &material) { m_materialType = material; }
	void setTextureType(const std::string &texture, bool reflect) { m_textureType = texture; m_reflective = reflect; }
	void setMaterialProperties(osg::ref_ptr<osg::Material>& mat,double r , double g, double b);
	void setTransformation(const std::vector<double>& _transformation);
	void setHighlightNode(osg::Node* highlight);

	osg::Matrix getTransformation(void);
	osg::Matrix getParentTransformation(void);
	osg::Matrix getOwnTransformation(void) { return m_transformationMatrix; }
	void		setOwnTransformation(const osg::Matrix &matrix) { m_transformationMatrix = matrix; }

	void applyTransform(osg::Matrix matrix);

	osg::Node *getTriangles(void) { return m_triangles; };
	osg::Node* getEdges(void) { return m_edges; };
	osg::Node *getEdgesHighlighted(void) { return m_edgesHighlighted; };
	osg::Switch *getFaceEdgesHighlight(void) { return m_faceEdgesHighlightNode; };

	void setEdgeHighlightNode(unsigned long long faceId1, unsigned long long faceId2);

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;
	virtual ot::SelectionHandlingResult setSelected(bool selected, const ot::SelectionData& _selectionData, bool _singleSelection, const std::list<SceneNodeBase*>& _selectedNodes) override;

	void setShowWhenSelected(bool flag) { m_showWhenSelected = flag; };

	void initializeFromFacetData(std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string> &faceNameMap);
	//void assignMaterial(const std::string & materialType);
	void updateObjectColor(double surfaceColorRGB[3], const double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective);
	void updateObjectFacetsFromDataBase(ot::UID _dataEntityID, ot::UID _dataEntityVersion);

	void setEdgeHighlight(unsigned long long faceId, bool h, double thickness);
	unsigned long long getFaceIdFromTriangleIndex(unsigned long long index) { return m_triangleToFaceId[index]; };
	ot::UID getFaceIdFromEdgePrimitiveIndex(unsigned long long hitIndex);

	std::string getFaceNameFromId(unsigned long long faceId);

	std::string getEdgeNameFromFaceIds(unsigned long long faceId1, unsigned long long faceId2);

	void setSurfaceColorRGB(double color[3]) { m_surfaceColorRGB[0] = color[0]; m_surfaceColorRGB[1] = color[1]; m_surfaceColorRGB[2] = color[2]; };
	void setEdgeColorRGB(double color[3]) { m_edgeColorRGB[0] = color[0]; m_edgeColorRGB[1] = color[1]; m_edgeColorRGB[2] = color[2]; };
	void setBackFaceCulling(bool b) { m_backFaceCulling = b; };
	void setOffsetFactor(double o) { m_offsetFactor = o; };
	void setStorage(const std::string &proj, ot::UID _dataID, ot::UID _dataVersion) { m_projectName = proj; m_dataEntityID = _dataID, m_dataEntityVersion = _dataVersion; };

	void getSurfaceColorRGB(double color[3]) { color[0] = m_surfaceColorRGB[0]; color[1] = m_surfaceColorRGB[1]; color[2] = m_surfaceColorRGB[2]; };

	void setNeedsInitialization(void) { m_needsInitialization = true; };
	
	std::string getProjectName(void) { return m_projectName; };

	void initializeFromDataStorage(void);

	virtual bool isItem3D(void) const override { return true; };

	osg::Node* addSelectedEdge(unsigned long long faceId1, unsigned long long faceId2);
	void removeSelectedEdge(osg::Node* selected);

	void setCutCapGeometryTriangles(osg::ref_ptr<osg::Geometry>& geometry);
	void deleteCutCapGeometryTriangles();

	void setCutCapGeometryEdges(osg::ref_ptr<osg::Geometry> &geometry);
	void deleteCutCapGeometryEdges();

	virtual void setTransparency(double value) override;

	void setHighlightColor(const ot::Color & colorValue);
	void setHighlightLineWidth(double lineWidth);
	void updateEdgeColorMode();

private:
	void deleteShapeNode(void);
	osg::Node *createOSGNodeFromTriangles(double colorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, bool backFaceCulling, double offsetFactor, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles);
	void       createOSGNodeFromEdges(double colorRGB[3], std::list<Geometry::Edge> &edges, osg::Node *&edgesNode, osg::Node *&edgesHighlightedNode, osg::Switch *&faceEdgesHighlightNode);
	osg::Node *buildEdgesOSGNode(unsigned long long nEdges, osg::ref_ptr <osg::Vec3Array>& vertices, double r, double g, double b, double transp, bool depthTest, double width);
	void assignTexture(osg::Geometry *geometry, const std::string &textureType, bool reflective, double r, double g, double b);
	void updateWireframeState(bool visible, bool wireframe, bool transparent);
	void setVisibleState(bool visible, bool transparent, bool wireframe);
	void updateTransparentState(bool visible, bool transparent, bool wireframe);
	void applyParentTransform(void);
	osg::Node* getEdgeHighlightNode(unsigned long long faceId1, unsigned long long faceId2, double lineWidth);
	double getActualEdgeColor(const double color[], int index);
	void setEdgesColor(const double color[]);

	osg::Node* m_triangles;
	osg::Node* m_edges;
	osg::Node* m_edgesHighlighted;
	osg::Switch* m_faceEdgesHighlightNode;
	osg::Node* m_highlightNode;

	std::map<unsigned long long, osg::Node *> m_faceEdgesHighlight;
	std::vector<unsigned long long> m_triangleToFaceId;
	std::map<ot::UID, std::string> m_faceIdToNameMap;
	std::vector<unsigned long long> m_edgeStartIndex;
	std::vector<ot::UID> m_edgeFaceId;

	double m_surfaceColorRGB[3];
	double m_edgeColorRGB[3];
	bool m_backFaceCulling;
	double m_offsetFactor;
	std::string m_materialType;
	std::string m_textureType;
	bool m_reflective;
	bool m_showWhenSelected;
	std::string m_projectName;
	ot::UID m_dataEntityID;
	ot::UID m_dataEntityVersion;
	bool m_needsInitialization;
	osg::StateAttribute* m_textureAttribute;
	osg::TexGen* m_textureAttributeGen;
	osg::TexEnv* m_textureAttributeEnv;
	double m_edgeTransparency;
	osg::Matrix m_transformationMatrix;
	osg::ref_ptr<osg::Geometry> m_cutCapGeometryTriangles;
	osg::ref_ptr<osg::Geometry> m_cutCapGeometryEdges;
	bool m_enableEdgesDisplay;
};


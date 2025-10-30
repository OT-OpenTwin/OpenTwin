// @otlicense
// File: WorkingPlane.cpp
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

// Project header
#include "stdafx.h"
#include "WorkingPlane.h"
#include "ViewerSettings.h"

// Open twin header
#include "OTSystem/OTAssert.h"

// Osg header
#include <osg/Array>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osg/PolygonOffset>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Stencil>
#include <osg/MatrixTransform>
#include <osg/Depth>

WorkingPlane::WorkingPlane(osg::Node * _rootGeometryNode, osg::Group * _parentGroup)
	: m_centerX(0.f), m_centerY(0.f), m_centerZ(0.f),
	m_isVisible(false), m_rootGeometryNode(_rootGeometryNode),
	m_parentGroup(_parentGroup), m_nodeGridLines(nullptr), m_nodeGridLinesWide(nullptr), m_nodePlane(nullptr),
	m_firstVisibility(true)
{
	m_planeNode = new osg::MatrixTransform();
	m_parentGroup->addChild(m_planeNode);

	m_planeNode->setNodeMask(m_planeNode->getNodeMask() & ~3); // Reset the first and second bit of the node mask to exclude the plane from picking

	bool v = false;
	calculateSize(true, v);
	rebuildNode();
}

WorkingPlane::~WorkingPlane() {
	if (m_nodePlane) { m_planeNode->removeChild(m_nodePlane); }
	if (m_nodeGridLines) { m_planeNode->removeChild(m_nodeGridLines); }
	if (m_nodeGridLinesWide) { m_planeNode->removeChild(m_nodeGridLinesWide); }

	m_parentGroup->removeChild(m_planeNode);
}

// ################################################################################################

// Setter

void WorkingPlane::setTransformation(osg::Matrix &matrix) 
{
	m_planeNode->setMatrix(matrix);
}

void WorkingPlane::setCenterPoint(float _x, float _y, float _z) {
	m_centerX = _x;
	m_centerY = _y;
	m_centerZ = _z;

	rebuildNode();
}

bool WorkingPlane::setVisible(bool _isVisible) {
	if (_isVisible == m_isVisible) { return false; }
	m_isVisible = _isVisible;

	if (m_isVisible) {
		bool gridResolutionChanged{ false };
		calculateSize(m_firstVisibility, gridResolutionChanged);
		m_firstVisibility = false;
		rebuildNode();
		if (gridResolutionChanged) {
			return true;
		}
	}
	else {
		if (m_nodePlane) { m_planeNode->removeChild(m_nodePlane); m_nodePlane = nullptr; }
		if (m_nodeGridLines) { m_planeNode->removeChild(m_nodeGridLines); m_nodeGridLines = nullptr; }
		if (m_nodeGridLinesWide) { m_planeNode->removeChild(m_nodeGridLinesWide); m_nodeGridLinesWide = nullptr; }
	}
	return false;
}

bool WorkingPlane::refreshAfterModelChange(void) {
	if (!ViewerSettings::instance()->workingPlaneAutoSize) { return false; }
	bool gridResolutionChanged{ false };
	if (calculateSize(ViewerSettings::instance()->workingPlaneAutoGridResolution, gridResolutionChanged)) {
		rebuildNode();
	}
	return gridResolutionChanged;
}

bool WorkingPlane::refreshAfterSettingsChange(void) {
	bool gridResolutionChanged{ false };
	calculateSize(ViewerSettings::instance()->workingPlaneAutoGridResolution, gridResolutionChanged);
	rebuildNode();
	return gridResolutionChanged;
}

// ################################################################################################

// Getter



// ################################################################################################

// Private functions

bool WorkingPlane::calculateSize(bool _calculateGridStepWidth, bool& _gridResolutionChanged) {
	float w, h;
	if (!ViewerSettings::instance()->workingPlaneAutoSize) {
		w = ViewerSettings::instance()->workingPlaneSize;
		h = ViewerSettings::instance()->workingPlaneSize;
	}
	else if (!m_rootGeometryNode->getBound().valid()) {
		w = ViewerSettings::instance()->workingPlaneDefaultSize;
		h = ViewerSettings::instance()->workingPlaneDefaultSize;
	}
	else {
		w = m_rootGeometryNode->getBound().radius() * 5.f;
		h = w;
		if (w == 0.f || h == 0.f) {
			w = ViewerSettings::instance()->workingPlaneDefaultSize;
			h = ViewerSettings::instance()->workingPlaneDefaultSize;
		}
	}
	if (w == 0.f || h == 0.f) {
		OTAssert(0, "Invalid working plane size");
		return false;
	}
	if (w == m_width && h == m_height) { 
		if (_calculateGridStepWidth) { 
			calculateGridStepWidth(_gridResolutionChanged); 
			return _gridResolutionChanged;
		}
		else {
			return false;
		}
	}
	m_width = w;
	m_height = h;
	if (_calculateGridStepWidth) { calculateGridStepWidth(_gridResolutionChanged); }
	return true;
}

void WorkingPlane::calculateGridStepWidth(bool& _gridResolutionChanged) {
	float mul = 0.01f;
	while (mul < m_width) { mul *= 10; }
	int mulI = (mul * 10.f);
	while ((mulI % 10) != 0) { mulI++; }
	if (mulI == 0) { mulI = 1; }
	float v = mulI / 1000.f;
	if (v != ViewerSettings::instance()->workingPlaneGridResolution) {
		_gridResolutionChanged = true;
		ViewerSettings::instance()->workingPlaneGridResolution = v;
	}
	else {
		_gridResolutionChanged = false;
	}
}

void WorkingPlane::rebuildNode(void) {
	if (m_nodePlane) { m_planeNode->removeChild(m_nodePlane); m_nodePlane = nullptr; }
	if (m_nodeGridLines) { m_planeNode->removeChild(m_nodeGridLines); m_nodeGridLines = nullptr; }
	if (m_nodeGridLinesWide) { m_planeNode->removeChild(m_nodeGridLinesWide); m_nodeGridLinesWide = nullptr; }
	if (!m_isVisible) { return; }

	if (m_height <= 0.f || m_width <= 0.f) {
		OTAssert(0, "Invalid size @WorkingPlane");
		return;
	}

	// Load settings
	ViewerSettings * settings = ViewerSettings::instance();

	bool gridVisible = settings->workingPlaneStyle != settings->workingPlaneStyle_Plane;
	if (settings->workingPlaneGridResolution <= 0.f) { gridVisible = false; }
	bool planeVisible = settings->workingPlaneStyle != settings->workingPlaneStyle_Grid;

	if (!gridVisible && !planeVisible) { OTAssert(0, "Invalid working plane style selected"); return; }

	// Load settings
	float width, height;

	ot::Color gridCol, planeCol;
	planeCol = settings->workingPlanePlaneColor;
	gridCol = settings->workingPlaneGridLineColor;
	int widthMultiplier{ 0 };
	int heightMultiplier{ 0 };

	if (gridVisible) {
		widthMultiplier = (m_width / ViewerSettings::instance()->workingPlaneGridResolution) + 1;
		heightMultiplier = (m_height / ViewerSettings::instance()->workingPlaneGridResolution) + 1;
		if ((widthMultiplier % 2) != 0) { widthMultiplier++; }
		if ((heightMultiplier % 2) != 0) { heightMultiplier++; }
		width = ViewerSettings::instance()->workingPlaneGridResolution * widthMultiplier;
		height = ViewerSettings::instance()->workingPlaneGridResolution * heightMultiplier;
	}
	else {
		width = m_width;
		height = m_height;
	}

	if (planeVisible) {
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(4);
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(4);
		for (int i{ 0 }; i < 4; i++) {
			colors->at(i).set(planeCol.r() / 255.0, planeCol.g() / 255.0, planeCol.b() / 255.0, planeCol.a() / 255.0);
			normals->at(i).set(0.f, 0.f, 1.f);
		}
		vertices->at(0).set(m_centerX - (width / 2), m_centerY - (height / 2), m_centerZ);
		vertices->at(1).set(m_centerX - (width / 2), m_centerY + (height / 2), m_centerZ);
		vertices->at(2).set(m_centerX + (width / 2), m_centerY + (height / 2), m_centerZ);
		vertices->at(3).set(m_centerX + (width / 2), m_centerY - (height / 2), m_centerZ);

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
		newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

		// Now set the vertices, the normal and the colors
		newGeometry->setVertexArray(vertices);

		newGeometry->setNormalArray(normals);
		newGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

		newGeometry->setColorArray(colors);
		newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

		// Now create the geometry node and assign the drawable
		m_nodePlane = new osg::Geode;
		m_nodePlane->addDrawable(newGeometry);

		osg::StateSet *ssPlane = m_nodePlane->getOrCreateStateSet();
		ssPlane->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//ssNode->setMode(GL_BLEND, osg::StateAttribute::OFF);
		//ssNode->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
		ssPlane->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
		//ssNode->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);
		//ssNode->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		ssPlane->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		osg::ref_ptr<osg::BlendFunc> blendFuncPlane = new osg::BlendFunc;
		blendFuncPlane->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		ssPlane->setAttributeAndModes(blendFuncPlane);
		ssPlane->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ssPlane->setAttributeAndModes(depth, osg::StateAttribute::ON);
		ssPlane->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		ssPlane->setRenderBinDetails(100, "RenderBin"); // draw after everything else
	}

	// Grid
	if (gridVisible) {
		std::list<osg::Vec3> gridEdgesList;
		std::list<osg::Vec3> gridEdgesWideList;

		int stepCt{ 0 };
		float gridResolution = ViewerSettings::instance()->workingPlaneGridResolution;
		int hightlightStep = ViewerSettings::instance()->workingPlaneHighlightEveryStep;

		// Vertical
		for (int ctX{ 0 }; ctX < (widthMultiplier / 2); ctX++) {
			float x = ctX * gridResolution;
			if (stepCt >= hightlightStep && hightlightStep > 0) {
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX - x, m_centerY - height / 2.f, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX - x, m_centerY + height / 2.f, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX + x, m_centerY + height / 2.f, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX + x, m_centerY - height / 2.f, m_centerZ });
				stepCt = 0;
			}
			else {
				gridEdgesList.push_back(osg::Vec3{ m_centerX - x, m_centerY - height / 2.f, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX - x, m_centerY + height / 2.f, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX + x, m_centerY + height / 2.f, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX + x, m_centerY - height / 2.f, m_centerZ });
				stepCt++;
			}
		}

		// Horizontal
		stepCt = 0;
		for (int ctY{ 0 }; ctY < (heightMultiplier / 2); ctY++) {
			float y = ctY * gridResolution;
			if (stepCt >= hightlightStep && hightlightStep > 0) {
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY - y, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY + y, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY + y, m_centerZ });
				gridEdgesWideList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY - y, m_centerZ });
				stepCt = 0;
			}
			else {
				gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY - y, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY + y, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY + y, m_centerZ });
				gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY - y, m_centerZ });
				stepCt++;
			}
		}

		if (gridEdgesList.size() > 0) {
			gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY - height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY - height / 2.f, m_centerZ });

			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY - height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY - height / 2.f, m_centerZ });

			gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY + height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY + height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY, m_centerZ });

			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX, m_centerY + height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY + height / 2.f, m_centerZ });
			gridEdgesList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY, m_centerZ });
		}

		if (gridEdgesWideList.size() > 0) {
			gridEdgesWideList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY - height / 2.f, m_centerZ });
			gridEdgesWideList.push_back(osg::Vec3{ m_centerX - width / 2.f, m_centerY + height / 2.f, m_centerZ });
			gridEdgesWideList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY + height / 2.f, m_centerZ });
			gridEdgesWideList.push_back(osg::Vec3{ m_centerX + width / 2.f, m_centerY - height / 2.f, m_centerZ });
		}

		// Create array
		osg::ref_ptr<osg::Vec3Array> gridVertices = new osg::Vec3Array(gridEdgesList.size());
		osg::ref_ptr<osg::Vec3Array> gridVerticesWide = new osg::Vec3Array(gridEdgesWideList.size());
		size_t ctGrid{ 0 };
		size_t ctGridWide{ 0 };
		for (auto pt : gridEdgesList) {
			gridVertices->at(ctGrid++).set(pt);
		}
		for (auto pt : gridEdgesWideList) {
			gridVerticesWide->at(ctGridWide++).set(pt);
		}

		osg::ref_ptr<osg::Vec4Array> gridColors = new osg::Vec4Array;
		osg::ref_ptr<osg::Vec4Array> gridColorsWide = new osg::Vec4Array;
		gridColors->push_back(osg::Vec4(gridCol.r() / 255.0, gridCol.g() / 255.0, gridCol.b() / 255.0, gridCol.a() / 255.0));
		gridColorsWide->push_back(osg::Vec4(gridCol.r() / 255.0, gridCol.g() / 255.0, gridCol.b() / 255.0, gridCol.a() / 255.0));

		// Create the geometry object to store the data
		osg::ref_ptr<osg::Geometry> gridGeometry = new osg::Geometry;
		
		if (!ViewerSettings::instance()->useDisplayLists)
		{
			gridGeometry->setUseDisplayList(false);
			gridGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
		}

		gridGeometry->setVertexArray(gridVertices);

		gridGeometry->setColorArray(gridColors);
		gridGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		gridGeometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, ctGrid));

		// Now create the geometry node and assign the drawable
		m_nodeGridLines = new osg::Geode;
		m_nodeGridLines->addDrawable(gridGeometry);

		// Set the display attributes for the edges geometry
		osg::StateSet *ssGrid = m_nodeGridLines->getOrCreateStateSet();

		//osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		//blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//ss->setAttributeAndModes(blendFunc);

		ssGrid->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		ssGrid->setMode(GL_BLEND, osg::StateAttribute::OFF);
		ssGrid->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
		ssGrid->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
		ssGrid->setAttribute(new osg::LineWidth(settings->workingPlaneGridLineWidth), osg::StateAttribute::ON);
		ssGrid->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		ssGrid->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));

		//osg::ref_ptr<osg::BlendFunc> blendFuncGrid = new osg::BlendFunc;
		//blendFuncGrid->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//ssGrid->setAttributeAndModes(blendFuncGrid);
		//ssGrid->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		if (ctGridWide > 0) {
			osg::ref_ptr<osg::Geometry> gridGeometryWide = new osg::Geometry;

			if (!ViewerSettings::instance()->useDisplayLists)
			{
				gridGeometryWide->setUseDisplayList(false);
				gridGeometryWide->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
			}

			gridGeometryWide->setVertexArray(gridVerticesWide);

			gridGeometryWide->setColorArray(gridColorsWide);
			gridGeometryWide->setColorBinding(osg::Geometry::BIND_OVERALL);

			gridGeometryWide->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, ctGridWide));

			m_nodeGridLinesWide = new osg::Geode;
			m_nodeGridLinesWide->addDrawable(gridGeometryWide);

			osg::StateSet *ssGridWide = m_nodeGridLinesWide->getOrCreateStateSet();
			ssGridWide->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			ssGridWide->setMode(GL_BLEND, osg::StateAttribute::OFF);
			ssGridWide->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
			ssGridWide->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
			ssGridWide->setAttribute(new osg::LineWidth(settings->workingPlaneWideGridLineWidth), osg::StateAttribute::ON);
			ssGridWide->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
			ssGridWide->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));

			//osg::ref_ptr<osg::BlendFunc> blendFuncGridWide = new osg::BlendFunc;
			//blendFuncGridWide->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//ssGridWide->setAttributeAndModes(blendFuncGridWide);
			//ssGridWide->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			m_planeNode->addChild(m_nodeGridLinesWide);
		}

		m_planeNode->addChild(m_nodeGridLines);
		m_planeNode->getOrCreateStateSet()->setMode(GL_STENCIL_TEST, osg::StateAttribute::OFF);
	}

	if (m_nodePlane) {
		m_planeNode->addChild(m_nodePlane);
	}
}

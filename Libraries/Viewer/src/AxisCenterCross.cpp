// @otlicense
// File: AxisCenterCross.cpp
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
#include "AxisCenterCross.h"
#include "ViewerSettings.h"

#include "OTSystem/OTAssert.h"

#include <osg/Group>
#include <osg/Shape>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/Depth>

AxisCenterCross::AxisCenterCross(osg::Node * _geometryNode, osg::Group * _parentGroup)
	: m_parentGroup(_parentGroup), m_xNode(nullptr), m_yNode(nullptr), m_zNode(nullptr),
	m_isVisible(false), m_lineLength(30.f), m_geometryNode(_geometryNode)
{
	m_axisCrossNode = new osg::MatrixTransform();
	m_parentGroup->addChild(m_axisCrossNode);
}

AxisCenterCross::~AxisCenterCross() {
	if (m_xNode) { m_axisCrossNode->removeChild(m_xNode); m_xNode = nullptr; }
	if (m_yNode) { m_axisCrossNode->removeChild(m_yNode); m_yNode = nullptr; }
	if (m_zNode) { m_axisCrossNode->removeChild(m_zNode); m_zNode = nullptr; }

	m_parentGroup->removeChild(m_axisCrossNode);
}

// #########################################################################################

void AxisCenterCross::setTransformation(osg::Matrix &matrix) 
{
	m_axisCrossNode->setMatrix(matrix);
}

void AxisCenterCross::setVisible(bool _isVisible) {
	if (_isVisible == m_isVisible) { return; }
	m_isVisible = _isVisible;
	if (m_isVisible) {
		rebuildNode();
	}
	else {
		if (m_xNode) { m_axisCrossNode->removeChild(m_xNode); m_xNode = nullptr; }
		if (m_yNode) { m_axisCrossNode->removeChild(m_yNode); m_yNode = nullptr; }
		if (m_zNode) { m_axisCrossNode->removeChild(m_zNode); m_zNode = nullptr; }
	}
}

void AxisCenterCross::refreshAfterGeometryChange(void) {
	float l{ 0.f };
	const osg::BoundingSphere& bound = m_geometryNode->getBound();
	if (bound.valid()) {
		l = bound.radius() * 3.f;
	}
	if (l <= 0.f) { l = 30.f; }

	if (l != m_lineLength) {	
		m_lineLength = l;
		rebuildNode();
	}
}

void AxisCenterCross::refreshAfterSettingsChange(void) {
	rebuildNode();
}

// #########################################################################################

void AxisCenterCross::rebuildNode(void) {
	// Clean up
	if (m_xNode) { m_axisCrossNode->removeChild(m_xNode); m_xNode = nullptr; }
	if (m_yNode) { m_axisCrossNode->removeChild(m_yNode); m_yNode = nullptr; }
	if (m_zNode) { m_axisCrossNode->removeChild(m_zNode); m_zNode = nullptr; }

	if (!m_isVisible) { return; }
	
	ViewerSettings * settings = ViewerSettings::instance();
	// Arrow X

	m_xNode = new osg::Geode;
	m_yNode = new osg::Geode;
	m_zNode = new osg::Geode;

	createArrow(m_xNode, settings->axisXColor, settings->axisCenterCrossLineWidth, oX);
	createArrow(m_yNode, settings->axisYColor, settings->axisCenterCrossLineWidth, oY);
	createArrow(m_zNode, settings->axisZColor, settings->axisCenterCrossLineWidth, oZ);

	if (settings->axisCenterCrossDashedLineVisible) {
		createDashedLine(m_xNode, settings->axisXColor, settings->axisCenterCrossLineWidth, oX);
		createDashedLine(m_yNode, settings->axisYColor, settings->axisCenterCrossLineWidth, oY);
		createDashedLine(m_zNode, settings->axisZColor, settings->axisCenterCrossLineWidth, oZ);
	}

	finalizeGeode(m_xNode);
	finalizeGeode(m_yNode);
	finalizeGeode(m_zNode);

	m_axisCrossNode->addChild(m_xNode);
	m_axisCrossNode->addChild(m_yNode);
	m_axisCrossNode->addChild(m_zNode);
}

void AxisCenterCross::createArrow(osg::Geode * _geode, const ot::Color& _color, float _lineWidth, eOrientation _o) {
	ot::Point3D<float> pos(0.f, 0.f, 0.f);
	switch (_o)
	{
	case AxisCenterCross::oX: pos.setX(m_lineLength); break;
	case AxisCenterCross::oY: pos.setY(m_lineLength); break;
	case AxisCenterCross::oZ: pos.setZ(m_lineLength); break;
	default:
		OTAssert(0, "Unknown orientation");
		break;
	}
	std::list<osg::Vec3> pointList;
	pointList.push_back(osg::Vec3(0.f, 0.f, 0.f));
	pointList.push_back(osg::Vec3(pos.x(), pos.y(), pos.z()));

	// Create vertices
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(pointList.size());
	size_t ctVertices{ 0 };
	for (auto pt : pointList) { vertices->at(ctVertices++).set(pt); }

	// Create colors
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(_color.r() / 255.0, _color.g() / 255.0, _color.b() / 255.0, _color.a() / 255.0));

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

	// Enable polygon offset
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	// Set vertices
	newGeometry->setVertexArray(vertices);

	// Set colors
	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Draw vertices
	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointList.size()));

	// Now create the geometry node and assign the drawable
	_geode->addDrawable(newGeometry);
}

void AxisCenterCross::createDashedLine(osg::Geode * _geode, const ot::Color& _color, float _lineWidth, eOrientation _o) {
	
	float stepLen = m_lineLength / 50.f;
	if (stepLen <= 0.f) { OTAssert(0, "No steps"); return; }
	std::list<osg::Vec3> pointList;

	for (float pt{ stepLen * 2.f }; pt < m_lineLength; pt += (2.f * stepLen)) {
		ot::Point3D<float> posF(0.f, 0.f, 0.f);
		ot::Point3D<float> posT(0.f, 0.f, 0.f);
		switch (_o)
		{
		case AxisCenterCross::oX: posF.setX((pt - stepLen) * (-1.f)); posT.setX(pt * (-1.f)); break;
		case AxisCenterCross::oY: posF.setY((pt - stepLen) * (-1.f)); posT.setY(pt * (-1.f)); break;
		case AxisCenterCross::oZ: posF.setZ((pt - stepLen) * (-1.f)); posT.setZ(pt * (-1.f)); break;
		default:
			OTAssert(0, "Unknown orientation");
			break;
		}
		pointList.push_back(osg::Vec3(posF.x(), posF.y(), posF.z()));
		pointList.push_back(osg::Vec3(posT.x(), posT.y(), posT.z()));
	}

	// Create vertices
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(pointList.size());
	size_t ctVertices{ 0 };
	for (auto pt : pointList) { vertices->at(ctVertices++).set(pt); }

	// Create colors
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(_color.r() / 255.0, _color.g() / 255.0, _color.b() / 255.0, _color.a() / 255.0));

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

	// Enable polygon offset
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	newGeometry->setVertexArray(vertices);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointList.size()));

	// Now create the geometry node and assign the drawable
	_geode->addDrawable(newGeometry);
}

void AxisCenterCross::finalizeGeode(osg::Geode * _geode) {
	ViewerSettings * settings = ViewerSettings::instance();

	osg::StateSet *ss = _geode->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(settings->axisCenterCrossLineWidth), osg::StateAttribute::ON);
	ss->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));
 
	if (settings->axisCenterCrossLineAtFront) {
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	}
	else {
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		osg::ref_ptr<osg::BlendFunc> blendFuncGrid = new osg::BlendFunc;
		blendFuncGrid->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		ss->setAttributeAndModes(blendFuncGrid);
		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ss->setAttributeAndModes(depth, osg::StateAttribute::ON);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	}
}

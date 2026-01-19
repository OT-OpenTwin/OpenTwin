// @otlicense
// File: AxisCross.h
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

#include "OTCore/Color.h"

#include <osg/Geode>
#include <osgText/Text>

namespace osg { 
	class Group;
	class ShapeDrawable;
	class MatrixTransform;
}

class AxisCross {
public:
	AxisCross(osg::Group * _parentGroup, osgText::Font *_font);
	virtual ~AxisCross();

	void setVisible(bool _visible);

	bool isVisible(void) const { return m_isVisible; }

	void refreshAfterSettingsChange(void);

	void setTransformation(osg::Matrix matrix);

	void setTextColor(double color[3]);
private:

	void rebuildNode(void);

	void createArrow(osg::Geode * _geode, const ot::Color& _color);
	void createText(osg::Geode * _geode, const std::string &label);

	osg::Group *						m_parentGroup;
	osg::ref_ptr<osg::Geode>			m_xNode;
	osg::ref_ptr<osg::Geode>			m_yNode;
	osg::ref_ptr<osg::Geode>			m_zNode;
	osg::ref_ptr<osg::Geode>			m_centerPoint;
	osg::ref_ptr<osg::MatrixTransform>	m_xTransform;
	osg::ref_ptr<osg::MatrixTransform>	m_yTransform;
	osg::ref_ptr<osg::MatrixTransform>	m_zTransform;

	osg::ref_ptr<osg::MatrixTransform>	m_axisCrossNode;

	double textColor[3];

	osgText::Font *font;

	float						m_midpointX;
	float						m_midpointY;
	float						m_midpointZ;
	float						m_size;
	float						m_cylinderRadius;

	bool						m_isVisible;
	bool						m_isIdentity;
};
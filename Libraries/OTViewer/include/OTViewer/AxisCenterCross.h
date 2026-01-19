// @otlicense
// File: AxisCenterCross.h
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

#include "OTCore/Point3D.h"
#include "OTCore/Color.h"
#include <osg/Geode>

namespace osg {
	class Group;
	class MatrixTransform;
	class Node;
}

class AxisCenterCross {
public:
	AxisCenterCross(osg::Node * _geometryNode, osg::Group * _parentGroup);

	virtual ~AxisCenterCross();

	// #########################################################################################

	void setVisible(bool _isVisible);

	bool isVisible(void) const { return m_isVisible; }

	void refreshAfterGeometryChange(void);

	void refreshAfterSettingsChange(void);

	void setTransformation(osg::Matrix &matrix);

	// #########################################################################################


private:
	enum eOrientation {
		oX,
		oY,
		oZ
	};

	void rebuildNode(void);

	void createArrow(osg::Geode * _geode, const ot::Color& _color, float _lineWidth, eOrientation _o);

	void createDashedLine(osg::Geode * _geode, const ot::Color& _color, float _lineWidth, eOrientation _o);

	void finalizeGeode(osg::Geode * _geode);

	bool								m_isVisible;
	
	osg::Group *						m_parentGroup;
	osg::Node *							m_geometryNode;

	osg::ref_ptr<osg::Geode>			m_xNode;
	osg::ref_ptr<osg::Geode>			m_yNode;
	osg::ref_ptr<osg::Geode>			m_zNode;
	osg::ref_ptr<osg::Geode>			m_centerPoint;

	osg::ref_ptr<osg::MatrixTransform>	m_axisCrossNode;


	float								m_lineLength;

	AxisCenterCross() = delete;
	AxisCenterCross(AxisCenterCross&) = delete;
	AxisCenterCross& operator = (AxisCenterCross&) = delete;
};
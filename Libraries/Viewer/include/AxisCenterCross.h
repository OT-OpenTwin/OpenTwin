#pragma once

#include "OpenTwinCore/Point3D.h"
#include "OpenTwinCore/Color.h"
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
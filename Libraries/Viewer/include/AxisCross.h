// @otlicense

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
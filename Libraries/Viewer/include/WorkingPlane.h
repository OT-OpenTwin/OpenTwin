// @otlicense

#pragma once

#include "OTCore/Color.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Matrix>

namespace osg {
	class Node;
	class Group;
}

class WorkingPlane {
public:
	WorkingPlane(osg::Node * _rootGeometryNode, osg::Group * _parentGroup);
	virtual ~WorkingPlane();

	// ################################################################################################

	// Setter

	void setCenterPoint(float _x, float _y, float _z);

	bool setVisible(bool _isVisible);

	bool refreshAfterModelChange(void);

	bool refreshAfterSettingsChange(void);

	void setTransformation(osg::Matrix &matrix);

	// ################################################################################################

	// Getter

	float centerX(void) const { return m_centerX; }
	float centerY(void) const { return m_centerY; }
	float centerZ(void) const { return m_centerZ; }
	
	float height(void) const { return m_height; }
	float width(void) const { return m_width; }

	bool isVisible(void) const { return m_isVisible; }

private:

	bool calculateSize(bool _calculateGridStepWidth, bool& _gridResolutionChanged);
	void calculateGridStepWidth(bool& _gridResolutionChanged);

	void rebuildNode(void);

	osg::Node *					m_rootGeometryNode;
	osg::Group *				m_parentGroup;
	osg::ref_ptr<osg::Geode>	m_nodePlane;
	osg::ref_ptr<osg::Geode>	m_nodeGridLines;
	osg::ref_ptr<osg::Geode>	m_nodeGridLinesWide;

	osg::ref_ptr<osg::MatrixTransform>	m_planeNode;

	bool		m_firstVisibility;

	float		m_centerX;
	float		m_centerY;
	float		m_centerZ;

	float		m_height;
	float		m_width;

	bool		m_isVisible;

	WorkingPlane() = delete;
	WorkingPlane(WorkingPlane&) = delete;
	WorkingPlane& operator = (WorkingPlane&) = delete;
};
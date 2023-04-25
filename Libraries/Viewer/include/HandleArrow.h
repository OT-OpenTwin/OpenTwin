#pragma once

#include "HandlerBase.h"

#include <osg/Geode>

class HandleArrow : public HandlerBase
{
public:
	HandleArrow() = delete;
	HandleArrow(osg::Vec3d point, osg::Vec3d dir, double color[4], double length, double radius);
	virtual ~HandleArrow();

	virtual osg::Node *getNode(void) override;

	virtual void mouseOver(bool flag) override;
	virtual void setInteraction(int intersectionIndex, double intersectionRatio) override;
	virtual void setInteractionReference(int intersectionIndex, double intersectionRatio) override;

	void setPosition(osg::Vec3d pos);
	osg::Vec3d getPosition(void) { return position; }

private:
	void createArrow(osg::Geode * _geode, double color[4], double length, double radius);
	void updateTransform(void);

	osg::ref_ptr<osg::MatrixTransform>  osgNode;
	osg::ref_ptr<osg::Switch>			osgSwitchNode;
	osg::ref_ptr<osg::Geode>			arrowNode;
	osg::ref_ptr<osg::Geode>			arrowNodeSelected;

	osg::Vec3d position;
	osg::Vec3d direction;

	double intersectionLineLength;
	double referenceOffset;
};


// @otlicense

#pragma once

#include <osg/Geode>
#include <osg/Switch>

namespace osg
{
	class Node;
	class Switch;
	class Drawable;
}

#include <osg/Vec3d>

#include <list>

class HandlerBase;
class Viewer;

class HandlerNotifier
{
public:
	HandlerNotifier() {};
	virtual ~HandlerNotifier() {};

	virtual void handlerInteraction(HandlerBase *handler) = 0;
};

class HandlerBase
{
public:
	HandlerBase() : selectorDrawable(nullptr) {};
	virtual ~HandlerBase() {};

	virtual osg::Node *getNode(void) = 0;

	virtual void mouseOver(bool flag) = 0;

	void createSelectLine(osg::Switch *node, std::list<osg::Vec3d> &pointList, bool visible = false);

	osg::Drawable *getSelectorDrawable(void) { return selectorDrawable; }

	virtual void setInteraction(Viewer *viewer, int intersectionIndex, double intersectionRatio) = 0;
	virtual void setInteractionReference(int intersectionIndex, double intersectionRatio) = 0;

	void beginInteraction(void);
	void endInteraction(void);

	void addNotifier(HandlerNotifier *notifier);
	void removeNotifier(HandlerNotifier *notifier);

	void reportInteraction(void);

private:
	osg::Drawable *selectorDrawable;
	osg::ref_ptr<osg::Geode> selectLine;
	osg::ref_ptr<osg::Switch> switchNode;

	std::list<HandlerNotifier *> notifiers;
};


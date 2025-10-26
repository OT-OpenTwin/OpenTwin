#include "stdafx.h"

#include "HandlerBase.h"
#include "ViewerSettings.h"

#include <cassert>

#include <osg/Geode>
#include <osg/PolygonMode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Switch>

void HandlerBase::createSelectLine(osg::Switch *node, std::list<osg::Vec3d> &pointList, bool visible)
{
	assert(!pointList.empty());

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(pointList.size());

	// Now store the edge vertices in the array
	size_t nVertex = 0;
	for (auto point : pointList)
	{
		vertices->at(nVertex).set(point.x(), point.y(), point.z());
		nVertex++;
	}

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0, 1.0, 1.0, 0.5));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->setVertexArray(vertices);

	newGeometry->setColorArray(colors);
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, pointList.size()));

	// Now create the geometry node and assign the drawable
	selectLine = new osg::Geode;
	selectLine->addDrawable(newGeometry);
	selectorDrawable = newGeometry;

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = selectLine->getOrCreateStateSet();

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	node->addChild(selectLine);
	if (!visible)
	{
		node->setChildValue(selectLine, false);
	}

	switchNode = node;
}

void HandlerBase::beginInteraction(void)
{
	if (switchNode == nullptr) return;
	
	assert(selectLine != nullptr);
	switchNode->setChildValue(selectLine, true);
}

void HandlerBase::endInteraction(void)
{
	if (switchNode == nullptr) return;

	assert(selectLine != nullptr);
	switchNode->setChildValue(selectLine, false);
}

void HandlerBase::addNotifier(HandlerNotifier *notifier)
{
	notifiers.push_back(notifier);
}

void HandlerBase::removeNotifier(HandlerNotifier *notifier)
{
	notifiers.remove(notifier);
}

void HandlerBase::reportInteraction(void)
{
	for (auto notifier : notifiers)
	{
		notifier->handlerInteraction(this);
	}
}

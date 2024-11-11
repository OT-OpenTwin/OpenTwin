#include "stdafx.h"

#include "SceneNodePlot1DCurve.h"

#include "DataBase.h"
#include "Model.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>

SceneNodePlot1DCurve::SceneNodePlot1DCurve() :
	modelEntityVersion(0),
	model(nullptr)
{

}

SceneNodePlot1DCurve::~SceneNodePlot1DCurve()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	// loop through all parent nodes
	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group *parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		shapeNode = nullptr;
	}
}

void SceneNodePlot1DCurve::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
}

void SceneNodePlot1DCurve::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
}

void SceneNodePlot1DCurve::setVisible(bool v)
{
	if (v == isVisible()) return;

	SceneNodeBase::setVisible(v);
}

void SceneNodePlot1DCurve::setHighlighted(bool h)
{
}

#include "stdafx.h"

#include "SceneNodeResult1DItem.h"

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

SceneNodeResult1DItem::SceneNodeResult1DItem() :
	modelEntityVersion(0),
	model(nullptr)
{

}

SceneNodeResult1DItem::~SceneNodeResult1DItem()
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

void SceneNodeResult1DItem::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
}

void SceneNodeResult1DItem::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
}

void SceneNodeResult1DItem::setVisible(bool v)
{
	if (v == isVisible()) return;

	SceneNodeBase::setVisible(v);
}

void SceneNodeResult1DItem::setHighlighted(bool h)
{
}

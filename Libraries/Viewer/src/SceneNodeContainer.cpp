#include "stdafx.h"

#include "SceneNodeContainer.h"

#include <osg/Node>
#include <osg/Switch>

SceneNodeContainer::SceneNodeContainer()
{
	// Create a group osg node
	m_shapeNode = new osg::Switch;

	m_shapeNode->setAllChildrenOn();
}

SceneNodeContainer::~SceneNodeContainer()
{
	// We first need to recursively delete all children, since the osg nodes will also recursively delete their children as well.
	std::list<SceneNodeBase*> currentChilds = getChildren();
	for (auto child : currentChilds)
	{
		delete child;
	}

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
		m_shapeNode = nullptr;
	}
}

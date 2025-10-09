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
}

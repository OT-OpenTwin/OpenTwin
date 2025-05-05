#include "stdafx.h"

#include "SceneNodeCartesianMeshItem.h"
#include "SceneNodeCartesianMesh.h"
#include "Geometry.h"

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

SceneNodeCartesianMeshItem::SceneNodeCartesianMeshItem() :
	model(nullptr),
	mesh(nullptr),
	colorRGB{0.0, 0.0, 0.0},
	isVolume(false)
{

}

SceneNodeCartesianMeshItem::~SceneNodeCartesianMeshItem()
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
		m_shapeNode = nullptr;
	}

	getMesh()->removeOwner(this, faceID);
}

void SceneNodeCartesianMeshItem::updateVisibility(void)
{
	getMesh()->updateFaceStatus(faceID);
}

void SceneNodeCartesianMeshItem::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setVisible(bool v)
{
	if (v == isVisible()) return;

	if (v)
	{
		getMesh()->ensureDataLoaded();
	}

	SceneNodeBase::setVisible(v);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setHighlighted(bool h)
{
	return;
	if (h == isHighlighted()) return;

	SceneNodeBase::setHighlighted(h);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setFacesList(std::vector<int> &faces)
{ 
	faceID = faces; 

	for (auto face : faceID)
	{
		getMesh()->addOwner(this, face);
	}

	updateVisibility();
}

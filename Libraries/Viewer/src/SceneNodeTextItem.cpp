#include "stdafx.h"

#include "SceneNodeTextItem.h"

#include "DataBase.h"
#include "Model.h"
#include "Notifier.h"

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

SceneNodeTextItem::SceneNodeTextItem() :
	model(nullptr),
	textLoaded(false)
{

}

SceneNodeTextItem::~SceneNodeTextItem()
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

void SceneNodeTextItem::setSelected(bool _selection)
{
	if (getModel() != nullptr)
	{
		if (!isSelected() && _selection && getModel()->isSingleItemSelected())
		{
			const std::list<IVisualiser*> visualisers = getVisualiser();
			for (IVisualiser* visualiser : visualisers)
			{
				if (visualiser->isVisible())
				{
					visualiser->visualise();
				}
			}
		}
	}

	SceneNodeBase::setSelected(_selection);
}

void SceneNodeTextItem::addVisualiserText()
{
	auto textVis = new TextVisualiser(getModelEntityID());
	addVisualiser(textVis);
}

#include "stdafx.h"
#include "SceneNodeMultiVisualisation.h"
#include <osg/Node>
#include <osg/Switch>
#
SceneNodeMultiVisualisation::~SceneNodeMultiVisualisation()
{
	//Always necessary?

	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group* parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		shapeNode = nullptr;
	}
}

void SceneNodeMultiVisualisation::setSelected(bool _selection)
{
	if (getModel() != nullptr)
	{
		if (!isSelected() && _selection && getModel()->isSingleItemSelected())
		{
			const std::list<Visualiser*> visualisers = getVisualiser();
			for (Visualiser* visualiser : visualisers)
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

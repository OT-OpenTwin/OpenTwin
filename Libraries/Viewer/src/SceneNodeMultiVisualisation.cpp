#include "stdafx.h"
#include "SceneNodeMultiVisualisation.h"
#include <osg/Node>
#include <osg/Switch>
#include "TableVisualiser.h"

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

void SceneNodeMultiVisualisation::setViewChange(const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType)
{
	if (_state == ot::ViewChangedStates::changesSaved)
	{
		const std::list< Visualiser*>& allVisualiser = getVisualiser();
		for (Visualiser* visualiser : allVisualiser)
		{
			//Update the other visualiser, if it is open 
			if (_viewType == ot::WidgetViewBase::ViewType::ViewTable)
			{
				TextVisualiser* textVisualiser = dynamic_cast<TextVisualiser*>(visualiser);
				if (textVisualiser != nullptr && textVisualiser->viewIsCurrentlyOpen())
				{
					textVisualiser->visualise();
				}
			}
			else if (_viewType == ot::WidgetViewBase::ViewType::ViewText)
			{
				TableVisualiser* tableVisualiser = dynamic_cast<TableVisualiser*>(visualiser);
				if (tableVisualiser != nullptr && tableVisualiser->viewIsCurrentlyOpen())
				{
					tableVisualiser->visualise();
				}
			}
			else
			{
				assert(0); // not supported view Type for scene node.
			}
		}
	}
	else
	{
		const std::list< Visualiser*>& allVisualiser = getVisualiser();
		bool viewIsOpen = _state == ot::ViewChangedStates::viewOpened ? true : false;
		bool thisIsTheView = false;
		for (Visualiser* visualiser : allVisualiser)
		{
			if (_viewType == ot::WidgetViewBase::ViewType::ViewText)
			{
				TextVisualiser* textVisualiser = dynamic_cast<TextVisualiser*>(visualiser);
				thisIsTheView = textVisualiser != nullptr;
			}
			else if (_viewType == ot::WidgetViewBase::ViewType::ViewTable)
			{
				TableVisualiser* tableVisualiser = dynamic_cast<TableVisualiser*>(visualiser);
				thisIsTheView = tableVisualiser != nullptr;
			}

			if (thisIsTheView)
			{
				visualiser->setViewIsOpen(viewIsOpen);
				break;
			}
		}
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
				if (visualiser->isVisible() && !visualiser->viewIsCurrentlyOpen())
				{
					visualiser->visualise();
				}
			}
		}
	}

	SceneNodeBase::setSelected(_selection);
}

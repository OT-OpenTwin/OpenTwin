#include "stdafx.h"
#include "SceneNodeMultiVisualisation.h"
#include <osg/Node>
#include <osg/Switch>
#include "TableVisualiser.h"
#include "FrontendAPI.h"

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
		// Update every other visualiser if open 
		for (Visualiser* visualiser : allVisualiser)
		{
			if (visualiser->getViewType() != _viewType && visualiser->viewIsCurrentlyOpen()) {
				visualiser->visualise(false);
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

void SceneNodeMultiVisualisation::setSelected(bool _selection, bool _selectionFromNavigationTree)
{
	if (getModel() != nullptr)
	{
		if (!isSelected() && _selection && getModel()->isSingleItemSelected())
		{
			const std::list<Visualiser*> visualisers = getVisualiser();
			for (Visualiser* visualiser : visualisers)
			{
				if (visualiser->isVisible() && !visualiser->viewIsCurrentlyOpen() && _selectionFromNavigationTree)
				{
					visualiser->visualise();
				}
				else if (visualiser->viewIsCurrentlyOpen() && _selectionFromNavigationTree)
				{
					FrontendAPI::instance()->setCurrentVisualizationTabFromEntityName(getName(), visualiser->getViewType());
				}
			}
		}
	}

	SceneNodeBase::setSelected(_selection, _selectionFromNavigationTree);
}

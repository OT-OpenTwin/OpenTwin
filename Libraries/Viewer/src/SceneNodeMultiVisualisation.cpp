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

	// Here we switch the view state changes
	if (_state == ot::ViewChangedStates::changesSaved)
	{
		const std::list< Visualiser*>& allVisualiser = getVisualiser();
		// We initiated a model state change from the ui. Now we request a new visualisation for every visualiser which is not the one that initiated the 
		// Model state change under the condition that the view is open.
		for (Visualiser* visualiser : allVisualiser)
		{
			if (visualiser->getViewType() != _viewType && visualiser->viewIsCurrentlyOpen()) {
				visualiser->visualise(false);
			}
		}
	}
	else
	{
		//Here we set/unset the viewer state isOpen for the opened visualisation.
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

ot::SelectionHandlingResult SceneNodeMultiVisualisation::setSelected(bool _selection, ot::SelectionOrigin _selectionOrigin) {
	ot::SelectionHandlingResult result;

	if (getModel() != nullptr)
	{
		// First we check if there is a change from not selected to selected. Multi selections don't trigger a visualisation since it could 
		// require too much computational power
		if (!isSelected() && _selection && getModel()->isSingleItemSelected())
		{
			const std::list<Visualiser*> visualisers = getVisualiser();
			for (Visualiser* visualiser : visualisers)
			{
				// We have a valid state switch, so we visualise all views, if they are not already opened in a view and the selection origins from a user interaction
				// In case that properties change, effectively a new entity is created (same ID, different version) and a new scene node is created. 
				// Therefore it is not necessary to compare the states of scenenode and entity. This algorithm only deals with the state of the view being
				// open or not.
				// Other selection origins are neglected. View origin is triggered if a view is selected in UI, which triggers the selection of the corresponding entity.
				// Custom selection changes come from somewhere within the code.
				if (visualiser->isVisible() && !visualiser->viewIsCurrentlyOpen() && _selectionOrigin == ot::SelectionOrigin::User)
				{
					visualiser->visualise();
					result |= ot::SelectionHandlingEvent::NewViewRequested;
				}
				else if (visualiser->viewIsCurrentlyOpen() && _selectionOrigin == ot::SelectionOrigin::User)
				{
					// Here we just want to focus an already opened view.
					FrontendAPI::instance()->setCurrentVisualizationTabFromEntityName(getName(), visualiser->getViewType());
					result |= ot::SelectionHandlingEvent::ActiveViewChanged;
				}
			}
		}
	}

	result |= SceneNodeBase::setSelected(_selection, _selectionOrigin);

	return result;
}

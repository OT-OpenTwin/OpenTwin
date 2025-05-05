#include "stdafx.h"
#include "SceneNodeBase.h"
#include "TextVisualiser.h"
#include "TableVisualiser.h"
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"

#include "FrontendAPI.h"

SceneNodeBase::~SceneNodeBase()
{
	for (Visualiser* visualiser : m_visualiser)
	{
		delete visualiser;
		visualiser = nullptr;
	}

	if (m_parent != nullptr)
	{
		m_parent->removeChild(this);
	}
	std::list<SceneNodeBase*> currentChilds = m_children;
	for (auto child : currentChilds)
	{
		delete child;
	}
}

ot::SelectionHandlingResult SceneNodeBase::setSelected(bool _selected, ot::SelectionOrigin _selectionOrigin, bool _singleSelection)
{
	ot::SelectionHandlingResult result = ot::SelectionHandlingEvent::Default;

	// First we check if there is a state change
	if (!isSelected() && _selected || isSelected() && !_selected)
	{
		m_selected = _selected;
		const std::list<Visualiser*>& visualisers = getVisualiser();
		for (Visualiser* visualiser : visualisers)
		{
			// We have a valid state change, so we visualise all views, if they are not already opened in a view and the selection origins from a user interaction
			// In case that properties change, effectively a new entity is created (same ID, different version) and a new scene node is created. 
			// Therefore it is not necessary to compare the states of scenenode and entity. This algorithm only deals with the state of the view being
			// open or not.

			if (visualiser->mayVisualise())
			{
				VisualiserState state;
				state.m_selected = _selected;
				state.m_singleSelection = _singleSelection;
				state.m_selectionOrigin = _selectionOrigin;
				visualiser->visualise(state);
				result |= ot::SelectionHandlingEvent::NewViewRequested;
			}
			else if (visualiser->viewIsCurrentlyOpen())
			{
				// Here we just want to focus an already opened view.
				FrontendAPI::instance()->setCurrentVisualizationTabFromEntityName(getName(), visualiser->getViewType());
				result |= ot::SelectionHandlingEvent::ActiveViewChanged;
			}
		}
	}

	return result;

}
void SceneNodeBase::setHighlighted(bool _highlight)
{
	if (m_highlighted != _highlight) 
	{
		m_highlighted = _highlight;
		if (m_selectChildren) 
		{ 
			for (auto child : m_children)
			{
				child->setHighlighted(_highlight);
			}
		} 
	}
}

void SceneNodeBase::setViewChange(const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType)
{

	// Here we switch the view state changes
	if (_state == ot::ViewChangedStates::changesSaved)
	{
		const std::list< Visualiser*>& allVisualiser = getVisualiser();
		// We initiated a model state change from the ui. Now we request a new visualisation for every visualiser which is not the one that initiated the 
		// Model state change under the condition that the view is open.
		VisualiserState state;
		state.m_setFocus = false;
		state.m_selected = true;
		state.m_singleSelection = true;
		for (Visualiser* visualiser : allVisualiser)
		{
			if (visualiser->getViewType() != _viewType && visualiser->viewIsCurrentlyOpen()) {
				visualiser->visualise(state);
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
			else if (_viewType == ot::WidgetViewBase::ViewType::View1D)
			{
				PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);
				thisIsTheView |= plotVisualiser != nullptr;
				CurveVisualiser* curveVisualiser = dynamic_cast<CurveVisualiser*>(visualiser);
				thisIsTheView |= curveVisualiser != nullptr;

			}
			if (thisIsTheView)
			{
				visualiser->setViewIsOpen(viewIsOpen);
				break;
			}
		}
	}
}


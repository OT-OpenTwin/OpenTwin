// @otlicense
// File: SceneNodeBase.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "stdafx.h"
#include "SceneNodeBase.h"
#include "TextVisualiser.h"
#include "TableVisualiser.h"
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"

#include "FrontendAPI.h"

#include "OTCore/LogDispatcher.h"

#include <osg/Switch>

SceneNodeBase::~SceneNodeBase() {
	// Remove visualiser before deleting to avoid access to visualiser during deletion
	std::list<Visualiser*> visualisers = std::move(m_visualiser);
	m_visualiser.clear();

	for (Visualiser* visualiser : visualisers) {
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

	// Remove shape node from parent nodes
	if (getShapeNode() != nullptr) {
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++) {
			osg::Group* parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		m_shapeNode = nullptr;
	}
}

void SceneNodeBase::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("TreeItemID", m_treeItemID, _allocator);
	_object.AddMember("ModelEntityID", m_modelEntityID, _allocator);
	_object.AddMember("Editable", m_editable, _allocator);
	_object.AddMember("Visible", m_visible, _allocator);
	_object.AddMember("Selected", m_selected, _allocator);
	_object.AddMember("SelectionFromNavigationTree", m_selectionFromNavigationTree, _allocator);
	_object.AddMember("Transparent", m_transparent, _allocator);
	_object.AddMember("Wireframe", m_wireframe, _allocator);
	_object.AddMember("Highlighted", m_highlighted, _allocator);
	_object.AddMember("Offset", m_offset, _allocator);
	_object.AddMember("SelectChildren", m_selectChildren, _allocator);
	_object.AddMember("ManageVisibilityOfParent", m_manageVisibilityOfParent, _allocator);
	_object.AddMember("ManageVisibilityOfChildren", m_manageVisibilityOfChildren, _allocator);
	_object.AddMember("SelectionHandled", m_selectionHandled, _allocator);
	_object.AddMember("Errors", ot::JsonString(m_errors, _allocator), _allocator);
	if (m_parent) {
		_object.AddMember("ParentName", ot::JsonString(m_parent->getName(), _allocator), _allocator);
	}
	else {
		_object.AddMember("ParentName", ot::JsonNullValue(), _allocator);
	}

	ot::JsonArray childrenArr;
	for (const SceneNodeBase* child : m_children) {
		ot::JsonObject childObj;
		child->getDebugInformation(childObj, _allocator);
		childrenArr.PushBack(childObj, _allocator);
	}
	_object.AddMember("Children", childrenArr, _allocator);

	ot::JsonArray visualizersArr;
	for (const Visualiser* visualizer : m_visualiser) {
		ot::JsonObject visualizerObj;
		visualizer->getDebugInformation(visualizerObj, _allocator);
		visualizersArr.PushBack(visualizerObj, _allocator);
	}
	_object.AddMember("Visualizers", visualizersArr, _allocator);
}

ot::SelectionHandlingResult SceneNodeBase::setSelected(bool _selected, const ot::SelectionData& _selectionData, bool _singleSelection, const std::list<SceneNodeBase*>& _selectedNodes)
{
	ot::SelectionHandlingResult result = ot::SelectionHandlingEvent::Default;

	if (m_selected != _selected) {
		m_selected = _selected;

		const std::list<Visualiser*>& visualisers = getVisualiser();

		VisualiserState state;
		state.m_selected = _selected;
		state.m_singleSelection = _singleSelection;
		state.m_selectionData = _selectionData;
		state.m_anyVisualiserHasFocus = false;
		state.m_selectedNodes = _selectedNodes;

		bool skipViewHandling = _selectionData.getKeyboardModifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
		skipViewHandling |= _selectionData.isViewHandlingFlagSet(ot::ViewHandlingFlag::SkipViewHandling);

		// Check if any visualiser has focus
		for (Visualiser* visualiser : visualisers) {
			if (FrontendAPI::instance()->hasViewFocus(getName(), visualiser->getViewType())) {
				state.m_anyVisualiserHasFocus = true;
				break;
			}
		}

		for (Visualiser* visualiser : visualisers) {

			// We have a valid state change, so we visualise all views, if they are not already opened in a view and the selection origins from a user interaction
			// In case that properties change, effectively a new entity is created (same ID, different version) and a new scene node is created. 
			// Therefore it is not necessary to compare the states of scenenode and entity. This algorithm only deals with the state of the view being
			// open or not.

			if (m_selected) {
				// Entity was selected, so we want to visualise it

				if (visualiser->getViewIsOpen()) {
					// If the view is currently open and the entity is selected, we want to set the focus on the view
					// We do not want to focus every visualiser, so if any visualiser has focus, we do not set the focus again
					if (!state.m_anyVisualiserHasFocus) {
						if (!skipViewHandling) {
							FrontendAPI::instance()->setCurrentVisualizationTabFromEntityName(getName(), visualiser->getViewType());
						}
						
						result |= ot::SelectionHandlingEvent::ActiveViewChanged;

						state.m_anyVisualiserHasFocus = true;
					}
					
					FrontendAPI::instance()->addVisualizingEntityToView(m_treeItemID, getName(), visualiser->getViewType());

					// The visualizer may want to unhide/un-dim the visualisation
					visualiser->showVisualisation(state);
				}
				else if (visualiser->mayVisualise()) {
					// The view is not open and the visualiser is enabled
					
					if (visualiser->requestVisualization(state)) {
						// Visualisation was requested
						result |= ot::SelectionHandlingEvent::NewViewRequested;
					}
				}
			}
			else if (visualiser->getViewIsOpen()) {
				// Entity was deselected, so we potentially want to hide or dim the visualisation
				visualiser->hideVisualisation(state);
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

ot::UIDList SceneNodeBase::getVisualisedEntities() const {
	ot::UIDList entities;
	for (auto visualiser : m_visualiser) {
		entities.push_back(visualiser->getVisualizationEntity());
	}
	entities.unique();
	return entities;
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
		state.m_selectionData.setSelectionOrigin(ot::SelectionOrigin::User);
		for (Visualiser* visualiser : allVisualiser)
		{
			if (visualiser->getViewType() != _viewType && visualiser->getViewIsOpen()) {
				visualiser->requestVisualization(state);
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


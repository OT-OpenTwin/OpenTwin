#include <stdafx.h>
#include "PlotVisualiser.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

PlotVisualiser::PlotVisualiser(SceneNodeBase* _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D)
{
}

bool PlotVisualiser::requestVisualization(const VisualiserState& _state) {
	if (!m_alreadyRequestedVisualisation) {
		m_alreadyRequestedVisualisation = true;
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_VIEW1D_Setup, doc.GetAllocator());
	
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, getSceneNode()->getModelEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _state.m_setFocus, doc.GetAllocator());

		if (_state.m_selectionData.getKeyboardModifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier)) {
			doc.AddMember(OT_ACTION_PARAM_SuppressViewHandling, true, doc.GetAllocator());
		}
		else {
			doc.AddMember(OT_ACTION_PARAM_SuppressViewHandling, false, doc.GetAllocator());
		}

		// Find all selected entities for this plot
		
		doc.AddMember(OT_ACTION_PARAM_VisualizingEntities, ot::JsonArray(getVisualizingUIDs(_state), doc.GetAllocator()), doc.GetAllocator());

		FrontendAPI::instance()->messageModelService(doc.toJson());
		return true;
	}
	else {
		return false;
	}
}

void PlotVisualiser::showVisualisation(const VisualiserState& _state) {
	if (!this->getSceneNode()->getSelectionHandled()) {
		this->getSceneNode()->setSelectionHandled(true);

		FrontendAPI::instance()->clearVisualizingEntitesFromView(this->getSceneNode()->getName(), ot::WidgetViewBase::View1D);
	}

	for (const SceneNodeBase* node : this->getVisualizingEntities(_state)) {
		FrontendAPI::instance()->addVisualizingEntityToView(node->getTreeItemID(), this->getSceneNode()->getName(), ot::WidgetViewBase::View1D);
	}
}

void PlotVisualiser::hideVisualisation(const VisualiserState& _state) {

}

void PlotVisualiser::setViewIsOpen(bool _viewIsOpen)
{
	m_alreadyRequestedVisualisation = false;
	m_viewIsOpen = _viewIsOpen;

	for (SceneNodeBase* curve : m_node->getChildren())
	{
		assert(curve->getVisualiser().size() == 1);
		Visualiser* curveVisualiser = *curve->getVisualiser().begin();
		curveVisualiser->setViewIsOpen(_viewIsOpen);
	}
}

ot::UIDList PlotVisualiser::getVisualizingUIDs(const VisualiserState& _state) const {
	ot::UIDList visualizingUIDs;
	for (SceneNodeBase* node : this->getVisualizingEntities(_state)) {
		visualizingUIDs.push_back(node->getModelEntityID());
	}
	return visualizingUIDs;
}

std::list<SceneNodeBase*> PlotVisualiser::getVisualizingEntities(const VisualiserState& _state) const {
	std::list<SceneNodeBase*> visualizingEntities;
	for (SceneNodeBase* node : _state.m_selectedNodes) {
		if (node == this->getSceneNode()) {
			// Plot
			visualizingEntities.push_back(this->getSceneNode());
		}
		else if (node->getParent() == this->getSceneNode()) {
			// Curve
			visualizingEntities.push_back(node);
		}
	}

	visualizingEntities.unique();

	return visualizingEntities;
}
#include <stdafx.h>
#include "PlotVisualiser.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

PlotVisualiser::PlotVisualiser(SceneNodeBase* _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D)
{
}

void PlotVisualiser::visualise(const VisualiserState& _state)
{
	if (_state.m_selected && ! m_alreadyRequestedVisualisation)
	{
		m_alreadyRequestedVisualisation = true;
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_PARAM_VIEW1D_Setup, doc.GetAllocator());
	
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, getSceneNode()->getModelEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _state.m_setFocus, doc.GetAllocator());

		FrontendAPI::instance()->messageModelService(doc.toJson());
	}
}

void PlotVisualiser::setViewIsOpen(bool _viewIsOpen)
{
	m_viewIsOpen = _viewIsOpen;
	m_alreadyRequestedVisualisation = false;
}

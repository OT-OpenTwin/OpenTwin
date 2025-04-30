#include "stdafx.h"
#include "CurveVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

void CurveVisualiser::visualise(const VisualiserState& _state)
{
	if (!m_viewIsOpen)
	{
		SceneNodeBase* plot = m_node->getParent();
		const std::list<Visualiser*>& visualiser = plot->getVisualiser();
		assert(visualiser.size() == 1); //Currently, the plot has only a single visualiser
		auto plotVisualiser = *visualiser.begin();
	
		VisualiserState plotState;
		plotState.m_selected = true;
		plotState.m_setFocus = true;
		plotState.m_singleSelection = true;
		plotVisualiser->visualise(plotState);
	}
	else
	{

	}

	//Set gray = false
}

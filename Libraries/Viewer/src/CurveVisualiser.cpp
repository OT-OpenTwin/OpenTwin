#include "stdafx.h"
#include "CurveVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "PlotVisualiser.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

bool CurveVisualiser::visualise(const VisualiserState& _state)
{
	bool newVisualisation = false;
	if (!m_viewIsOpen)
	{
		SceneNodeBase* plot = m_node->getParent();
		const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
		assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser
		auto visualiser = *allVisualiser.begin();
		PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);
		if (!plotVisualiser->alreadyRequestedVisualisation())
		{
			VisualiserState plotState;
			plotState.m_selected = true;
			plotState.m_setFocus = true;
			plotState.m_singleSelection = true;
			plotVisualiser->visualise(plotState);
			newVisualisation = true;
		}
	}
	else
	{
		SceneNodeBase* plot = m_node->getParent();
		FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), !_state.m_selected);
	}
	return newVisualisation;
}

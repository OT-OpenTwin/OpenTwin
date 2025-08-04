#include "stdafx.h"
#include "CurveVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "PlotVisualiser.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

bool CurveVisualiser::requestVisualization(const VisualiserState& _state) {
	bool newVisualisation = false;

	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser
	
	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);
	
	OTAssertNullptr(plotVisualiser);

	if (!plotVisualiser->alreadyRequestedVisualisation()) {
		VisualiserState plotState;
		plotState.m_selected = true;
		plotState.m_setFocus = true;
		plotState.m_singleSelection = true;
		plotState.m_selectedNodes = _state.m_selectedNodes;
		plotVisualiser->requestVisualization(plotState);
		newVisualisation = true;
	}

	return newVisualisation;
}

void CurveVisualiser::showVisualisation(const VisualiserState& _state) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), false);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->showVisualisation(_state);
}

void CurveVisualiser::hideVisualisation(const VisualiserState& _state) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), true);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->hideVisualisation(_state);
}

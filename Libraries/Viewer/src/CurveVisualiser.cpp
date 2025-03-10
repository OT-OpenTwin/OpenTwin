#include "stdafx.h"
#include "CurveVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

void CurveVisualiser::visualise(bool _setFocus) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_MODEL_UpdateCurvesOfPlot, doc.GetAllocator());
	//doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_PARAM_VIEW1D_Setup, doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, getSceneNode()->getModelEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setFocus, doc.GetAllocator());

	FrontendAPI::instance()->messageModelService(doc.toJson());
}

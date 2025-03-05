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

	//BsonViewOrValue m_query;
	//BsonViewOrValue m_projection;
	//int m_limit;
	//ot::Plot1DCfg curveConfig;

	//const std::string _xAxisEntryName;
	//const std::string _yAxisEntryName;

	//DataStorageAPI::ResultDataStorageAPI access("collection");
	//DataStorageAPI::DataStorageResponse response = access.SearchInResultCollection(m_query, m_projection, m_limit);
	//auto queryResult =	response.getBsonResult();
	//size_t numberOfEntries = queryResult.value().length();
	//std::vector<double> xAxisValues, yAxisValues;
	//xAxisValues.reserve(numberOfEntries);
	//yAxisValues.reserve(numberOfEntries);

	////Hier gehe ich davon aus, dass keine Kurvenschar zurückgegeben wird.
	//if (queryResult.has_value())
	//{
	//	for (auto& entry : queryResult.value())
	//	{
	//		xAxisValues.push_back(entry[_xAxisEntryName].get_double().value);
	//		yAxisValues.push_back(entry[_yAxisEntryName].get_double().value);
	//	}
	//}
	//
	//ot::Plot* ownerPlot;
	//QString title = "";
	//int id = 0;
	//ot::PlotDataset m_dataset(ownerPlot,id,title , xAxisValues.data(), yAxisValues.data(), numberOfEntries);
	//ownerPlot->addDataset()

	//ot::Plot plot;
	//plot.setConfig(curveConfig); 



}

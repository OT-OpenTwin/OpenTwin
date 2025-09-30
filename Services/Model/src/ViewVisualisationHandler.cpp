#include <stdafx.h>
#include "ViewVisualisationHandler.h"
#include "Application.h"
#include "Model.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"
#include "IVisualisationPlot1D.h"
#include "IVisualisationCurve.h"
#include "IVisualisationGraphicsView.h"

#include "OTCore/RuntimeTests.h"


#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED==true
#define OT_TEST_VISUALIZATIONHANDLER_Interval(___testText) OT_TEST_Interval(ot_intern_visualizationhandler_lcl_performancetest, "ViewVisualisationHandler", ___testText)
#else
#define OT_TEST_VISUALIZATIONHANDLER_Interval(___testText)
#endif

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);

	ot::JsonDocument document;
	
	ot::JsonObject visualisationCfgJson;
	_visualisationCfg.addToJsonObject(visualisationCfgJson, document.GetAllocator());
	document.AddMember( OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJson, document.GetAllocator());
	
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	info.addToJsonObject(document, document.GetAllocator());

	if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_TABLE_Setup)
	{

		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize table");
		IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
		assert(tableEntity != nullptr);
		if (tableEntity != nullptr && tableEntity->visualiseTable())
		{
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());

			ot::TableCfg tableCfg = tableEntity->getTableConfig(_visualisationCfg.getOverrideViewerContent());
			ot::JsonObject cfgObj;
			tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_TEXTEDITOR_Setup)
	{
		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize text");

		IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
		assert(textEntity != nullptr);
		if (textEntity != nullptr && textEntity->visualiseText())
		{
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());

			ot::TextEditorCfg configuration = textEntity->createConfig(_visualisationCfg.getOverrideViewerContent());
			ot::JsonObject cfgObj;
			configuration.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_VIEW1D_Setup)
	{
		IVisualisationPlot1D* plotEntity = dynamic_cast<IVisualisationPlot1D*>(baseEntity);
		if (plotEntity != nullptr && plotEntity->visualisePlot())
		{
			EntityContainer* containerEntity = dynamic_cast<EntityContainer*>(plotEntity);
			std::list<EntityBase*> curveEntities = containerEntity->getChildrenList();

			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, document.GetAllocator());
			
			const ot::Plot1DCfg plotCfg = plotEntity->getPlot();
			ot::JsonObject cfgObj;
			plotCfg.addToJsonObject(cfgObj, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

			if(_visualisationCfg.getOverrideViewerContent())
			{
				ot::JsonArray curveCfgs;
				for (EntityBase* curveEntity : curveEntities)
				{
					IVisualisationCurve* curve = dynamic_cast<IVisualisationCurve*>(curveEntity);
					ot::Plot1DCurveCfg curveCfg = curve->getCurve();
					ot::JsonObject curveCfgSerialised;
					curveCfg.addToJsonObject(curveCfgSerialised, document.GetAllocator());
					curveCfgs.PushBack(curveCfgSerialised, document.GetAllocator());
				}

				document.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgs, document.GetAllocator());
			}

			std::string response;
		}
		else
		{
			OT_LOG_E("Tried visualising an entity as plot which is not a plot.");
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor)
	{
		IVisualisationGraphicsView* graphicsView = dynamic_cast<IVisualisationGraphicsView*>(baseEntity);
		if (graphicsView != nullptr && graphicsView->visualiseGraphicsView())
		{
			document.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, document.GetAllocator()), document.GetAllocator());

			ot::GraphicsNewEditorPackage* editor = graphicsView->getGraphicsEditorPackage();
			ot::JsonObject pckgObj;
			editor->addToJsonObject(pckgObj, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, document.GetAllocator());
			
			ot::BasicServiceInformation info(baseEntity->getOwningService());
			info.addToJsonObject(document, document.GetAllocator());
			std::string response;
		}
	}
	else
	{
		OT_LOG_WAS("Unknown visualization request: \"" + _visualisationCfg.getVisualisationType() + "\"");
		return;
	}

	Application::instance()->queuedRequestToFrontend(document);
}

void ViewVisualisationHandler::handleRenaming(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	const bool includeData = false;
	

	IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
	
	ot::JsonDocument documentBase;
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	info.addToJsonObject(documentBase, documentBase.GetAllocator());
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setAsActiveView(true);
	visualisationCfg.setOverrideViewerContent(true);
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, documentBase.GetAllocator());
	documentBase.AddMember( OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJson, documentBase.GetAllocator());

	if (tableEntity != nullptr && tableEntity->visualiseTable())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, singleRequest.GetAllocator());
		
		ot::TableCfg tableCfg = tableEntity->getTableConfig(includeData);
		ot::JsonObject cfgObj;
		tableCfg.addToJsonObject(cfgObj, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
	if (textEntity != nullptr && textEntity->visualiseText())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, singleRequest.GetAllocator());

		ot::TextEditorCfg configuration = textEntity->createConfig(includeData);
		ot::JsonObject cfgObj;
		configuration.addToJsonObject(cfgObj, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());
		
		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	IVisualisationPlot1D* plotEntity = dynamic_cast<IVisualisationPlot1D*>(baseEntity);
	if (plotEntity != nullptr && plotEntity->visualisePlot())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, singleRequest.GetAllocator());

		const ot::Plot1DCfg plotCfg = plotEntity->getPlot();
		ot::JsonObject cfgObj;
		plotCfg.addToJsonObject(cfgObj, singleRequest.GetAllocator());
		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	IVisualisationCurve* curve = dynamic_cast<IVisualisationCurve*>(baseEntity);
	if (curve != nullptr && curve->visualiseCurve())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UpdateCurvesOfPlot, singleRequest.GetAllocator());

		const std::string plotName = baseEntity->getParent()->getName();
		singleRequest.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, singleRequest.GetAllocator()), singleRequest.GetAllocator());

		ot::JsonObject curveCfgSerialised;
		ot::Plot1DCurveCfg curveCfg = curve->getCurve();
		curveCfg.addToJsonObject(curveCfgSerialised, singleRequest.GetAllocator());
		singleRequest.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgSerialised, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

}

void ViewVisualisationHandler::setupPlot(EntityBase* _plotEntityBase, bool _setAsActiveView)
{

}

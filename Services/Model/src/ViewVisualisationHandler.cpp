#include <stdafx.h>
#include "ViewVisualisationHandler.h"
#include "Application.h"
#include "Model.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"
#include "IVisualisationPlot1D.h"
#include "IVisualisationCurve.h"

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

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID, const std::string& _visualisationType, bool _setAsActiveView, bool _overrideContent)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	if (_visualisationType == OT_ACTION_CMD_UI_TABLE_Setup)
	{

		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize table");
		IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
		assert(tableEntity != nullptr);
		if (tableEntity != nullptr && tableEntity->visualiseTable())
		{
			ot::JsonDocument document;
			info.addToJsonObject(document, document.GetAllocator());
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setAsActiveView, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_OverwriteContent, _overrideContent, document.GetAllocator());
		
			ot::TableCfg tableCfg = tableEntity->getTableConfig(_overrideContent);
			ot::JsonObject cfgObj;
			tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
			std::string response;
			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else if (_visualisationType == OT_ACTION_CMD_UI_TEXTEDITOR_Setup)
	{
		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize text");

		IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
		assert(textEntity != nullptr);
		if (textEntity != nullptr && textEntity->visualiseText())
		{
			ot::JsonDocument document;
			info.addToJsonObject(document, document.GetAllocator());
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setAsActiveView, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_OverwriteContent, _overrideContent, document.GetAllocator());

			ot::TextEditorCfg configuration = textEntity->createConfig(_overrideContent);
			ot::JsonObject cfgObj;
			configuration.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
			std::string response;
			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else if (_visualisationType == OT_ACTION_CMD_VIEW1D_Setup)
	{
		IVisualisationPlot1D* plotEntity = dynamic_cast<IVisualisationPlot1D*>(baseEntity);
		if (plotEntity != nullptr && plotEntity->visualisePlot())
		{
			EntityContainer* containerEntity = dynamic_cast<EntityContainer*>(plotEntity);
			std::list<EntityBase*> curveEntities = containerEntity->getChildrenList();

			ot::JsonDocument document;
			info.addToJsonObject(document, document.GetAllocator());
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setAsActiveView, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_OverwriteContent, _overrideContent, document.GetAllocator());

			const ot::Plot1DCfg plotCfg = plotEntity->getPlot();
			ot::JsonObject cfgObj;
			plotCfg.addToJsonObject(cfgObj, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

			if(_overrideContent)
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
			Application::instance()->queuedRequestToFrontend(document);
		}
		else
		{
			OT_LOG_E("Tried visualising an entity as plot which is not a plot.");
		}
	}
	else
	{
		OT_LOG_WAS("Unknown visualization request: \"" + _visualisationType + "\"");
	}
}

void ViewVisualisationHandler::handleRenaming(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	const bool includeData = false;
	const bool setAsActiveView = true;
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);

	IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
	if (tableEntity != nullptr && tableEntity->visualiseTable())
	{
		ot::JsonDocument document;
		info.addToJsonObject(document, document.GetAllocator());
		document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, setAsActiveView, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_OverwriteContent, includeData, document.GetAllocator());
		
		ot::TableCfg tableCfg = tableEntity->getTableConfig(includeData);
		ot::JsonObject cfgObj;
		tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

		document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		std::string response;
		Application::instance()->queuedRequestToFrontend(document);
	}

	IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
	if (textEntity != nullptr && textEntity->visualiseText())
	{
		ot::JsonDocument document;
		info.addToJsonObject(document, document.GetAllocator());
		document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, setAsActiveView, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_OverwriteContent, includeData, document.GetAllocator());
		
		ot::TextEditorCfg configuration = textEntity->createConfig(includeData);
		ot::JsonObject cfgObj;
		configuration.addToJsonObject(cfgObj, document.GetAllocator());

		document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		std::string response;
		Application::instance()->queuedRequestToFrontend(document);
	}

	IVisualisationPlot1D* plotEntity = dynamic_cast<IVisualisationPlot1D*>(baseEntity);
	if (plotEntity != nullptr && plotEntity->visualisePlot())
	{

		ot::JsonDocument document;
		info.addToJsonObject(document, document.GetAllocator());
		document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, setAsActiveView, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_OverwriteContent, includeData, document.GetAllocator());

		const ot::Plot1DCfg plotCfg = plotEntity->getPlot();
		ot::JsonObject cfgObj;
		plotCfg.addToJsonObject(cfgObj, document.GetAllocator());
		document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

		std::string response;
		Application::instance()->queuedRequestToFrontend(document);
	}

}

void ViewVisualisationHandler::setupPlot(EntityBase* _plotEntityBase, bool _setAsActiveView)
{

}

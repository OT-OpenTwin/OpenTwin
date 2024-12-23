#include <stdafx.h>
#include "ViewVisualisationHandler.h"
#include "Application.h"
#include "Model.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"

#include "OTCore/PerformanceTests.h"

#define OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED false

#if OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED==true
#include "OTCore/PerformanceTests.h"
#define OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCE_TEST(___testText) OT_TEST_Interval(ot_intern_entityfiletext_lcl_performancetest, "ViewVisualisationHandler " ___testText)
#else
#define OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCE_TEST(___testText)
#endif

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID, const std::string& _visualisationType, bool _setAsActiveView, bool _overrideContent)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	if (_visualisationType == OT_ACTION_CMD_UI_TABLE_Setup)
	{
		OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCE_TEST("Visualize table");

		IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
		assert(tableEntity != nullptr);
		if (tableEntity != nullptr && tableEntity->visualiseTable())
		{
			ot::JsonDocument document;
			info.addToJsonObject(document, document.GetAllocator());
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setAsActiveView, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_OverwriteContent, _overrideContent, document.GetAllocator());
		
			ot::TableCfg tableCfg = tableEntity->getTableConfig();
			ot::JsonObject cfgObj;
			tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
			std::string response;
			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else if (_visualisationType == OT_ACTION_CMD_UI_TEXTEDITOR_Setup)
	{
		OT_INTERN_VISUALIZATIONHANDLER_PERFORMANCE_TEST("Visualize text");

		IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
		assert(textEntity != nullptr);
		if (textEntity != nullptr && textEntity->visualiseText())
		{
			ot::JsonDocument document;
			info.addToJsonObject(document, document.GetAllocator());
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _setAsActiveView, document.GetAllocator());

			ot::TextEditorCfg configuration = textEntity->createConfig();
			ot::JsonObject cfgObj;
			configuration.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
			std::string response;
			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else
	{
		OT_LOG_WAS("Unknown visualization request: \"" + _visualisationType + "\"");
	}
}

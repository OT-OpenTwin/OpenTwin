// @otlicense

// OpenTwin header
#include "TestCode.h"

void TestCode::initialize()
{
	connectAction("TestCode", this, &TestCode::testCodeSetupDialogCallback);
}

void TestCode::runTestCode()
{
	using namespace ot;
	PropertyDialogCfg cfg;

	m_app->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, DialogHandler::createDialogRequest(cfg, "TestCode"));
	
}

void TestCode::testCodeSetupDialogCallback(ot::JsonDocument& _document)
{
	using namespace ot;

	PropertyDialogCfg cfg;
	cfg.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));

	const auto& grid = cfg.getGridConfig();
	
}


// @otlicense

#pragma once

// OpenTwin header
#include "Application.h"
#include "OTSystem/DateTime.h"
#include "OTGui/Properties/PropertyInt.h"
#include "OTGui/Properties/PropertyBool.h"
#include "OTGui/Properties/PropertyColor.h"
#include "OTGui/Properties/PropertyGroup.h"
#include "OTGui/Properties/PropertyDouble.h"
#include "OTGui/Properties/PropertyString.h"
#include "OTGui/Properties/PropertyGridCfg.h"
#include "OTGui/Properties/PropertyPainter2D.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTGui/Dialog/PropertyDialogCfg.h"
#include "OTGuiAPI/Frontend.h"
#include "OTGuiAPI/DialogHandler.h"
#include "OTCommunication/Handler/ActionHandler.h"

// std header
#include <string>
#include <optional>

class TestCode : public ot::ActionHandler
{
	OT_DECL_NOCOPY(TestCode)
	OT_DECL_NOMOVE(TestCode)
	OT_DECL_NODEFAULT(TestCode)
public:
	TestCode(Application* _app) : m_app(_app) {};

	void initialize();

	void runTestCode();

private:
	void testCodeSetupDialogCallback(ot::JsonDocument& _document);

	Application* m_app;

};
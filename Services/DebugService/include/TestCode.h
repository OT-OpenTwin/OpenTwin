// @otlicense
// File: TestCode.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
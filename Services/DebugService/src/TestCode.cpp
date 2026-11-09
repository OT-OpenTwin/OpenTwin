// @otlicense
// File: TestCode.cpp
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


// @otlicense
// File: ScriptEngine.cpp
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
#include "AppBase.h"
#include "ToolBar.h"
#include "ScriptEngine.h"
#include "OTWidgets/GlobalWidgetViewManager.h"

ScriptEngine::ScriptEngine(QObject* _parent) 
	: QJSEngine(_parent)
{

}

ScriptEngine::~ScriptEngine() {

}

bool ScriptEngine::initialize(AppBase* _app) {
	// Register global objects
	
	registerObject("app", _app);
	registerObject("views", &ot::GlobalWidgetViewManager::instance());

	if (_app->getToolBar()) {
		registerToolBar(_app->getToolBar());
	}

	return true;
}

void ScriptEngine::registerToolBar(QObject* _toolBar) {
	registerObject("ttb", _toolBar);
}

void ScriptEngine::registerObject(const QString& _name, QObject* _object) {
	globalObject().setProperty(_name, newQObject(_object));
	setObjectOwnership(_object, QJSEngine::CppOwnership);
}

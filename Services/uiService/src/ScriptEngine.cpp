// @otlicense

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

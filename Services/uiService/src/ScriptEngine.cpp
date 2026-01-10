// @otlicense

// OpenTwin header
#include "AppBase.h"
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
	
	globalObject().setProperty("AppBase", newQObject(_app));
	setObjectOwnership(_app, QJSEngine::CppOwnership);

	globalObject().setProperty("ViewManager", newQObject(&ot::GlobalWidgetViewManager::instance()));
	setObjectOwnership(&ot::GlobalWidgetViewManager::instance(), QJSEngine::CppOwnership);
	return true;
}

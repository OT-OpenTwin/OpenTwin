// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ScriptEngine.h"

ScriptEngine::ScriptEngine(QObject* _parent) 
	: QJSEngine(_parent)
{

}

ScriptEngine::~ScriptEngine() {

}

bool ScriptEngine::initialize(AppBase* _app) {
	// Register global objects
	globalObject().setProperty("Engine",newQObject(this));
	globalObject().setProperty("AppBase", newQObject(_app));

	return true;
}

void ScriptEngine::setLogInDialog(QObject* _dialogObject) {
	if (_dialogObject) {
		Q_EMIT logInDialogAvailable(_dialogObject);
	}
}

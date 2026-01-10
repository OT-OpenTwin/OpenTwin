// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtQml/qjsengine.h>

class AppBase;

//! @brief The ScriptEngine class is used to execute JavaScript code within the OpenTwin frontend.
class ScriptEngine : public QJSEngine {
	Q_OBJECT
	OT_DECL_NODEFAULT(ScriptEngine)
	OT_DECL_NOCOPY(ScriptEngine)
	OT_DECL_NOMOVE(ScriptEngine)
public:
	ScriptEngine(QObject* _parent);
	~ScriptEngine();
	
	bool initialize(AppBase* _app);

	void registerToolBar(QObject* _toolBar);

private:
	void registerObject(const QString& _name, QObject* _object);
};
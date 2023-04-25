// Toolkit header
#include "AbstractTool.h"
#include "AppBase.h"
#include "StatusBar.h"

QString OToolkitAPI::AbstractTool::applicationName(void) const {
	return QString(APP_BASE_APP_NAME);
}

void OToolkitAPI::AbstractTool::displayStatusText(const QString& _text) const {
	AppBase::instance()->sb()->setInfo(_text);
}

void OToolkitAPI::AbstractTool::displayErrorStatusText(const QString& _text) const {
	AppBase::instance()->sb()->setErrorInfo(_text);
}
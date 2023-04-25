#include <openTwin/AbstractUIWidgetInterface.h>
#include "OpenTwinCore/otAssert.h"

void ot::AbstractUIWidgetInterface::appenInfoMessage(const char * _message) {
	appenInfoMessage(QString(_message));
}

void ot::AbstractUIWidgetInterface::appenInfoMessage(const std::string& _message) {
	appenInfoMessage(QString(_message.c_str()));
}

void ot::AbstractUIWidgetInterface::appenDebugMessage(const char * _message) {
	appenDebugMessage(QString(_message));
}

void ot::AbstractUIWidgetInterface::appenDebugMessage(const std::string& _message) {
	appenDebugMessage(QString(_message.c_str()));
}
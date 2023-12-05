#include <openTwin/AbstractUIWidgetInterface.h>
#include "OTCore/OTAssert.h"

void ot::AbstractUIWidgetInterface::appenInfoMessage(const char * _message) {
	this->appenInfoMessage(QString(_message));
}

void ot::AbstractUIWidgetInterface::appenInfoMessage(const std::string& _message) {
	this->appenInfoMessage(QString(_message.c_str()));
}

void ot::AbstractUIWidgetInterface::appenDebugMessage(const char * _message) {
	this->appenDebugMessage(QString(_message));
}

void ot::AbstractUIWidgetInterface::appenDebugMessage(const std::string& _message) {
	this->appenDebugMessage(QString(_message.c_str()));
}
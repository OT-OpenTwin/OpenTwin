#include "StudioSuiteConnector/ServiceConnector.h"

#include <QMetaObject>

void ServiceConnector::sendExecuteRequest(ot::JsonDocument& doc)
{
	assert(mainObject != nullptr);
	assert(!serviceURL.empty());

	std::string message = doc.toJson();

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	char* url = new char[serviceURL.length() + 1];
	strcpy(url, serviceURL.c_str());

	QMetaObject::invokeMethod(mainObject, "sendExecuteRequest", Qt::QueuedConnection, Q_ARG(const char*, url), Q_ARG(const char*, msg));
}


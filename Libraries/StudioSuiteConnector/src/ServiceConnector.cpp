#include "StudioSuiteConnector/ServiceConnector.h"

#include <QMetaObject>

bool ServiceConnector::sendExecuteRequest(ot::JsonDocument& doc, std::string& response)
{
	assert(mainObject != nullptr);
	assert(!serviceURL.empty());

	std::string message = doc.toJson();

	char* responseChar = nullptr;

	QMetaObject::invokeMethod(mainObject, "sendExecuteRequest", Qt::DirectConnection, Q_RETURN_ARG(char*, responseChar), Q_ARG(const char*, serviceURL.c_str()), Q_ARG(const char*, message.c_str()));

	response = responseChar;

	delete[] responseChar; responseChar = nullptr;

	return true;
}


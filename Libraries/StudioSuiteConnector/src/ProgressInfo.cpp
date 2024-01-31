#include "StudioSuiteConnector/ProgressInfo.h"

#include <QMetaObject>


void ProgressInfo::setProgressState(bool visible, const std::string& message, bool continuous)
{
	assert(mainObject != nullptr);

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	QMetaObject::invokeMethod(mainObject, "setProgressState", Qt::QueuedConnection, Q_ARG(bool, visible), Q_ARG(const char*, msg), Q_ARG(bool, continuous));
}

void ProgressInfo::setProgressValue(int percentage)
{
	assert(mainObject != nullptr);

	QMetaObject::invokeMethod(mainObject, "setProgressValue", Qt::QueuedConnection, Q_ARG(int, percentage));
}

void ProgressInfo::unlockGui(void)
{
	assert(mainObject != nullptr);

	QMetaObject::invokeMethod(mainObject, "unlockGui", Qt::QueuedConnection);
}

void ProgressInfo::showError(const std::string& message)
{
	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	QMetaObject::invokeMethod(mainObject, "showError", Qt::QueuedConnection, Q_ARG(const char*, msg));
}

void ProgressInfo::showInformation(const std::string& message)
{
	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	QMetaObject::invokeMethod(mainObject, "showInformation", Qt::QueuedConnection, Q_ARG(const char*, msg));
}



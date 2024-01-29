#include "StudioSuiteConnector/ProgressInfo.h"

#include <QMetaObject>


void ProgressInfo::setProgressState(bool visible, const std::string& message, bool continuous)
{
	assert(mainObject != nullptr);

	QMetaObject::invokeMethod(mainObject, "setProgressState", Qt::DirectConnection, Q_ARG(bool, visible), Q_ARG(const char*, message.c_str()), Q_ARG(bool, continuous));
}

void ProgressInfo::setProgressValue(int percentage)
{
	assert(mainObject != nullptr);

	QMetaObject::invokeMethod(mainObject, "setProgressValue", Qt::DirectConnection, Q_ARG(int, percentage));
}


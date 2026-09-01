// @otlicense

// OpenTwin header
#include "OTCore/Debugging/CoreDebug.h"
#include "OTWidgets/Debugging/GlobalEventLogger.h"

// Qt header
#include <QtCore/qdebug.h>

void ot::GlobalEventLogger::install(const std::initializer_list<QEvent::Type>& _eventTypes)
{
	static std::atomic_bool g_installed{ false };

	if (g_installed.exchange(true))
	{
		OT_LOG_E("GlobalEventLogger is already installed.");
		return;
	}

	// Create the logger
	GlobalEventLogger* logger = new GlobalEventLogger(_eventTypes);

	// Install the logger as event filter for all objects
	QGuiApplication::instance()->installEventFilter(logger);
	logger->setParent(QGuiApplication::instance());
}

bool ot::GlobalEventLogger::eventFilter(QObject* _obj, QEvent* _event)
{
	if (_event->type() >= QEvent::User)
	{
		const QString dbgString = QDebug::toString(QEvent::User);
		OT_BASE_DEBUG_LOG("GLOBALEVENTS", dbgString.toStdString());
	}
	else if (m_enabledEventTypes[static_cast<int>(_event->type())])
	{
		const QString dbgString = QDebug::toString(_event->type());
		OT_BASE_DEBUG_LOG("GLOBALEVENTS", dbgString.toStdString());
	}

	return false;
}

ot::GlobalEventLogger::GlobalEventLogger(const std::initializer_list<QEvent::Type>& _eventTypes)
	: m_enabledEventTypes(false)
{
	for (const auto& type : _eventTypes)
	{
		if (type < QEvent::User)
		{
			m_enabledEventTypes[static_cast<int>(type)] = true;
		}
	}
}
// @otlicense

// Frontend header
#include "StatusBarManager.h"

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/Style/IconManager.h"
#include "OTWidgets/Widgets/Label.h"

StatusBarManager::StatusBarManager(QMainWindow* _window)
{
	m_statusBar = new ot::DefaultStatusBar(_window);
	_window->setStatusBar(m_statusBar);

	ot::Label* logIntensityL = new ot::Label("Log Intensity:", m_statusBar);
	m_logIntensity = new ot::Label(m_statusBar);

	m_statusBar->addRightCornerWidget(logIntensityL);
	m_statusBar->addRightCornerWidget(m_logIntensity);

	updateLogIntensityInfo();
}

StatusBarManager::~StatusBarManager()
{
}

void StatusBarManager::updateLogIntensityInfo()
{
	ot::LogFlags flags = ot::LogDispatcher::instance().getLogFlags();

	std::string info = "Currently enabled log flags:";

	if (flags & ot::INFORMATION_LOG) info.append("\n - Info");
	if (flags & ot::DETAILED_LOG) info.append("\n - Detailed Info");
	if (flags & ot::WARNING_LOG) info.append("\n - Warning");
	if (flags & ot::ERROR_LOG) info.append("\n - Error");
	if (flags & ot::ALL_INCOMING_MESSAGE_LOG_FLAGS) info.append("\n - Inbound Messages");
	if (flags & ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS) info.append("\n - Outbound Messages");
	if (flags & ot::TEST_LOG) info.append("\n - Testing");

	if (flags == ot::NO_LOG)
	{
		// NONE
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/NoLogging.png"));
		m_logIntensity->setToolTip("Logging is disabled.");
	}
	else if (flags & (~(ot::WARNING_LOG | ot::ERROR_LOG)))
	{
		// FULL
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/FullLogging.png"));
		m_logIntensity->setToolTip(QString::fromStdString(info));
	}
	else
	{
		// DEFAULT
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/BasicLogging.png"));
		m_logIntensity->setToolTip(QString::fromStdString(info));
	}
}
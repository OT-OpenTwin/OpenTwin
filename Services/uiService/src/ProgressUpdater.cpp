#include "AppBase.h"
#include "ProgressUpdater.h"

ProgressUpdater::ProgressUpdater(std::string& _message, uint64_t _totalNbSteps)
	: m_totalNbSteps(static_cast<double>(_totalNbSteps)), m_app(nullptr),
	m_timeTrigger(std::chrono::seconds(2)), m_priorTimestamp(std::chrono::steady_clock::now())
{
	m_app = AppBase::instance();
	OTAssertNullptr(m_app);

	QMetaObject::invokeMethod(m_app, "slotSetProgressBarVisibility", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(_message)), Q_ARG(bool, true), Q_ARG(bool, false));
	QMetaObject::invokeMethod(m_app, "slotSetProgressBarValue", Qt::QueuedConnection, Q_ARG(int, 0));
}

ProgressUpdater::~ProgressUpdater() {
	QMetaObject::invokeMethod(m_app, "slotSetProgressBarVisibility", Qt::QueuedConnection, Q_ARG(QString, ""), Q_ARG(bool, false), Q_ARG(bool, false));
}

void ProgressUpdater::triggerUpdate(uint64_t _currentStep) {
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_priorTimestamp);

	if (m_totalNbSteps > 0 && (duration >= m_timeTrigger) || _currentStep == m_totalNbSteps) {
		uint32_t percentage = static_cast<uint32_t>((_currentStep / m_totalNbSteps) * 100 + 0.5);
		m_priorTimestamp = currentTime;

		QMetaObject::invokeMethod(m_app, "slotSetProgressBarValue", Qt::QueuedConnection, Q_ARG(int, percentage));
	}
}


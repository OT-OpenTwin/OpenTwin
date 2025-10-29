// @otlicense

#include "AppBase.h"
#include "ProgressUpdater.h"

ProgressUpdater::ProgressUpdater(std::string& _message, uint64_t _totalNbSteps)
	: m_totalNbSteps(static_cast<double>(_totalNbSteps)), m_app(nullptr),
	m_timeTrigger(std::chrono::seconds(2)), m_priorTimestamp(std::chrono::steady_clock::now())
{
	m_app = AppBase::instance();
	OTAssertNullptr(m_app);

	QMetaObject::invokeMethod(m_app, &AppBase::slotSetProgressBarVisibility, Qt::QueuedConnection, QString::fromStdString(_message), true, false);
	QMetaObject::invokeMethod(m_app, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, 0);
}

ProgressUpdater::~ProgressUpdater() {
	QMetaObject::invokeMethod(m_app, &AppBase::slotSetProgressBarVisibility, Qt::QueuedConnection, QString(), false, false);
}

void ProgressUpdater::triggerUpdate(uint64_t _currentStep) {
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_priorTimestamp);

	if (m_totalNbSteps > 0 && (duration >= m_timeTrigger) || _currentStep == m_totalNbSteps) {
		uint32_t percentage = static_cast<uint32_t>((_currentStep / m_totalNbSteps) * 100 + 0.5);
		m_priorTimestamp = currentTime;

		QMetaObject::invokeMethod(m_app, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, percentage);
	}
}


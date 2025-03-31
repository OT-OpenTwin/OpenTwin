//! @file DelayedCallHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/DelayedCallHandler.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qeventloop.h>

ot::DelayedCallHandler::DelayedCallHandler(int _delayInMs) :
	m_waitForCall(false)
{
	m_timer.setSingleShot(true);
	m_timer.setInterval(_delayInMs);

	this->connect(&m_timer, &QTimer::timeout, this, &DelayedCallHandler::slotTimeout);
}

void ot::DelayedCallHandler::setDelay(int _delayInMs) {
	m_timer.setInterval(_delayInMs);
}

int ot::DelayedCallHandler::getDelay(void) const {
	return m_timer.interval();
}

void ot::DelayedCallHandler::callDelayed(bool _waitUntilCalled) {
	if (m_waitForCall) {
		m_timer.stop();
	}
	if (m_timer.interval() > 0) {
		m_waitForCall = _waitUntilCalled;

		m_timer.start();

		while (m_waitForCall) {
			QEventLoop loop;
			loop.processEvents(QEventLoop::ExcludeUserInputEvents);
		}
	}
	else {
		m_waitForCall = true;
		this->timeout();
	}
}

void ot::DelayedCallHandler::stop(void) {
	m_waitForCall = false;
	m_timer.stop();
}

void ot::DelayedCallHandler::slotTimeout(void) {
	if (!m_waitForCall) {
		return;
	}

	m_waitForCall = false;
	Q_EMIT timeout();
}
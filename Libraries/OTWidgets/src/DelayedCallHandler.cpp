// @otlicense
// File: DelayedCallHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
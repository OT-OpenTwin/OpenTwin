// @otlicense
// File: ProgressUpdater.cpp
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


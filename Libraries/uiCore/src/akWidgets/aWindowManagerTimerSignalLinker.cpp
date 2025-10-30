// @otlicense
// File: aWindowManagerTimerSignalLinker.cpp
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

// AK header
#include <akCore/aException.h>
#include <akWidgets/aWindowManager.h>
#include <akWidgets/aWindowManagerTimerSignalLinker.h>

// Qt header
#include <qtimer.h>								// QTimer

ak::aWindowManagerTimerSignalLinker::aWindowManagerTimerSignalLinker(
	aWindowManager *				_uiManager
) {
	if (_uiManager == nullptr) { throw aException("Is nullptr", "Check ui manager"); }
	m_uiManager = _uiManager;
}

ak::aWindowManagerTimerSignalLinker::~aWindowManagerTimerSignalLinker() {
	for (int i = 0; i < m_links.size(); i++) {
		switch (m_links.at(i).type)
		{
		case progressShow:
			disconnect(m_links.at(i).timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutProgressShow);
			break;
		case progressHide:
			disconnect(m_links.at(i).timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutProgressHide);
			break;
		case statusLabelShow:
			disconnect(m_links.at(i).timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutLabelShow);
			break;
		case statusLabelHide:
			disconnect(m_links.at(i).timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutLabelHide);
			break;
		case timerType::showWindow:
			disconnect(m_links.at(i).timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutShowWindow);
			break;
		default:
			assert(0); // not implemented timer type
			break;
		}
	}
}

void ak::aWindowManagerTimerSignalLinker::addLink(
	QTimer *											_timer,
	timerType		_timerType
) {
	switch (_timerType)
	{
	case progressShow:
		connect(_timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutProgressShow); break;
	case progressHide:
		connect(_timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutProgressHide); break;
	case statusLabelShow:
		connect(_timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutLabelShow); break;
	case statusLabelHide:
		connect(_timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutLabelHide); break;
	case showWindow:
		connect(_timer, &QTimer::timeout, this, &aWindowManagerTimerSignalLinker::timerTimeoutShowWindow); break;
	default:
		assert(0); // not implemented timer type
		break;
	}
	m_links.push_back(timer{ _timer, _timerType });
}

void ak::aWindowManagerTimerSignalLinker::timerTimeoutLabelHide() { m_uiManager->setStatusLabelVisible(false, false); }

void ak::aWindowManagerTimerSignalLinker::timerTimeoutLabelShow() { m_uiManager->setStatusLabelVisible(true, false); }

void ak::aWindowManagerTimerSignalLinker::timerTimeoutProgressHide() { m_uiManager->setStatusBarVisible(false, false); }

void ak::aWindowManagerTimerSignalLinker::timerTimeoutProgressShow() { m_uiManager->setStatusBarVisible(true, false); }

void ak::aWindowManagerTimerSignalLinker::timerTimeoutShowWindow() { m_uiManager->showMaximized(); }

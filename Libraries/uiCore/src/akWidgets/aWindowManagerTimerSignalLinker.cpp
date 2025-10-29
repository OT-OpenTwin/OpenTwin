// @otlicense

/*
 *	File:		aWindowManagerTimerSignalLinker.cpp
 *	Package:	akWidgets
 *
 *  Created on: February 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

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

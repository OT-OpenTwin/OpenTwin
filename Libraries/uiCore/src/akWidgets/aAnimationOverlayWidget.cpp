// @otlicense
// File: aAnimationOverlayWidget.cpp
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

// uiCore header
#include <akCore/aAssert.h>
#include <akGui/aTimer.h>
#include <akWidgets/aAnimationOverlayWidget.h>

// Qt header
#include <qevent.h>
#include <qmovie.h>
#include <qlabel.h>

ak::aAnimationOverlayWidget::aAnimationOverlayWidget(QWidget* _parent)
	: QWidget(_parent), m_waitingAnimationVisible(false), m_animationDelay(0), m_animationDelayRequested(false)
{
	m_childWidget = nullptr;
	m_waitingLabel = new QLabel("", this);
	m_waitingLabel->setAttribute(Qt::WA_NoSystemBackground);
	m_waitingLabel->setParent(this);
	m_waitingLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
	m_waitingLabel->hide();

	m_delayTimer = new aTimer;

	connect(m_delayTimer, &aTimer::timeout, this, &aAnimationOverlayWidget::slotTimeout);
}

ak::aAnimationOverlayWidget::~aAnimationOverlayWidget() {
	if (m_waitingLabel->movie()) { m_waitingLabel->movie()->stop(); }
	delete m_waitingLabel;
	delete m_delayTimer;
}

void ak::aAnimationOverlayWidget::setChild(
	QWidget *		_widget
) {
	if (_widget == m_childWidget) {
		return;
	}

	QSize s = size();

	if (m_childWidget) {
		m_childWidget->setParent(nullptr);
		m_childWidget->hide();
	}

	m_childWidget = _widget;
	if (m_childWidget != nullptr) {
		_widget->setParent(this);
		_widget->resize(s);
		_widget->show();
	}

	if (m_waitingAnimationVisible) {
		setWaitingAnimationVisible(true);
	}
}

void ak::aAnimationOverlayWidget::resizeEvent(QResizeEvent * _event) {
	if (m_childWidget != nullptr) {
		m_childWidget->resize(size());
	}
	m_waitingLabel->resize(size());
}

void ak::aAnimationOverlayWidget::setWaitingAnimationVisible(
	bool			_visible
) {

	if (m_waitingLabel->movie() == nullptr && !m_waitingAnimationVisible) {
		aAssert(0, "No movie set"); 
		return;
	}
	else if (m_waitingLabel->movie() == nullptr && m_waitingAnimationVisible) {
		aAssert(0, "Movie is not accessable anymore");
		return;
	}
	m_waitingAnimationVisible = _visible;
	if (m_waitingAnimationVisible) {
		if (m_animationDelayRequested) { m_delayTimer->stop(); m_animationDelayRequested = false; }

		if (m_animationDelay > 0) {
			m_delayTimer->setInterval(m_animationDelay);
			m_delayTimer->setSingleShot(true);
			m_delayTimer->start();
			m_animationDelayRequested = false;
		}
		else {
			if (m_childWidget == nullptr) { m_waitingLabel->setParent(this); }
			else { m_waitingLabel->setParent(m_childWidget); }

			m_waitingLabel->movie()->start();
			m_waitingLabel->setGeometry(rect());
			m_waitingLabel->show();
		}
	}
	else {
		m_animationDelayRequested = false;
		m_delayTimer->stop();
		m_waitingLabel->movie()->stop();
		m_waitingLabel->setParent(nullptr);
		m_waitingLabel->hide();
	}
}

void ak::aAnimationOverlayWidget::setWaitingAnimation(
	QMovie *							_movie
) {
	assert(_movie != nullptr); // Nullptr provided
	m_waitingLabel->setMovie(_movie);
}

void ak::aAnimationOverlayWidget::slotTimeout(void) {
	if (!m_animationDelayRequested) { return; }
	m_animationDelayRequested = false;
	m_delayTimer->stop();

	if (m_waitingLabel->movie() == nullptr && !m_waitingAnimationVisible) {
		aAssert(0, "No movie set");
		return;
	}
	else if (m_waitingLabel->movie() == nullptr && m_waitingAnimationVisible) {
		aAssert(0, "Movie is not accessable anymore");
		return;
	}
	
	if (m_childWidget == nullptr) { m_waitingLabel->setParent(this); }
	else { m_waitingLabel->setParent(m_childWidget); }

	m_waitingLabel->movie()->start();
	m_waitingLabel->setGeometry(rect());
	m_waitingLabel->show();
}
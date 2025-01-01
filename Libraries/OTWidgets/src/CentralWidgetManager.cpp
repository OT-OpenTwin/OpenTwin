//! @file CentralWidgetManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CentralWidgetManager.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>

ot::CentralWidgetManager::CentralWidgetManager()
	: m_currentCentral(nullptr), m_showDelay(0), m_hideDelay(0)
{
	// Create animation label
	m_animationLabel = new QLabel("");
	m_animationLabel->setAttribute(Qt::WA_NoSystemBackground);
	m_animationLabel->setParent(this);
	m_animationLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
	m_animationLabel->hide();

	// Setup delay timer
	m_delayTimer = new QTimer;
	m_delayTimer->setSingleShot(true);
	connect(m_delayTimer, &QTimer::timeout, this, &CentralWidgetManager::slotTimeout);
}

ot::CentralWidgetManager::~CentralWidgetManager() {
	m_delayTimer->stop();
	if (m_animationLabel->movie()) {
		m_animationLabel->movie()->stop();
	}
	delete m_animationLabel;
	delete m_delayTimer;
}

QWidget* ot::CentralWidgetManager::replaceCurrentCentralWidget(QWidget* _widget) {
	QSize newSize = this->size();

	if (_widget == m_currentCentral) {
		return m_currentCentral;
	}
	QWidget* result = m_currentCentral;
	if (result) {
		if (m_animationLabel->parent() == result) {
			m_animationLabel->setParent(nullptr);
		}
		result->hide();
	}

	m_currentCentral = _widget;
	if (m_currentCentral != nullptr) {
		m_currentCentral->setParent(this);
		m_currentCentral->move(QPoint(0, 0));
		m_currentCentral->resize(newSize);
		m_currentCentral->show();

		m_animationLabel->setParent(nullptr);

		if (m_animationFlags & AnimationState::IsVisible) {
			this->applyWaitingAnimationVisible(true);
		}
	}

	return result;
}

QMovie* ot::CentralWidgetManager::replaceWaitingAnimation(QMovie* _animation) {
	QMovie* result = nullptr;

	if (m_animationLabel->movie()) {
		m_animationLabel->movie()->stop();
		result = m_animationLabel->movie();
	}
	
	m_animationLabel->setMovie(_animation);
	if (m_animationFlags & AnimationState::IsVisible) {
		this->applyWaitingAnimationVisible(true);
	}

	return result;
}

QMovie* ot::CentralWidgetManager::getWaitingAnimation(void) const {
	return m_animationLabel->movie();
}

void ot::CentralWidgetManager::setWaitingAnimationVisible(bool _visible, bool _noDelay) {
	m_delayTimer->stop();
	m_animationFlags &= ~AnimationState::DelayedShow;
	m_animationFlags &= ~AnimationState::DelayedHide;

	if (_visible) {	
		if (_noDelay || m_showDelay <= 0) {
			this->applyWaitingAnimationVisible(true);
		}
		else {
			m_delayTimer->setInterval(m_showDelay);
			m_animationFlags |= AnimationState::DelayedShow;
			m_delayTimer->start();
		}
	}
	else {
		if (_noDelay || m_showDelay <= 0) {
			this->applyWaitingAnimationVisible(false);
		}
		else {
			m_delayTimer->setInterval(m_showDelay);
			m_animationFlags |= AnimationState::DelayedHide;
			m_delayTimer->start();
		}
	}
}

void ot::CentralWidgetManager::resizeEvent(QResizeEvent* _event) {
	if (m_currentCentral != nullptr) {
		m_currentCentral->resize(this->size());
	}
	
	m_animationLabel->resize(this->size());
}

void ot::CentralWidgetManager::applyWaitingAnimationVisible(bool _visible) {
	if (_visible) {
		if (m_currentCentral) {
			m_animationLabel->setParent(m_currentCentral);
		}
		else {
			m_animationLabel->setParent(this);
		}

		m_animationLabel->movie()->start();
		m_animationLabel->setGeometry(this->rect());
		m_animationLabel->show();

		m_animationFlags |= AnimationState::IsVisible;
	}
	else {
		m_delayTimer->stop();
		
		if (m_animationLabel->movie()) {
			m_animationLabel->movie()->stop();
		}
		m_animationLabel->setParent(nullptr);
		m_animationLabel->hide();

		m_animationFlags &= ~AnimationState::IsVisible;
	}
}

void ot::CentralWidgetManager::slotTimeout(void) {
	if (m_animationFlags | AnimationState::DelayedShow) {
		m_animationFlags &= ~AnimationState::DelayedShow;
		this->applyWaitingAnimationVisible(true);
	}
	else if (m_animationFlags | AnimationState::DelayedHide) {
		m_animationFlags &= ~AnimationState::DelayedHide;
		this->applyWaitingAnimationVisible(false);
	}
}
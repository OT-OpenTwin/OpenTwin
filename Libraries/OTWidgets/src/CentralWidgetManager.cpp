//! @file CentralWidgetManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/CentralWidgetManager.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>

ot::CentralWidgetManager::CentralWidgetManager(MainWindow* _window)
	: m_currentCentral(nullptr), m_animationVisible(false)
{
	// Create animation label
	m_animationLabel = new QLabel("");
	m_animationLabel->setAttribute(Qt::WA_NoSystemBackground);
	m_animationLabel->setParent(this);
	m_animationLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
	m_animationLabel->hide();

	// Attach to window if set
	if (_window) {
		_window->setCentralWidget(this);
	}

	// Connect signals
	this->connect(&m_animationShowHideHandler, &DelayedShowHideHandler::showRequest, this, &CentralWidgetManager::slotShowAnimation);
	this->connect(&m_animationShowHideHandler, &DelayedShowHideHandler::hideRequest, this, &CentralWidgetManager::slotHideAnimation);
}

ot::CentralWidgetManager::~CentralWidgetManager() {
	// Stop animation handling and animation itself
	m_animationShowHideHandler.stop();
	if (m_animationLabel->movie()) {
		m_animationLabel->movie()->stop();
	}

	// Destroy animation label
	delete m_animationLabel;
}

std::unique_ptr<QWidget> ot::CentralWidgetManager::replaceCurrentCentralWidget(QWidget* _widget) {
	QSize newSize = this->size();

	if (_widget == m_currentCentral) {
		return std::unique_ptr<QWidget>(); // Nothing replaced
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


		if (m_animationVisible) {
			this->applyWaitingAnimationVisible(true);
		}
	}

	return std::unique_ptr<QWidget>(result);
}

void ot::CentralWidgetManager::setOverlayAnimation(std::shared_ptr<QMovie> _animation) {
	if (m_animationLabel->movie()) {
		m_animationLabel->movie()->stop();
	}

	m_animationLabel->setMovie(_animation.get());
	m_animation = _animation;

	if (m_animationVisible) {
		this->applyWaitingAnimationVisible(true);
	}
}

void ot::CentralWidgetManager::setOverlayAnimationVisible(bool _visible, bool _noDelay) {
	m_animationShowHideHandler.stop();
	if (_visible) {	
		if (_noDelay) {
			this->slotShowAnimation();
		}
		else {
			m_animationShowHideHandler.show();
		}
	}
	else {
		if (_noDelay) {
			this->slotHideAnimation();
		}
		else {
			m_animationShowHideHandler.hide();
		}
	}
}

void ot::CentralWidgetManager::setOverlayAnimatioShowDelay(int _delay) {
	m_animationShowHideHandler.setShowDelay(_delay);
}

int ot::CentralWidgetManager::getOverlayAnimatioShowDelay(void) const {
	return m_animationShowHideHandler.getShowDelay();
}

void ot::CentralWidgetManager::setOverlayAnimatioHideDelay(int _delay) {
	m_animationShowHideHandler.setHideDelay(_delay);
}

int ot::CentralWidgetManager::getOverlayAnimatioHideDelay(void) const {
	return m_animationShowHideHandler.getHideDelay();
}

void ot::CentralWidgetManager::resizeEvent(QResizeEvent* _event) {
	if (m_currentCentral != nullptr) {
		m_currentCentral->resize(this->size());
	}
	
	m_animationLabel->resize(this->size());
}

void ot::CentralWidgetManager::slotShowAnimation(void) {
	this->applyWaitingAnimationVisible(true);
}

void ot::CentralWidgetManager::slotHideAnimation(void) {
	this->applyWaitingAnimationVisible(false);
}

void ot::CentralWidgetManager::applyWaitingAnimationVisible(bool _visible) {
	m_animationShowHideHandler.stop();

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
		m_animationVisible = true;
	}
	else {		
		if (m_animationLabel->movie()) {
			m_animationLabel->movie()->stop();
		}
		m_animationLabel->setParent(nullptr);
		m_animationLabel->hide();
		m_animationVisible = false;
	}
}

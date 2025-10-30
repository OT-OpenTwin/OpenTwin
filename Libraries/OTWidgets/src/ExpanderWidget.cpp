// @otlicense
// File: ExpanderWidget.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/ExpanderWidget.h"

// Qt header
#include <QtCore/qpropertyanimation.h>
#include <QtCore/qparallelanimationgroup.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::ExpanderWidget::ExpanderWidget(const QString& _title, QWidget* _parent) 
    : QWidget(_parent), m_animationDuration(300), m_toggleAnimation(nullptr), m_expanded(false),
	m_currentWidget(nullptr), m_expandedHeight(0), m_dragStartHeight(0), m_resizing(false),
	c_titleHeight(25), c_handleHeight(5), m_minimumExpandedHeight(20)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

	// Toggle button
    m_toggleButton = new ot::ToolButton;
    m_toggleButton->setObjectName("OT_ExpanderWidgetToggle");
    m_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggleButton->setArrowType(Qt::ArrowType::RightArrow);
    m_toggleButton->setText(_title);
    m_toggleButton->setCheckable(false);
    m_toggleButton->setFixedHeight(25);

	// Header line
    m_headerLine = new QFrame;
	m_headerLine->setObjectName("OT_ExpanderWidgetHeaderLine");

	// Title
	QWidget* titleWidget = new QWidget;
	titleWidget->setObjectName("OT_ExpanderWidgetTitle");
    titleWidget->setFixedHeight(c_titleHeight);
    titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	titleWidget->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(titleWidget);

    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(m_toggleButton, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_headerLine, 1);

	// Child area
	m_childAreaWidget = new QWidget;
    m_childAreaWidget->setObjectName("OT_ExpanderWidgetChildArea");
	m_childAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_childAreaWidget->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_childAreaWidget);

	m_childArea = new QVBoxLayout(m_childAreaWidget);
	m_childArea->setContentsMargins(0, 0, 0, 0);

	// Resize handle
    m_handle = new QFrame;
    m_handle->setObjectName("OT_ExpanderWidgetResizeHandle");
	m_handle->setFixedHeight(c_handleHeight);
	m_handle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_handle->setCursor(Qt::SizeVerCursor);
    m_handle->setVisible(false);
	mainLayout->addWidget(m_handle);

    // Start out collapsed
	m_expanded = false;
    m_childAreaWidget->setMaximumHeight(0);
    m_childAreaWidget->setMinimumHeight(0);

    mainLayout->addStretch(1);

	// Animation
	m_toggleAnimation = new QParallelAnimationGroup(this);
	m_toggleAnimation->addAnimation(new QPropertyAnimation(m_childAreaWidget, "minimumHeight"));
	m_toggleAnimation->addAnimation(new QPropertyAnimation(m_childAreaWidget, "maximumHeight"));

    for (int i = 0; i < m_toggleAnimation->animationCount(); i++) {
        QPropertyAnimation* animation = static_cast<QPropertyAnimation*>(m_toggleAnimation->animationAt(i));
        animation->setDuration(m_animationDuration);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
	}

	// Finalize
	this->setContentsMargins(0, 0, 0, 0);
	this->setObjectName("OT_ExpanderWidget");
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->connect(m_toggleButton, &QToolButton::clicked, this, &ExpanderWidget::toggle);
	this->connect(m_toggleAnimation, &QPropertyAnimation::finished, this, &ExpanderWidget::animationFinished);
}

ot::ExpanderWidget::~ExpanderWidget() {
    m_toggleAnimation->stop();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ExpanderWidget::setWidget(QWidget* _widget) {
	// Remove old widget
    if (m_currentWidget) {
		m_childArea->removeWidget(m_currentWidget);
		m_currentWidget->setParent(nullptr);
        m_currentWidget->deleteLater();
	}

	m_currentWidget = _widget;
	
    if (!m_currentWidget) {
        return;
    }

    m_childArea->addWidget(m_currentWidget);
    m_currentWidget->setParent(m_childAreaWidget);

	m_minimumExpandedHeight = std::max(20, m_currentWidget->minimumHeight());
    m_expandedHeight = std::max(m_currentWidget->sizeHint().height(), m_minimumExpandedHeight);

	this->updateChild();
}

void ot::ExpanderWidget::setTitle(const QString& _title) {
    m_toggleButton->setText(_title);
}

QString ot::ExpanderWidget::getTitle() const {
    return m_toggleButton->text();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::ExpanderWidget::expand() {
    if (!m_currentWidget || m_expanded) {
        return;
    }
	m_expanded = true;

    m_toggleButton->setArrowType(Qt::ArrowType::DownArrow);

    if (m_toggleAnimation->state() == QAbstractAnimation::Running) {
        m_toggleAnimation->pause();
    }

    m_toggleAnimation->setDirection(QAbstractAnimation::Forward);

    m_toggleAnimation->start();
}

void ot::ExpanderWidget::collapse() {
    if (!m_currentWidget || !m_expanded) {
        return;
    }
    m_expanded = false;

    m_toggleButton->setArrowType(Qt::ArrowType::RightArrow);

    if (m_toggleAnimation->state() == QAbstractAnimation::Running) {
        m_toggleAnimation->pause();
    }

    m_toggleAnimation->setDirection(QAbstractAnimation::Backward);
    m_toggleAnimation->start();
}

void ot::ExpanderWidget::toggle() {
    if (m_expanded) {
        this->collapse();
    }
    else {
        this->expand();
    }
}

void ot::ExpanderWidget::animationFinished() {
	m_handle->setVisible(m_expanded);
}

void ot::ExpanderWidget::resizeEvent(QResizeEvent* _event) {
    QWidget::resizeEvent(_event);

    if (m_expanded && m_currentWidget) {
        // Update expanded height when widget is resized
        //m_expandedHeight = this->height() - (c_handleHeight + c_titleHeight);
        //updateChild();
    }
}

void ot::ExpanderWidget::mousePressEvent(QMouseEvent* _event) {
    if (isInResizeHandleArea(_event->pos())) {
        if (m_expanded) {
            m_resizing = true;
            m_dragStartPosition = _event->globalPosition().toPoint();
            m_dragStartHeight = m_childAreaWidget->height();
            setCursor(Qt::SizeVerCursor);
        }
        _event->accept();
        return;
    }

    QWidget::mousePressEvent(_event);
}

void ot::ExpanderWidget::mouseMoveEvent(QMouseEvent* _event) {
    if (m_resizing) {
        QPoint delta = _event->globalPosition().toPoint() - m_dragStartPosition;

        int newHeight = std::max(m_minimumExpandedHeight, m_dragStartHeight + delta.y());
		newHeight = std::min(newHeight, this->maximumHeight() - (c_handleHeight + c_titleHeight));

        if (m_expandedHeight != newHeight) {
            m_expandedHeight = newHeight;
            updateChild();
		}

        _event->accept();
        return;
    }

    QWidget::mouseMoveEvent(_event);
}

void ot::ExpanderWidget::mouseReleaseEvent(QMouseEvent* _event) {
    if (m_resizing) {
        m_resizing = false;
        unsetCursor();
        _event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(_event);
}


// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::ExpanderWidget::isInResizeHandleArea(const QPoint& _pos) const {
	return m_handle->geometry().contains(_pos);
}

void ot::ExpanderWidget::updateChild() {
    if (!m_currentWidget) {
        return;
    }

    for (int i = 0; i < m_toggleAnimation->animationCount(); i++) {
        QPropertyAnimation* animation = static_cast<QPropertyAnimation*>(m_toggleAnimation->animationAt(i));
        animation->setStartValue(0);
        animation->setEndValue(m_expandedHeight);
    }

    if (m_expanded) {
        m_childAreaWidget->setMinimumHeight(m_expandedHeight);
        m_childAreaWidget->setMaximumHeight(m_expandedHeight);
        m_childAreaWidget->resize(m_childAreaWidget->width(), m_expandedHeight);
    }
    else {
        m_childAreaWidget->setMinimumHeight(0);
        m_childAreaWidget->setMaximumHeight(0);
        m_childAreaWidget->resize(m_childAreaWidget->width(), 0);
    }
}

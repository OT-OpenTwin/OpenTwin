//! @file ExpanderWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/ExpanderWidget.h"

// Qt header
#include <QtCore/qpropertyanimation.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::ExpanderWidget::ExpanderWidget(const QString& _title, QWidget* _parent) :
	QWidget(_parent), m_animationDuration(300), m_toggleAnimation(nullptr), m_expanded(false),
    m_currentWidget(nullptr), m_lastHeightHint(0), m_minExpandedHeight(0)
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

	// Header line
    m_headerLine = new QFrame;
	m_headerLine->setObjectName("OT_ExpanderWidgetHeaderLine");

	// Title layout
    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(m_toggleButton, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_headerLine, 1);
    
    mainLayout->addLayout(titleLayout);

	// Child area
	m_childAreaWidget = new QWidget;
    m_childAreaWidget->setObjectName("OT_ExpanderWidgetChildArea");
	m_childAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_childAreaWidget->setContentsMargins(0, 0, 0, 0);

	m_childArea = new QVBoxLayout(m_childAreaWidget);
	m_childArea->setContentsMargins(0, 0, 0, 0);

    // Start out collapsed
	m_expanded = false;
    m_childAreaWidget->setMaximumHeight(0);
    m_childAreaWidget->setMinimumHeight(0);

    mainLayout->addWidget(m_childAreaWidget);
    mainLayout->addStretch(1);

	// Animation
	m_toggleAnimation = new QPropertyAnimation(m_childAreaWidget, "maximumHeight");
    
	// Finalize
	this->setObjectName("OT_ExpanderWidget");
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->connect(m_toggleButton, &QToolButton::clicked, this, &ExpanderWidget::toggle);
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
	this->updateAnimationLimits();
}

void ot::ExpanderWidget::setTitle(const QString& _title) {
    m_toggleButton->setText(_title);
}

QString ot::ExpanderWidget::getTitle() const {
    return m_toggleButton->text();
}

void ot::ExpanderWidget::setMinExpandedHeight(int _height) {
    m_minExpandedHeight = _height;
	this->updateAnimationLimits();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::ExpanderWidget::updateAnimationLimits() {
    if (!m_currentWidget) {
        return;
    }

    int heightHint = std::max(m_currentWidget->sizeHint().height(), m_minExpandedHeight);
    if (m_lastHeightHint == heightHint) {
        return;
    }
    m_lastHeightHint = heightHint;

    m_toggleAnimation->setDuration(m_animationDuration);
    m_toggleAnimation->setStartValue(0);
    m_toggleAnimation->setEndValue(m_lastHeightHint);

    if (m_expanded) {
        m_childAreaWidget->setMaximumHeight(m_lastHeightHint);
        m_childAreaWidget->resize(m_childAreaWidget->width(), m_lastHeightHint);
    }
}

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
//! \file OverlayWidgetBase.hpp
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/OverlayWidgetBase.h"
#include "OTCore/Logger.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtGui/qevent.h>

ot::OverlayWidgetBase::OverlayWidgetBase(QWidget* _parent, Alignment _overlayAlignment)
	: QFrame(_parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), m_parent(_parent)
{
	OTAssertNullptr(m_parent);

	this->setAttribute(Qt::WA_NoSystemBackground, true);
	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	m_parent->installEventFilter(this);
	this->updateOverlayGeometry();
}

ot::OverlayWidgetBase::~OverlayWidgetBase() {

}

bool ot::OverlayWidgetBase::eventFilter(QObject* _watched, QEvent* _event) {
    if (_watched == parent()) {
        switch (_event->type()) {
        case QEvent::Resize:
        case QEvent::Move:
        case QEvent::Show:
        case QEvent::Hide:
            updateGeometry();
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(_watched, _event);
}

void ot::OverlayWidgetBase::updateOverlayGeometry(void) {
    bool isVis = m_parent->isVisible();

    QRect parentRect = m_parent->geometry();
    if (this->minimumWidth() > parentRect.width() || this->minimumHeight() > parentRect.height()) {
        this->hide();
        return;
    }

    QSize newSize = this->minimumSize().expandedTo(parentRect.size()).boundedTo(this->maximumSize());

    this->setGeometry(ot::calculateChildRect(parentRect, newSize, m_alignment));
}
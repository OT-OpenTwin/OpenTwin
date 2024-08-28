//! \file OverlayWidgetBase.cpp
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/OverlayWidgetBase.h"
#include "OTCore/Logger.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtGui/qevent.h>

ot::OverlayWidgetBase::OverlayWidgetBase(QWidget* _parent, Alignment _overlayAlignment, const QMargins& _overlayMargins)
    : QFrame(nullptr, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), m_parent(_parent), 
    m_alignment(_overlayAlignment), m_margins(_overlayMargins), m_hidden(false)
{
    OTAssertNullptr(m_parent);

    //this->setAttribute(Qt::WA_NoSystemBackground, true);
    //this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    
    QObject* obj = m_parent;
    while (obj) {
        obj->installEventFilter(this);
        obj = obj->parent();
    }

    this->updateOverlayGeometry();
}

ot::OverlayWidgetBase::~OverlayWidgetBase() {

}

bool ot::OverlayWidgetBase::eventFilter(QObject* _watched, QEvent* _event) {
    switch (_event->type()) {
    case QEvent::Show:
        if (!m_hidden) this->show();
        this->updateOverlayGeometry();
        break;
    case QEvent::Hide:
        this->hide();
        break;
    case QEvent::WindowActivate:
        //this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        //this->updateOverlayGeometry();
        break;
    case QEvent::WindowDeactivate:
        //this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        //this->updateOverlayGeometry();
        break;
    case QEvent::Resize:
    case QEvent::Move:
        this->updateOverlayGeometry();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(_watched, _event);
}

void ot::OverlayWidgetBase::updateOverlayGeometry(void) {
    QRect rec = m_parent->geometry().marginsRemoved(m_margins);

    if (this->minimumWidth() > rec.width() || this->minimumHeight() > rec.height()) {
        this->hide();
        return;
    }
    else if (!this->isVisible() && m_parent->isVisible() && !m_hidden) {
        this->show();
    }
    rec.moveTo(m_parent->mapToGlobal(m_parent->pos()));

    QSize newSize = this->minimumSize().expandedTo(rec.size()).boundedTo(this->maximumSize());

    rec = ot::calculateChildRect(rec, newSize, m_alignment);

    this->setGeometry(rec);
}

// @otlicense
// File: OverlayWidgetBase.cpp
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
#include "OTWidgets/OverlayWidgetBase.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtGui/qevent.h>

ot::OverlayWidgetBase::OverlayWidgetBase(QWidget* _parent, Alignment _overlayAlignment, const QMargins& _overlayMargins)
    : QFrame(_parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), m_parent(_parent), 
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
    QRect rec = m_parent->geometry();

    if (this->minimumWidth() > rec.width() || this->minimumHeight() > rec.height()) {
        this->hide();
        return;
    }
    else if (!this->isVisible() && m_parent->isVisible() && !m_hidden) {
        this->show();
    }
    rec.moveTo(m_parent->mapToGlobal(m_parent->pos()));

    QSize newSize = this->minimumSize().expandedTo(rec.size()).boundedTo(this->maximumSize());
    
    this->setGeometry(ot::Positioning::calculateChildRect(rec, newSize + QSize(m_margins.left() + m_margins.right(), m_margins.top() + m_margins.bottom()), m_alignment).marginsRemoved(m_margins));    
}

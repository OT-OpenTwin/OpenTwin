//! @file GraphicsItemPreview.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItemPreview.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtGui/qevent.h>

ot::GraphicsItemPreview::GraphicsItemPreview() : m_drag(nullptr) {
	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}

ot::GraphicsItemPreview::~GraphicsItemPreview() {
	if (m_drag) delete m_drag;
}

void ot::GraphicsItemPreview::mousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		if (m_drag == nullptr) {
			m_drag = new GraphicsItemPreviewDrag(m_itemName, m_owner);
		}
		m_drag->queue(this);
	}
}

void ot::GraphicsItemPreview::enterEvent(QEnterEvent* _event) {
	this->setStyleSheet("border-width: 1px;"
		"border-style: inset;"
		"border-color: gray;");
}

void ot::GraphicsItemPreview::leaveEvent(QEvent* _event) {
	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}
//! @file GraphicsItemDrag.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsItemDrag.h"

// Qt header
#include <QtCore/qmimedata.h>
#include <QtGui/qdrag.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qstyleoption.h>

ot::GraphicsItemDrag::GraphicsItemDrag(GraphicsItem* _owner) : m_widget(nullptr), m_owner(_owner), m_queueCount(0) {}

ot::GraphicsItemDrag::~GraphicsItemDrag() {}

void ot::GraphicsItemDrag::queue(QWidget* _widget) {
	m_queueCount++;
	m_widget = _widget;
	QMetaObject::invokeMethod(this, &GraphicsItemDrag::slotQueue, Qt::QueuedConnection);
}

void ot::GraphicsItemDrag::slotQueue(void) {
	if (--m_queueCount == 0) {
		// Add configuration to mime data
		QDrag drag(m_widget);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText("OT_BLOCK");
		mimeData->setData(OT_GRAPHICSITEM_MIMETYPE_ItemName, QByteArray::fromStdString(m_owner->graphicsItemName()));
		
		// Create drag
		drag.setMimeData(mimeData);

		// Create preview
		QPixmap prev(m_owner->getQGraphicsItem()->boundingRect().size().toSize());
		QPainter p(&prev);
		QStyleOptionGraphicsItem opt;
		p.fillRect(QRect(QPoint(0, 0), m_owner->getQGraphicsItem()->boundingRect().size().toSize()), Qt::gray);

		// Paint
		m_owner->callPaint(&p, &opt, m_widget);

		// Run drag
		drag.setPixmap(prev);
		drag.exec();
	}
}

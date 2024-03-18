//! @file GraphicsItemPreviewDrag.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtCore/qmimedata.h>
#include <QtGui/qdrag.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qstyleoption.h>

ot::GraphicsItemPreviewDrag::GraphicsItemPreviewDrag(const std::string& _itemName) : m_widget(nullptr), m_itemName(_itemName), m_queueCount(0) {}

ot::GraphicsItemPreviewDrag::~GraphicsItemPreviewDrag() {}

void ot::GraphicsItemPreviewDrag::queue(QWidget* _widget) {
	m_queueCount++;
	m_widget = _widget;
	QMetaObject::invokeMethod(this, &GraphicsItemPreviewDrag::slotQueue, Qt::QueuedConnection);
}

void ot::GraphicsItemPreviewDrag::slotQueue(void) {
	if (--m_queueCount == 0) {
		// Add configuration to mime data
		QDrag drag(m_widget);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText("OT_BLOCK");
		mimeData->setData(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName, QByteArray::fromStdString(m_itemName));
		
		// Create drag
		drag.setMimeData(mimeData);

		// Run drag
		drag.exec();
	}
}

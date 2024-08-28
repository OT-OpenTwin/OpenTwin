//! @file GraphicsItemPreviewDrag.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtCore/qmimedata.h>
#include <QtGui/qdrag.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qstyleoption.h>

ot::GraphicsItemPreviewDrag::GraphicsItemPreviewDrag(const std::string& _itemName, const BasicServiceInformation& _owner)
	: m_widget(nullptr), m_itemName(_itemName), m_queueCount(0), m_owner(_owner)
{}

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
		
		JsonDocument ownerDoc;
		m_owner.addToJsonObject(ownerDoc, ownerDoc.GetAllocator());
		mimeData->setData(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner, QByteArray::fromStdString(ownerDoc.toJson()));

		// Create drag
		drag.setMimeData(mimeData);

		// Run drag
		drag.exec();
	}
}

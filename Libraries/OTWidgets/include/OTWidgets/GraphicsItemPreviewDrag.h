//! @file GraphicsItemPreviewDrag.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

#define OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName "GraphicsItem.Name"

class QWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsItemPreviewDrag : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsItemPreviewDrag)
	public:
		GraphicsItemPreviewDrag(const std::string& _itemName);
		virtual ~GraphicsItemPreviewDrag();

		void queue(QWidget* _widget);

	private Q_SLOTS:
		void slotQueue(void);

	private:
		QWidget* m_widget;
		int m_queueCount;
		std::string m_itemName;
	};

}

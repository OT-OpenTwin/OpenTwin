//! @file GraphicsItemDrag.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsItemDrag : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsItemDrag)
	public:
		GraphicsItemDrag(GraphicsItem* _owner);
		virtual ~GraphicsItemDrag();

		void queue(QWidget* _widget);

	private slots:
		void slotQueue(void);

	private:
		QWidget* m_widget;
		GraphicsItem* m_owner;
		int m_queueCount;
	};

}
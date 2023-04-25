#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>

// Qt header
#include <QtCore/qpoint.h>

class QPainter;

namespace ot {
	class BLOCK_EDITOR_API_EXPORT BlockConnector {
	public:
		BlockConnector() {};
		virtual ~BlockConnector() {};

		virtual void paint(QPainter* _painter, qreal _xLimit, qreal _y) = 0;
		virtual QPointF connectionPoint(void) const = 0;
		virtual bool canConnect(void) const { return true; };
	};
}
//! @file Block.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtWidgets/qgraphicsitem.h>

namespace ot {

	class BlockConnector;

	class BLOCK_EDITOR_API_EXPORT Block : public QObject, public QGraphicsItem {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)

	public:
		Block();
		virtual ~Block();

		virtual QRectF boundingRect(void) const override;

		virtual qreal blockWidth(void) const = 0;
		virtual qreal blockHeigth(void) const = 0;

		virtual BlockConnector* connectorAt(const QPointF& _point) const = 0;

	private:

	};

}

Q_DECLARE_METATYPE(ot::Block *);
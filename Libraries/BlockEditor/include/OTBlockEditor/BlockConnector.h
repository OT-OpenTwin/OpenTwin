//! @file BlockConnector.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <QtWidgets/qgraphicsitem.h>

class QPainter;

namespace ot {
	class BLOCK_EDITOR_API_EXPORT BlockConnector : public QObject, public QGraphicsItem, public ot::BlockGraphicsObject {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		BlockConnector();
		virtual ~BlockConnector();

		virtual QRectF boundingRect(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;
	};
}
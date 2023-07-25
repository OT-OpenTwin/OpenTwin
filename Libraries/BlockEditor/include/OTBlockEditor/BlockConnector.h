//! @file BlockConnector.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OTGui/GuiTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>
#include <QtWidgets/qgraphicsitem.h>

class QPainter;

namespace ot {
	class BLOCK_EDITOR_API_EXPORT BlockConnector : public QObject, public QGraphicsItem, public ot::BlockGraphicsObject, public ot::BlockPaintJob {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		BlockConnector();
		virtual ~BlockConnector();

		virtual QRectF boundingRect(void) const override;

		//! @brief Empty function.
		//! This function is required by Qt.
		//! OpenTwin paints the connectors when the layers are painted manually
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override {};

		virtual QueueResultFlags runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) override;

		//! @brief Will calculate and return the size of the block graphics object
		virtual QSizeF calculateSize(void) const override;

		void setTitle(const QString& _title) { m_title = _title; };
		const QString& title(void) const { return m_title; };

		void setTitleOrientation(ot::Orientation _orientation) { m_titleOrientation = _orientation; };
		ot::Orientation titleOrientation(void) const { return m_titleOrientation; };

		void setConnectorStyle(ot::BlockConnectorStyle _style) { m_style = _style; };
		ot::BlockConnectorStyle connectorStyle(void) const { return m_style; };

		void setFillColor(const QColor& _color) { m_fillColor = _color; };
		const QColor& fillColor(void) const { return m_fillColor; };

		void setBorderColor(const QColor& _color) { m_borderColor = _color; };
		const QColor& borderColor(void) const { return m_borderColor; };

		void setTitleFont(const QFont& _font) { m_titleFont = _font; };
		const QFont& titleFont(void) const { return m_titleFont; };

	private:
		QFont m_titleFont;
		int m_connectorSize;
		QString m_title;
		QMarginsF m_titleMargins;
		ot::Orientation m_titleOrientation;
		ot::BlockConnectorStyle m_style;
		QColor m_fillColor;
		QColor m_borderColor;
	};
}
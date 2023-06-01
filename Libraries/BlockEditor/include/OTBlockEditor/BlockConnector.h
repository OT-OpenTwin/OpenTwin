//! @file BlockConnector.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"
#include "OTGui/GuiTypes.h"

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

	private:
		QString m_title;
		ot::Orientation m_titleOrientation;
		ot::BlockConnectorStyle m_style;
		QColor m_fillColor;
		QColor m_borderColor;
	};
}
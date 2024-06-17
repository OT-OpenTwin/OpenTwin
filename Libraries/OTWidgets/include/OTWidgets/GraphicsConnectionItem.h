//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtGui/qpen.h>

namespace ot {
	
	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsConnectionItem)
	public:
		enum GraphicsItemState {
			NoState = 0x00, //! @brief Default state
			HoverState = 0x01, //! @brief Item is hovered over by user
			SelectedState = 0x02  //! @brief Item is selected
		};
		typedef Flags<GraphicsItemState> GraphicsItemStateFlags;

		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Will set the connection properties as provided in the configuration
		//! The origin and destination item won't be set!
		bool setConfiguration(const ot::GraphicsConnectionCfg& _cfg);

		//! @brief Creates a configuration object containing the items origin and destination information
		const GraphicsConnectionCfg& getConfiguration(void) const { return m_config; };

		void updateConnection(void);

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		GraphicsItem* originItem(void) const { return m_origin; };
		GraphicsItem* destItem(void) const { return m_dest; };

	private:
		void updateConnectionInformation(void);

		void calculateDirectLinePoints(QPointF& _origin, QPointF& _destination) const;
		void calculateSmoothLinePoints(QPointF& _origin, QPointF& _control1, QPointF& _control2, QPointF& _destination) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;

		GraphicsConnectionCfg m_config;

		GraphicsItemStateFlags m_state;

		GraphicsItem* m_origin;
		GraphicsItem* m_dest;

		QRectF m_lastRect;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsConnectionItem::GraphicsItemState)
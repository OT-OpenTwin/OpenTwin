//! @file GraphicsGroupItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsGroupItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsGroupItem();
		virtual ~GraphicsGroupItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual const QGraphicsItem* getQGraphicsItem(void) const override { return this; };

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

		virtual void finalizeGraphicsItem(void) override;

		virtual std::list<GraphicsElement*> getAllGraphicsElements(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsItem

		virtual QRectF boundingRect(void) const override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsLayoutItem

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addItem(GraphicsItem* _item);

		//! \brief Returns the maximum trigger distance in any direction of this item and its childs.
		virtual double getMaxAdditionalTriggerDistance(void) const override;

		void clear(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;

		virtual void notifyChildsAboutTransformChange(const QTransform& _newTransform) override;

	private:
		GraphicsGroupItem(const GraphicsGroupItem&) = delete;
		GraphicsGroupItem& operator = (const GraphicsGroupItem&) = delete;
	};

}
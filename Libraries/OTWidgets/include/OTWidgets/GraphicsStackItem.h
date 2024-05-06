//! @file GraphicsStackItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsGroupItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public ot::GraphicsGroupItem {
	public:
		struct GraphicsStackItemEntry {
			ot::GraphicsItem* item;
			bool isMaster;
			bool isSlave;
		};

		GraphicsStackItem();
		virtual ~GraphicsStackItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void removeAllConnections(void) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlags _flags) override;

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual QRectF boundingRect(void) const override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		void addItem(ot::GraphicsItem* _item, bool _isMaster, bool _isSlave);

	private:
		void memClear(void);
		void adjustChildItems(void);

		std::list<GraphicsStackItemEntry> m_items;
		QSizeF m_lastCalculatedSize;

		GraphicsStackItem(const GraphicsStackItem&) = delete;
		GraphicsStackItem& operator = (const GraphicsStackItem&) = delete;
	};


}
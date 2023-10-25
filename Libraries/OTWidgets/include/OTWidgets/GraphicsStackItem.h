//! @file GraphicsStackItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsGroupItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItem "OT_GIStack"

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

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem); };

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) override;

	private:
		void memClear(void);
		void adjustChildItems(void);

		std::list<GraphicsStackItemEntry> m_items;
		bool m_isFirstPaint; //! @brief If true, the first paint event will resize calcule the child adjust

		GraphicsStackItem(const GraphicsStackItem&) = delete;
		GraphicsStackItem& operator = (const GraphicsStackItem&) = delete;
	};


}
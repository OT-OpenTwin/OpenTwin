//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/GraphicsItem.h"

namespace ot {

	class GraphicsLayoutItemWrapper;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItem : public ot::GraphicsItem {
	public:
		GraphicsLayoutItem();
		virtual ~GraphicsLayoutItem();
		
		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void setGraphicsItemName(const std::string& _name) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void setParentGraphicsItem(GraphicsItem* _itm) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const = 0;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override;
		virtual QGraphicsItem* getQGraphicsItem(void) override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

	protected:
		//! @brief Call this function from the item constructor to create the layout wrapper instance
		void createLayoutWrapper(QGraphicsLayout* _layout);

	private:
		GraphicsLayoutItemWrapper* m_layoutWrap;
	};

}
//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/GraphicsItem.h"

class QGraphicsLayout;

namespace ot {

	class GraphicsLayoutItemWrapper;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItem : public ot::GraphicsItem {
		OT_DECL_NODEFAULT(GraphicsLayoutItem)
	public:
		GraphicsLayoutItem(GraphicsItemCfg* _configuration);
		virtual ~GraphicsLayoutItem();
		
		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void setGraphicsItemName(const std::string& _name) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void setParentGraphicsItem(GraphicsItem* _itm) override;

		virtual void setGraphicsScene(GraphicsScene* _scene) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const = 0;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) override;

		virtual void graphicsItemConfigurationChanged(const GraphicsItemCfg* _config) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override;
		virtual QGraphicsItem* getQGraphicsItem(void) override;
		virtual const QGraphicsItem* getQGraphicsItem(void) const override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

		virtual void finalizeGraphicsItem(void) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		//! \brief Returns the maximum trigger distance in any direction of this item and its childs.
		virtual double getMaxAdditionalTriggerDistance(void) const override;

		virtual std::list<GraphicsElement*> getAllGraphicsElements(void) override;

		GraphicsLayoutItemWrapper* getLayoutWrapper(void) const { return m_layoutWrap; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;

		virtual void notifyChildsAboutTransformChange(const QTransform& _newTransform) override;

		//! @brief Call this function from the item constructor to create the layout wrapper instance
		void createLayoutWrapper(QGraphicsLayout* _layout);

	private:
		GraphicsLayoutItemWrapper* m_layoutWrap;
	};

}
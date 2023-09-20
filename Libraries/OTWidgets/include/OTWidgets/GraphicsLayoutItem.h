//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicslinearlayout.h>
#include <QtWidgets/qgraphicsgridlayout.h>
#include <QtWidgets/qgraphicswidget.h>

class QGraphicsWidget;

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem "OT_GILayV"
#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem "OT_GILayH"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem "OT_GILayG"
#define OT_SimpleFactoryJsonKeyValue_GraphicsLayoutWrapperItem "OT_GILayWrap"

namespace ot {

	class GraphicsLayoutItem;
	class GraphicsGroupItem;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItemWrapper : public QGraphicsWidget, public ot::GraphicsItem {
	public:
		GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner);
		virtual ~GraphicsLayoutItemWrapper();

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return m_group; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return m_group; };

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsLayoutWrapperItem); };

		virtual void setParentGraphicsItem(GraphicsItem* _itm) override;

		GraphicsGroupItem* getGroupItem(void) { return m_group; };

	private:
		GraphicsLayoutItem* m_owner;
		GraphicsGroupItem* m_group;

		GraphicsLayoutItemWrapper() = delete;
		GraphicsLayoutItemWrapper(const GraphicsLayoutItemWrapper&) = delete;
		GraphicsLayoutItemWrapper& operator = (const GraphicsLayoutItemWrapper&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItem : public ot::GraphicsItem {
	public:
		GraphicsLayoutItem();
		virtual ~GraphicsLayoutItem();
		
		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void setParentGraphicsItem(GraphicsItem* _itm) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const = 0;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return m_layoutWrap->getQGraphicsLayoutItem(); };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return m_layoutWrap->getQGraphicsItem(); };

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

	protected:
		//! @brief Call this function from the item constructor to create the layout wrapper instance
		void createLayoutWrapperAndGroup(QGraphicsLayout* _layout);

		void addChildToGroup(ot::GraphicsItem* _item);

	private:
		GraphicsLayoutItemWrapper* m_layoutWrap;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayoutItem : public QGraphicsLinearLayout, public ot::GraphicsLayoutItem {
	public:
		GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayoutItem() {};

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

	private:
		GraphicsBoxLayoutItem() = delete;
		GraphicsBoxLayoutItem(const GraphicsBoxLayoutItem&) = delete;
		GraphicsBoxLayoutItem& operator = (const GraphicsBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsVBoxLayoutItem(const GraphicsVBoxLayoutItem&) = delete;
		GraphicsVBoxLayoutItem& operator = (const GraphicsVBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsHBoxLayoutItem(const GraphicsHBoxLayoutItem&) = delete;
		GraphicsHBoxLayoutItem& operator = (const GraphicsHBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGridLayoutItem : public QGraphicsGridLayout, public ot::GraphicsLayoutItem {
	public:
		GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

	private:
		QSizeF m_size;

		GraphicsGridLayoutItem(const GraphicsGridLayoutItem&) = delete;
		GraphicsGridLayoutItem& operator = (const GraphicsGridLayoutItem&) = delete;
	};

}
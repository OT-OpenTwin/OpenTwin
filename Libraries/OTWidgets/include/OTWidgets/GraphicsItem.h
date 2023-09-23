//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTGui/GuiTypes.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Flags.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

// std header
#include <list>

#define OT_GRAPHICSITEM_MIMETYPE_ItemName "GraphicsItem.Name"

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItem "OT_GIText"
#define OT_SimpleFactoryJsonKeyValue_GraphicsLineItem "OT_GILine"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGroupItem "OT_GIGroup"
#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItem "OT_GIStack"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItem "OT_GIImage"
#define OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem "OT_GIElli"
#define OT_SimpleFactoryJsonKeyValue_GraphicsConnectionItem "OT_GIConnection"
#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class GraphicsItem;
	class GraphicsConnectionItem;
	class GraphicsItemCfg;
	class GraphicsScene;
	class GraphicsGroupItem;

	class OT_WIDGETS_API_EXPORT GraphicsItemDrag : public QObject {
		Q_OBJECT
	public:
		GraphicsItemDrag(GraphicsItem* _owner);
		virtual ~GraphicsItemDrag();

		void queue(QWidget* _widget);

	private slots:
		void slotQueue(void);

	private:
		QWidget* m_widget;
		GraphicsItem* m_owner;
		int m_queueCount;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemEvent {
			ItemMoved,
			ItemResized
		};

		enum GraphicsItemFlag {
			NoFlags = 0x00, //! @brief No graphics item flags
			ItemIsConnectable = 0x01, //! @brief Item can be used as source or destination of a conncetion
			ItemIsMoveable = 0x02, //! @brief The item can be moved by a user
			ItemPreviewContext = 0x10, //! @brief Item is placed in a preview (preview box)
			ItemNetworkContext = 0x20  //! @brief Item is placed in a network (editor)
		};

		GraphicsItem();
		virtual ~GraphicsItem();

		// ###############################################################################################################################################

		// Pure virtual functions

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) = 0;

		virtual QGraphicsItem* getQGraphicsItem(void) = 0;

		// ###############################################################################################################################################

		// Virtual functions

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg);

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {};

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {};

		virtual ot::GraphicsItem* findItem(const std::string& _itemName);

		// ###############################################################################################################################################

		// Event handler

		void handleMousePressEvent(QGraphicsSceneMouseEvent* _event);
		void handleMouseReleaseEvent(QGraphicsSceneMouseEvent* _event);
		void handleItemMoved(void);
		void paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget);

		// ###############################################################################################################################################

		// Getter / Setter

		void setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags);
		ot::GraphicsItem::GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };
		GraphicsScene* graphicsScene(void);

		void setHasHover(bool _hasHover) { m_hasHover = _hasHover; };
		bool hasHover(void) const { return m_hasHover; };		

		void setGraphicsItemUid(const std::string& _uid) { m_uid = _uid; };
		const std::string& graphicsItemUid(void) const { return m_uid; };

		void setGraphicsItemName(const std::string& _name) { m_name = _name; };
		const std::string& graphicsItemName(void) const { return m_name; };

		virtual void setParentGraphicsItem(GraphicsItem* _itm) { m_parent = _itm; };
		GraphicsItem* parentGraphicsItem(void) const { return m_parent; };
		GraphicsItem* getRootItem(void);

		void storeConnection(GraphicsConnectionItem* _connection);

		//! @brief Removes the collection from the list (item will not be destroyed)
		void forgetConnection(GraphicsConnectionItem* _connection);

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) { m_requestedSize = _size; };
		const QSizeF& graphicsItemRequestedSize(void) const { return m_requestedSize; };

		void setGraphicsItemAlignment(ot::Alignment _align) { m_alignment = _align; };
		ot::Alignment graphicsItemAlignment(void) const { return m_alignment; };

		void addGraphicsItemEventHandler(ot::GraphicsItem* _handler);
		void removeGraphicsItemEventHandler(ot::GraphicsItem* _handler);

		void raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event);

		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		QRectF calculateDrawRect(const QRectF& _rect) const;

	private:
		bool m_hasHover;
		std::string m_uid;
		std::string m_name;
		ot::Alignment m_alignment;
		GraphicsItemFlag m_flags;

		QPointF m_moveStartPt;
		GraphicsItem* m_parent;
		GraphicsItemDrag* m_drag;
		GraphicsScene* m_scene;

		QSizeF m_requestedSize;

		std::list<GraphicsItem*> m_eventHandler;
		std::list<GraphicsConnectionItem*> m_connections;

		GraphicsItem(const GraphicsItem&) = delete;
		GraphicsItem& operator = (const GraphicsItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGroupItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsGroupItem();
		virtual ~GraphicsGroupItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGroupItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

	private:
		GraphicsGroupItem(const GraphicsGroupItem&) = delete;
		GraphicsGroupItem& operator = (const GraphicsGroupItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public ot::GraphicsGroupItem {
	public:
		struct GraphicsStackItemEntry {
			ot::GraphicsItem* item;
			bool isMaster;
		};

		GraphicsStackItem();
		virtual ~GraphicsStackItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem); };

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) override;

	private:
		void memClear(void);

		std::list<GraphicsStackItemEntry> m_items;

		GraphicsStackItem(const GraphicsStackItem&) = delete;
		GraphicsStackItem& operator = (const GraphicsStackItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public QGraphicsItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual QRectF boundingRect(void) const override;
		virtual void setGeometry(const QRectF& rect) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		void setRectangleSize(const QSizeF& _size);
		const QSizeF& rectangleSize(void) const { return m_size; };

		void setRectangleBrush(const QBrush& _brush) { m_brush = _brush; };
		void setRectanglePen(const QPen& _pen) { m_pen = _pen; };
		void setCornerRadius(int _r) { m_cornerRadius = _r; };

	private:
		QSizeF m_size;
		QBrush m_brush;
		QPen m_pen;
		int m_cornerRadius;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsEllipseItem : public QGraphicsItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsEllipseItem();
		virtual ~GraphicsEllipseItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual QRectF boundingRect(void) const override;
		virtual void setGeometry(const QRectF& rect) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		void setRadius(int _x, int _y);
		int radiusX(void) const { return m_radiusX; };
		int radiusY(void) const { return m_radiusY; };

	private:
		QBrush m_brush;
		QPen m_pen;
		int m_radiusX;
		int m_radiusY;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsTextItem : public QGraphicsTextItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsTextItem();
		virtual ~GraphicsTextItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void setGeometry(const QRectF& rect) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	};


	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsImageItem : public QGraphicsPixmapItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsImageItem();
		virtual ~GraphicsImageItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsImageItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsLineItem : public QGraphicsLineItem, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsLineItem();
		virtual ~GraphicsLineItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsLineItem); };

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return nullptr; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public GraphicsLineItem {
	public:
		static std::string buildKey(const std::string& _originUid, const std::string& _originItemName, const std::string& _destUid, const std::string& _destItemName);

		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsConnectionItem); };

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		void updateConnection(void);

		GraphicsItem* originItem(void) { return m_origin; };
		GraphicsItem* destItem(void) { return m_dest; };

	private:
		GraphicsItem* m_origin;
		GraphicsItem* m_dest;
	};
	
}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemFlag);
//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Flags.h"

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

// std header
#include <list>

#define OT_GRAPHICSITEM_MIMETYPE_Configuration "GraphicsItem.Configuration"

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItem "OT_GIText"
#define OT_SimpleFactoryJsonKeyValue_GraphicsPathItem "OT_GIPath"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGroupItem "OT_GIGroup"
#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItem "OT_GIStack"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItem "OT_GIImage"
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

		void queue(QWidget* _widget, const QRectF& _rect);

	private slots:
		void slotQueue(void);

	private:
		QWidget* m_widget;
		QRectF m_rect;
		GraphicsItem* m_owner;
		int m_queueCount;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags            = 0x00, //! @brief No graphics item flags
			ItemIsConnectable  = 0x01, //! @brief Item can be used as source or destination of a conncetion
			ItemIsMoveable     = 0x02, //! @brief The item can be moved by a user
			ItemPreviewContext = 0x10, //! @brief Item is placed in a preview (preview box)
			ItemNetworkContext = 0x20  //! @brief Item is placed in a network (editor)
		};

		//! @param _containerItem If true the item will work as a container and create a group for grouping child items
		GraphicsItem(bool _containerItem = false);
		virtual ~GraphicsItem();

		// ###############################################################################################################################################

		// Virtual functions

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg);

		void finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group);

		// ###############################################################################################################################################

		// Pure virtual functions

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		virtual QRectF getGraphicsItemBoundingRect(void) const = 0;

		virtual QPointF getGraphicsItemCenter(void) const = 0;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {};

		// ###############################################################################################################################################

		// Generalized functionallity

		void handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect);

		void paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget);

		// ###############################################################################################################################################

		// Getter / Setter

		void setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags);
		ot::GraphicsItem::GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setConfiguration(const std::string& _jsonDocument) { m_configuration = _jsonDocument; };
		const std::string& configuration(void) const { return m_configuration; };

		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };
		GraphicsScene* graphicsScene(void) { return m_scene; }

		void setHasHover(bool _hasHover) { m_hasHover = _hasHover; };
		bool hasHover(void) const { return m_hasHover; };		

		void setGraphicsItemUid(ot::UID _uid) { m_uid = _uid; };
		ot::UID graphicsItemUid(void) const { return m_uid; };

		void setGraphicsItemName(const std::string& _name) { m_name = _name; };
		const std::string& graphicsItemName(void) const { return m_name; };

		void setParentGraphicsItem(GraphicsItem* _itm) { m_parent = _itm; };
		GraphicsItem* parentGraphicsItem(void) const { return m_parent; };
		GraphicsItem* getRootItem(void);

		void setGraphicsItemPosition(const QPointF& _pos);
		const QPointF& graphicsItemPosition(void) const { return m_pos; };

		bool isContainerItem(void) const { return m_isContainerItem; };

		GraphicsGroupItem* getItemGroup(void) const { return m_group; };

		void storeConnection(GraphicsConnectionItem* _connection);
		void forgetConnection(GraphicsConnectionItem* _connection);

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) = 0;

	private:
		std::string m_configuration;
		std::string m_name;
		GraphicsItemFlag m_flags;
		GraphicsItem* m_parent;
		GraphicsGroupItem* m_group;
		bool m_isContainerItem;
		bool m_hasHover;
		GraphicsItemDrag* m_drag;
		GraphicsScene* m_scene;
		ot::UID m_uid;
		QPointF m_pos;
		std::list<GraphicsConnectionItem*> m_connections;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGroupItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsGroupItem(bool _containerItem = false);
		virtual ~GraphicsGroupItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGroupItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

		virtual QPointF getGraphicsItemCenter(void) const override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public ot::GraphicsGroupItem {
	public:
		GraphicsStackItem();
		virtual ~GraphicsStackItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem); };

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;

	private:
		ot::GraphicsItem* m_top;
		ot::GraphicsItem* m_bottom;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public QGraphicsRectItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override { return m_size; };

		virtual void setGeometry(const QRectF& rect) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

		virtual QPointF getGraphicsItemCenter(void) const override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;

	private:
		QSizeF m_size;

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

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override { return m_size; };

		virtual void setGeometry(const QRectF& rect) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

		virtual QPointF getGraphicsItemCenter(void) const override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;

	private:
		QSizeF m_size;

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

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override { return m_size; };

		virtual void setGeometry(const QRectF& rect) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

		virtual QPointF getGraphicsItemCenter(void) const override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;

	private:
		QSizeF m_size;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsPathItem : public QGraphicsPathItem, public ot::GraphicsItem {
	public:
		GraphicsPathItem();
		virtual ~GraphicsPathItem();

		void setPathPoints(const QPointF& _origin, const QPointF& _dest);

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsPathItem); };

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

		virtual QPointF getGraphicsItemCenter(void) const override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	protected:
		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItemContents(GraphicsScene* _scene, GraphicsGroupItem* _group) override;

	private:
		QPointF m_origin;
		QPointF m_dest;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public GraphicsPathItem {
	public:
		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsConnectionItem); };

		void connect(GraphicsItem* _origin, GraphicsItem* _dest);

		void updateConnection(void);

		GraphicsItem* originItem(void) { return m_origin; };
		GraphicsItem* destItem(void) { return m_dest; };

	private:
		GraphicsItem* m_origin;
		GraphicsItem* m_dest;
	};
	

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemFlag);
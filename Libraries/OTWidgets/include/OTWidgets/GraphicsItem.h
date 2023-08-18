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

#define OT_GRAPHICSITEM_MIMETYPE_Configuration "GraphicsItem.Configuration"

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItem "OT_GIText"
#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItem "OT_GIStack"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItem "OT_GIImage"
#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class GraphicsItemCfg;

	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags            = 0x00, //! @brief No graphics item flags
			ItemIsConnectable  = 0x01, //! @brief Item can be used as source or destination of a conncetion
			ItemIsMoveable     = 0x02, //! @brief The item can be moved by a user
			ItemPreviewContext = 0x10, //! @brief Item is placed in a preview (preview box)
			ItemNetworkContext = 0x20  //! @brief Item is placed in a network (editor)
		};

		GraphicsItem(bool _isLayout = false);
		virtual ~GraphicsItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg);

		void setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags);
		ot::GraphicsItem::GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setConfiguration(const std::string& _jsonDocument) { m_configuration = _jsonDocument; };
		const std::string& configuration(void) const { return m_configuration; };

		virtual void finalizeAsRootItem(QGraphicsScene * _scene);

		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) = 0;

		void handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect);

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		void setHasHover(bool _hasHover) { m_hasHover = _hasHover; };
		bool hasHover(void) const { return m_hasHover; };

		void paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget);

		virtual QRectF getGraphicsItemBoundingRect(void) const = 0;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {};

	private:
		std::string m_configuration;
		GraphicsItemFlag m_flags;
		QGraphicsItemGroup* m_group;
		bool m_isLayout;
		bool m_hasHover;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsStackItem();
		virtual ~GraphicsStackItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem); };

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

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

		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

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

		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

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

		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF getGraphicsItemBoundingRect(void) const override;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	private:
		QSizeF m_size;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemFlag);
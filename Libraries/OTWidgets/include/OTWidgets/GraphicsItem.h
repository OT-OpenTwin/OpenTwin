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

#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"
#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItem "OT_GIText"
#define OT_GRAPHICSITEM_MIMETYPE_Configuration "GraphicsItem.Configuration"

namespace ot {

	class GraphicsItemCfg;

	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags        = 0x00, //! @brief No graphics item flags
			ItemIsMoveable = 0x01, //! @brief The item can be moved by a user
			ItemPreviewContext = 0x10, //! @brief Item is placed in a preview (preview box)
			ItemNetworkContext = 0x20 //! @brief Item is placed in a network (editor)
		};

		GraphicsItem(bool _isLayout = false);
		virtual ~GraphicsItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) = 0;

		void setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags);
		ot::GraphicsItem::GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setConfiguration(const std::string& _jsonDocument) { m_configuration = _jsonDocument; };
		const std::string& configuration(void) const { return m_configuration; };

		virtual void finalizeAsRootItem(QGraphicsScene * _scene);

		//! @brief Finish setting up the item and add it to the scene (and all childs)
		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) = 0;

		void handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect);

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {};
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

	private:
		std::string m_configuration;
		GraphicsItemFlag m_flags;
		QGraphicsItemGroup* m_group;
		bool m_isLayout;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//class OT_WIDGETS_API_EXPORT GraphicsItemPair : public ot::GraphicsItem {
	//public:
	//	GraphicsItemPair();
	//	virtual ~GraphicsItemPair();

	//	virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	//	//! @brief Returns the key that is used to create an instance of this class in the simple factory
	//	virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem); };

	//	virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override { return m_size; };

	//	virtual void setGeometry(const QRectF& rect) override;

	//	virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) override;

	//private:

	//};

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

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

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

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

	protected:
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

	private:
		QSizeF m_size;

	};

	

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemFlag);
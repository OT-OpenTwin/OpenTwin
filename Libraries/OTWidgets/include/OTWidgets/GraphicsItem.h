//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OpenTwinCore/SimpleFactory.h"

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"
#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItem "OT_GIText"

namespace ot {

	class GraphicsItemCfg;

	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		GraphicsItem();
		virtual ~GraphicsItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) = 0;

	private:

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

		virtual void setGeometry(const QRectF& rect) {
			setPos(rect.topLeft());
		}

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

		virtual void setGeometry(const QRectF& rect) {
			setPos(rect.topLeft());
		}

	private:
		QSizeF m_size;

	};

	

}

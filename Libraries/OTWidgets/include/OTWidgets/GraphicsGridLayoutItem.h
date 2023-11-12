//! @file GraphicsGridLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"

// Qt header
#include <QtWidgets/qgraphicsgridlayout.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem "OT_GILayG"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsGridLayoutItem : public QGraphicsGridLayout, public ot::GraphicsLayoutItem {
	public:
		GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

	private:
		QSizeF m_size;

		GraphicsGridLayoutItem(const GraphicsGridLayoutItem&) = delete;
		GraphicsGridLayoutItem& operator = (const GraphicsGridLayoutItem&) = delete;
	};

}
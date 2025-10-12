//! @file GraphicsBoxLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"

// Qt header
#include <QtWidgets/qgraphicslinearlayout.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayoutItem : public QGraphicsLinearLayout, public ot::GraphicsLayoutItem {
		OT_DECL_NOCOPY(GraphicsBoxLayoutItem)
	public:
		GraphicsBoxLayoutItem(Qt::Orientation _orientation, GraphicsItemCfg* _configuration, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayoutItem();

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

	private:
		GraphicsBoxLayoutItem() = delete;
	};

}
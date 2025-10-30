// @otlicense
// File: GraphicsGridLayoutItem.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"

// Qt header
#include <QtWidgets/qgraphicsgridlayout.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsGridLayoutItem : public QGraphicsGridLayout, public ot::GraphicsLayoutItem {
		OT_DECL_NOCOPY(GraphicsGridLayoutItem)
		OT_DECL_NOMOVE(GraphicsGridLayoutItem)
	public:
		GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayoutItem();

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
	};

}
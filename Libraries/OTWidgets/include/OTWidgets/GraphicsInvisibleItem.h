// @otlicense
// File: GraphicsInvisibleItem.h
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
#include "OTWidgets/CustomGraphicsItem.h"

#pragma once

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsInvisibleItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsInvisibleItem)
		OT_DECL_NOMOVE(GraphicsInvisibleItem)
	public:
		GraphicsInvisibleItem();
		virtual ~GraphicsInvisibleItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		void setItemSize(double _width, double _height) { this->setItemSize(Size2DD(_width, _height)); };
		void setItemSize(const QSizeF& _size) { this->setItemSize(Size2DD(_size.width(), _size.height())); };
		void setItemSize(const Size2DD& _size);
		const Size2DD& getItemSize(void) const;

	};

}
// @otlicense
// File: GraphicsDecoratedLineItem.h
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
#include "OTGui/Graphics/GraphicsDecoratedLineItemCfg.h"
#include "OTWidgets/GraphicsLineItem.h"
#include "OTWidgets/GraphicsDecoration.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsDecoratedLineItem : public GraphicsLineItem {
		OT_DECL_NOCOPY(GraphicsDecoratedLineItem)
		OT_DECL_NOMOVE(GraphicsDecoratedLineItem)
	public:
		GraphicsDecoratedLineItem();
		virtual ~GraphicsDecoratedLineItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual QMarginsF getOutlineMargins(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:
		void setFromDecoration(const GraphicsDecorationCfg& _config);
		void setToDecoration(const GraphicsDecorationCfg& _config);

	private:
		GraphicsDecoration m_fromDecoration;
		GraphicsDecoration m_toDecoration;

	};

}
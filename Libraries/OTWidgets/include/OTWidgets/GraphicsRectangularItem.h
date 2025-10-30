// @otlicense
// File: GraphicsRectangularItem.h
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
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsRectangularItem)
		OT_DECL_NOMOVE(GraphicsRectangularItem)
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

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

		void setRectangleSize(const Size2DD& _size);
		void setRectangleSize(const QSizeF& _size);
		const Size2DD& getRectangleSize(void) const;

		//! @brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;

		void setCornerRadius(int _r);
		int getCornerRadius(void) const;
	};


}
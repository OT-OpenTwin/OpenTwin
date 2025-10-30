// @otlicense
// File: GraphicsArcItem.h
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
#include "OTCore/Rect.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsArcItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsArcItem)
		OT_DECL_NOMOVE(GraphicsArcItem)
	public:
		GraphicsArcItem();
		virtual ~GraphicsArcItem();

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
		void setArcRect(const RectD& _rect);
		void setArcRect(const QRectF& _rect);
		const RectD& getArcRect(void) const;

		void setStartAngle(double _angle);
		double getStartAngle(void) const;

		void setSpanAngle(double _angle);
		double getSpanAngle(void) const;

		void setLineStyle(const PenFCfg& _style);
		const PenFCfg& getLineStyle(void) const;
	};


}
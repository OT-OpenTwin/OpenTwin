// @otlicense
// File: GraphicsShapeItem.h
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
#include "OTGui/Path2D.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsShapeItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsShapeItem)
		OT_DECL_NOMOVE(GraphicsShapeItem)
	public:
		GraphicsShapeItem();
		virtual ~GraphicsShapeItem();

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
		//! @brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(Painter2D* _painter);
		const Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;

		void setFillShape(bool _fill);
		bool getFillShape(void) const;

		//! @brief Will set the current path.
		//! @param _path Path to set.
		void setOutlinePath(const Path2DF& _path);

		//! @brief Current path.
		const Path2DF& getOutlinePath(void) const;
	};
}
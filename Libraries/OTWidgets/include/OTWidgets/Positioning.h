// @otlicense
// File: Positioning.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qrect.h>

class QWidget;

namespace ot {

	class WidgetBase;

	class OT_WIDGETS_API_EXPORT Positioning {
		OT_DECL_NOCOPY(Positioning)
		OT_DECL_NODEFAULT(Positioning)
	public:
		enum FitMode {
			FitByTopLeft, //! @brief Uses the top left corner as reference point.
			FitByCenter   //! @brief Uses the center point as reference point.
		};

		//! @brief Calculates the child rect.
		//! @param _parentRect Parent rectangle.
		//! @param _childSize Child size.
		//! @param _childAlignment Child alignment.
		static QRect calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment);

		//! @brief Calculates the child rect.
		//! @param _parentRect Parent rectangle.
		//! @param _childSize Child size.
		//! @param _childAlignment Child alignment.
		static QRectF calculateChildRect(const QRectF& _parentRect, const QSizeF& _childSize, ot::Alignment _childAlignment);

		//! @brief Fits the provided rect on the screen.
		//! If the source rect is bigger than the screen size the source rect will be returned.
		//! @param _sourceRect Initial rect.
		//! @param _mode Mode how to fit the rect on the screen.
		static QRect fitOnScreen(const QRect& _sourceRect, FitMode _mode = FitByCenter);

		//! @brief Centers this widget on the parent.
		//! If no parent is provided the widget will center on the primay screen.
		//! The size of the child won't be affected.
		static bool centerWidgetOnParent(const QWidget* _parentWidget, QWidget* _childWidget);

		//! @brief Calculates the top left corner of this widget centered on the parent widget.
		//! If no parent is provided the widget will center on the screen.
		static QRect getCenterWidgetOnParentRect(const QWidget* _parentWidget, const QRect& _childRect);

	};
}
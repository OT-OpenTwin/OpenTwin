// @otlicense
// File: CartesianPlotLegendView.h
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
#include <QtWidgets/qscrollarea.h>

class QWidget;
class QVBoxLayout;

namespace ot {

	class CartesianPlotLegendView : public QScrollArea {
		OT_DECL_NODEFAULT(CartesianPlotLegendView)
		OT_DECL_NOCOPY(CartesianPlotLegendView)
		OT_DECL_NOMOVE(CartesianPlotLegendView)
	public:
		explicit CartesianPlotLegendView(QWidget* _parent);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		QWidget* contentsWidget() const { return m_contentsWidget; };
		QVBoxLayout* contentsLayout() const { return m_contentsLayout; };

	private:
		QWidget* m_contentsWidget;
		QVBoxLayout* m_contentsLayout;

	};

}
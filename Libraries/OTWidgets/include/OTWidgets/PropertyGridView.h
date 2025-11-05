// @otlicense
// File: PropertyGridView.h
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
#include "OTWidgets/WidgetView.h"

namespace ot {

	class PropertyGrid;

	class OT_WIDGETS_API_EXPORT PropertyGridView : public WidgetView {
		OT_DECL_NOCOPY(PropertyGridView)
		OT_DECL_NOMOVE(PropertyGridView)
		OT_DECL_NODEFAULT(PropertyGridView)
	public:
		explicit PropertyGridView(QWidget* _parent);
		explicit PropertyGridView(PropertyGrid* _propertyGrid, QWidget* _parent);
		virtual ~PropertyGridView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		PropertyGrid* getPropertyGrid(void) const { return m_propertyGrid; };

	private:
		PropertyGrid* m_propertyGrid;
	};

}
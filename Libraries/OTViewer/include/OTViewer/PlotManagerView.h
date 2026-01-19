// @otlicense
// File: PlotManagerView.h
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

	class PlotManager;

	class __declspec(dllexport) PlotManagerView : public WidgetView {
		OT_DECL_NOCOPY(PlotManagerView)
		OT_DECL_NOMOVE(PlotManagerView)
		OT_DECL_NODEFAULT(PlotManagerView)
	public:
		PlotManagerView(QWidget* _parent);
		virtual ~PlotManagerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		PlotManager* getPlotManager(void) const { return m_plotManager; };

	private:
		PlotManager* m_plotManager;

	};

}

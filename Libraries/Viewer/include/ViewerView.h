// @otlicense
// File: ViewerView.h
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

class Viewer;

namespace ot {

	class __declspec(dllexport) ViewerView : public WidgetView {
	public:
		ViewerView() = delete;
		ViewerView(ot::UID _modelID, ot::UID _viewerID, double _scaleWidth, double _scaleHeight, int _backgroundR, int _backgroundG, int _backgroundB, int _overlayTextR, int _overlayTextG, int _overlayTextB);
		virtual ~ViewerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		Viewer* getViewer(void) const { return m_viewer; };

	private:
		Viewer* m_viewer;

	};

}

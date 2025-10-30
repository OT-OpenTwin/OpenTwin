// @otlicense
// File: Painter2DDialogFilterDefaults.cpp
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

// OpenTwin header
#include "OTGui/Painter2DDialogFilterDefaults.h"

ot::Painter2DDialogFilter ot::Painter2DDialogFilterDefaults::plotCurve(bool _allowTransparent) {
	Painter2DDialogFilter result;

	result.setPainterTypes(Painter2DDialogFilter::Fill | ot::Painter2DDialogFilter::StyleRef);
	result.setStyleReferences(std::list<ColorStyleValueEntry> {
		ColorStyleValueEntry::PlotCurve,
		ColorStyleValueEntry::PlotCurveSymbol,
		ColorStyleValueEntry::Rainbow0,
		ColorStyleValueEntry::Rainbow1,
		ColorStyleValueEntry::Rainbow2,
		ColorStyleValueEntry::Rainbow3,
		ColorStyleValueEntry::Rainbow4,
		ColorStyleValueEntry::Rainbow5,
		ColorStyleValueEntry::Rainbow6,
		ColorStyleValueEntry::Rainbow7,
		ColorStyleValueEntry::Rainbow8,
		ColorStyleValueEntry::Rainbow9,
	});

	if (_allowTransparent) {
		result.addStyleReference(ColorStyleValueEntry::Transparent);
	}

	return result;
}

// @otlicense
// File: GraphicsItemDesignerInfoOverlay.h
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
#include "OTWidgets/OverlayWidgetBase.h"

// Qt header
#include <QtCore/qstring.h>

class QLabel;
class GraphicsItemDesigner;

class GraphicsItemDesignerInfoOverlay : public ot::OverlayWidgetBase {
public:
	GraphicsItemDesignerInfoOverlay(const QString& _text, GraphicsItemDesignerView* _view, const QMargins& _overlayMargins = QMargins());

	void setText(const QString& _text);

private:
	QLabel* m_label;

};

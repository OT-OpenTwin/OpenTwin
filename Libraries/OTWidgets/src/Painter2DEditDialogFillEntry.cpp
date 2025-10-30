// @otlicense
// File: Painter2DEditDialogFillEntry.cpp
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
#include "OTGui/FillPainter2D.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/Painter2DEditDialogFillEntry.h"

ot::Painter2DEditDialogFillEntry::Painter2DEditDialogFillEntry(const Painter2D* _painter) {
	m_btn = new ColorPickButton;
	if (_painter) {
		const FillPainter2D* actualPainter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setColor(actualPainter->getColor());
	}
	this->connect(m_btn, &ColorPickButton::colorChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogFillEntry::~Painter2DEditDialogFillEntry() {
	delete m_btn;
}

QWidget* ot::Painter2DEditDialogFillEntry::getRootWidget() const {
	return m_btn;
}

ot::Painter2D* ot::Painter2DEditDialogFillEntry::createPainter() const {
	return new FillPainter2D(m_btn->otColor());
}


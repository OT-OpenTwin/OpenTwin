// @otlicense
// File: Painter2DEditDialogFillEntry.h
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
#include "OTWidgets/Painter2DEditDialogEntry.h"

namespace ot {

	class ColorPickButton;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogFillEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogFillEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogFillEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::FillType; };
		virtual QWidget* getRootWidget() const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		ColorPickButton* m_btn;
	};

}

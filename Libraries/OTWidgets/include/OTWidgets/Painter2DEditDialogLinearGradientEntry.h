// @otlicense
// File: Painter2DEditDialogLinearGradientEntry.h
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

class QWidget;
class QVBoxLayout;

namespace ot {

	class DoubleSpinBox;
	class Painter2DEditDialogGradientBase;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogLinearGradientEntry : public Painter2DEditDialogEntry {
		OT_DECL_NOCOPY(Painter2DEditDialogLinearGradientEntry)
		OT_DECL_NOMOVE(Painter2DEditDialogLinearGradientEntry)
		OT_DECL_NODEFAULT(Painter2DEditDialogLinearGradientEntry)
	public:
		explicit Painter2DEditDialogLinearGradientEntry(const Painter2D* _painter, QWidget* _parent);
		virtual ~Painter2DEditDialogLinearGradientEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::LinearType; };
		virtual QWidget* getRootWidget() const override { return m_cLayW; };

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		QWidget* m_cLayW;
		QVBoxLayout* m_cLay;
		Painter2DEditDialogGradientBase* m_gradientBase;

		DoubleSpinBox* m_startX;
		DoubleSpinBox* m_startY;
		DoubleSpinBox* m_finalX;
		DoubleSpinBox* m_finalY;
	};

}
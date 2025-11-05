// @otlicense
// File: Painter2DEditDialogReferenceEntry.h
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
#include "OTGui/Painter2DDialogFilter.h"
#include "OTWidgets/Painter2DEditDialogEntry.h"

namespace ot {

	class ComboBox;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogReferenceEntry : public Painter2DEditDialogEntry {
		OT_DECL_NOCOPY(Painter2DEditDialogReferenceEntry)
		OT_DECL_NOMOVE(Painter2DEditDialogReferenceEntry)
		OT_DECL_NODEFAULT(Painter2DEditDialogReferenceEntry)
	public:
		explicit Painter2DEditDialogReferenceEntry(const Painter2DDialogFilter& _filter, const Painter2D* _painter, QWidget* _parent);
		virtual ~Painter2DEditDialogReferenceEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::ReferenceType; };
		virtual QWidget* getRootWidget() const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		ComboBox* m_comboBox;
	};

}

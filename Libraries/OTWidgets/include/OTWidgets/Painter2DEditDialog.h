// @otlicense
// File: Painter2DEditDialog.h
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
#include "OTGui/GuiTypes.h"
#include "OTGui/Painter2DDialogFilter.h"
#include "OTGui/GradientPainterStop2D.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/ColorStyle.h"

class QVBoxLayout;

namespace ot {

	class ComboBox;
	class Painter2D;
	class PushButton;
	class Painter2DPreview;
	class Painter2DEditDialogEntry;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NODEFAULT(Painter2DEditDialog)
	public:
		enum PainterEntryType {
			FillType,
			LinearType,
			RadialType
		};

		Painter2DEditDialog(const Painter2DDialogFilter& _filter, const Painter2D* _painter = (const Painter2D*)nullptr);
		virtual ~Painter2DEditDialog();

		const Painter2D* currentPainter() const { return m_painter; };
		Painter2D* createPainter() const;

		void setHasChanged(bool _changed) { m_changed = _changed; };
		bool hasChanged() const { return m_changed; };

	Q_SIGNALS:
		void painterChanged();

	private Q_SLOTS:
		void slotTypeChanged();
		void slotUpdate();
		void slotConfirm();

	private:
		void ini();
		void applyPainter(const Painter2D* _painter);

		Painter2D* m_painter;
		Painter2DDialogFilter m_filter;

		bool m_changed;
		ComboBox* m_typeSelectionBox;
		QVBoxLayout* m_vLayout;
		Painter2DPreview* m_preview;
		Painter2DEditDialogEntry* m_currentEntry;
		PushButton* m_confirm;
		PushButton* m_cancel;
		QWidget* m_stretchItem;
	};

}

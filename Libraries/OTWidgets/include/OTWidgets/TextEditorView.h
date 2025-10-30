// @otlicense
// File: TextEditorView.h
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

	class TextEditor;

	class OT_WIDGETS_API_EXPORT TextEditorView : public WidgetView {
		Q_OBJECT
	public:
		TextEditorView(TextEditor* _textEditor = (TextEditor*)nullptr);
		virtual ~TextEditorView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		TextEditor* getTextEditor(void) const { return m_textEditor; };

	Q_SIGNALS:
		void saveRequested(void);

	private Q_SLOTS:
		void slotSaveRequested(void);
		void slotModifiedChanged(bool _isModified);
		
	private:
		TextEditor* m_textEditor;

	};

}

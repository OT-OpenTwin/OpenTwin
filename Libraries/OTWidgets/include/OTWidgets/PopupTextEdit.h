// @otlicense
// File: PopupTextEdit.h
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
#include "OTWidgets/WidgetBase.h"

namespace ot {

	class LineEdit;
	class ToolButton;

	class OT_WIDGETS_API_EXPORT PopupTextEdit : public QWidget, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PopupTextEdit)
		OT_DECL_NOMOVE(PopupTextEdit)
		OT_DECL_NODEFAULT(PopupTextEdit)
	public:
		explicit PopupTextEdit(QWidget* _parent);
		explicit PopupTextEdit(const QString& _text, QWidget* _parent);
		virtual ~PopupTextEdit();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setText(const QString& _text);
		const QString& getText() const { return m_text; };

		void setPlaceholderText(const QString& _text);
		QString getPlaceholderText() const;

		void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; };
		bool getReadOnly() const { return m_readOnly; };

		void setMaxTextLength(int _maxLength) { m_maxLength = _maxLength; };
		int getMaxTextLength() const { return m_maxLength; };

	Q_SIGNALS:
		void textChanged(const QString& _newText);

	private Q_SLOTS:
		void openEditor();

	private:
		void updatePreview();

		QString m_text;
		LineEdit* m_preview;
		bool m_readOnly;
		int m_maxLength;

		ToolButton* m_button;
	};

}
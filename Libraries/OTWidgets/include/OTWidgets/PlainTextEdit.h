// @otlicense
// File: PlainTextEdit.h
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

// Qt header
#include <QtWidgets/qplaintextedit.h>

namespace ot {

	class CustomValidator;

	class OT_WIDGETS_API_EXPORT PlainTextEdit : public QPlainTextEdit, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlainTextEdit)
		OT_DECL_NOMOVE(PlainTextEdit)
		OT_DECL_NODEFAULT(PlainTextEdit)
	public:
		explicit PlainTextEdit(QWidget* _parent);
		virtual ~PlainTextEdit();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setAutoScrollToBottomEnabled(bool _enabled);
		bool isAutoScrollToBottomEnabled() const { return m_autoScrollToBottom; }

		void setValidator(CustomValidator* _validator);
		CustomValidator* getValidator() const { return m_validator; };

		void scrollToBottom();

		void setMaxTextLength(int _maxLength);
		int getMaxTextLength() const { return m_maxLength; };

		void setTextChanged(bool _changed) { m_hasChanged = _changed; };
		bool getTextChanged() const { return m_hasChanged; };
		
	Q_SIGNALS:
		void editingFinished();

	public Q_SLOTS:
		void slotTextChanged();

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;
		virtual void insertFromMimeData(const QMimeData* _source) override;
		virtual void focusOutEvent(QFocusEvent* _event) override;

	private:
		bool m_hasChanged;
		CustomValidator* m_validator;
		bool m_autoScrollToBottom;
		int m_maxLength;

	};

}

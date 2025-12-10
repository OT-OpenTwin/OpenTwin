// @otlicense
// File: PopupTextEdit.cpp
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
#include "OTWidgets/Dialog.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PopupTextEdit.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::PopupTextEdit::PopupTextEdit(QWidget* _parent)
	: QWidget(_parent), m_maxLength(0), m_readOnly(false)
{
	setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	
	m_preview = new ot::LineEdit(this);
	m_preview->setReadOnly(true);

	m_button = new ot::ToolButton(this);
	m_button->setIcon(ot::IconManager::getIcon("Button/Edit"));

	layout->addWidget(m_preview, 1);
	layout->addWidget(m_button);

	connect(m_preview, &ot::LineEdit::leftMouseButtonPressed, this, &ot::PopupTextEdit::openEditor);
	connect(m_button, &ot::ToolButton::clicked, this, &ot::PopupTextEdit::openEditor);
}

ot::PopupTextEdit::PopupTextEdit(const QString& _text, QWidget* _parent) 
	: PopupTextEdit(_parent)
{
	setText(_text);
}

ot::PopupTextEdit::~PopupTextEdit() {

}

void ot::PopupTextEdit::setText(const QString& _text) {
	if (m_text != _text) {
		m_text = _text;
		updatePreview();
		textChanged(m_text);
	}
}

void ot::PopupTextEdit::setPlaceholderText(const QString& _text) {
	m_preview->setPlaceholderText(_text);
}

QString ot::PopupTextEdit::getPlaceholderText() const {
	return m_preview->placeholderText();
}

void ot::PopupTextEdit::openEditor() {
	Dialog dia(this);
	dia.setWindowTitle("Edit Text");
	dia.setMinimumSize(300, 300);

	QVBoxLayout* mainLayout = new QVBoxLayout(&dia);
	QHBoxLayout* centerLayout = new QHBoxLayout;
	mainLayout->addLayout(centerLayout, 1);

	TextEditor* editor = new TextEditor(&dia);
	editor->setPlainText(m_text);
	editor->setReadOnly(m_readOnly);
	editor->setMaxTextLength(m_maxLength);
	centerLayout->addWidget(editor);
	connect(editor, &TextEditor::saveRequested, &dia, &Dialog::closeOk);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mainLayout->addLayout(buttonLayout);
	buttonLayout->addStretch(1);

	PushButton* okButton = new PushButton("OK", &dia);
	buttonLayout->addWidget(okButton);
	okButton->setToolTip("Save changes and close editor (Ctrl+S)");
	connect(okButton, &PushButton::clicked, &dia, &Dialog::closeOk);

	PushButton* cancelButton = new PushButton("Cancel", &dia);
	buttonLayout->addWidget(cancelButton);
	connect(cancelButton, &PushButton::clicked, &dia, &Dialog::closeCancel);

	if (dia.showDialog() == Dialog::Ok) {
		setText(editor->toPlainText());
	}
}

void ot::PopupTextEdit::updatePreview() {
	const QStringList lines = m_text.split('\n');

	if (lines.isEmpty()) {
		m_preview->setText("");
		return;
	}

	QString preview = lines.first();
	if (lines.size() > 1) {
		preview.append(" ...");
	}

	m_preview->setText(preview);
}

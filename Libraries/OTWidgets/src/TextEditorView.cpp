// @otlicense
// File: TextEditorView.cpp
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
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/WidgetViewDock.h"

ot::TextEditorView::TextEditorView(QWidget* _parent) : TextEditorView(nullptr, _parent) {}

ot::TextEditorView::TextEditorView(TextEditor* _textEditor, QWidget* _parent)
	: WidgetView(WidgetViewBase::ViewText, _parent), m_textEditor(_textEditor)
{
	if (!m_textEditor) {
		m_textEditor = new TextEditor(getViewDockWidget());
	}
	else {
		m_textEditor->setParent(getViewDockWidget());
	}

	this->addWidgetInterfaceToDock(m_textEditor);
	this->connect(m_textEditor, &TextEditor::modificationChanged, this, &TextEditorView::slotModifiedChanged);
	this->connect(m_textEditor, &TextEditor::saveRequested, this, &TextEditorView::slotSaveRequested);
}

ot::TextEditorView::~TextEditorView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::TextEditorView::getViewWidget(void) {
	return m_textEditor;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::TextEditorView::slotSaveRequested(void) {
	Q_EMIT saveRequested();
}

void ot::TextEditorView::slotModifiedChanged(bool _isModified) {
	this->setViewContentModified(_isModified);
}


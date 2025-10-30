// @otlicense

// OpenTwin header
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TextEditorView.h"

ot::TextEditorView::TextEditorView(TextEditor* _textEditor)
	: WidgetView(WidgetViewBase::ViewText), m_textEditor(_textEditor)
{
	if (!m_textEditor) {
		m_textEditor = new TextEditor;
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


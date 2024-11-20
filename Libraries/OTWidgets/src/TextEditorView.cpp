//! @file TextEditorView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TextEditorView.h"

ot::TextEditorView::TextEditorView() {
	this->addWidgetToDock(this);
}

ot::TextEditorView::~TextEditorView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::TextEditorView::getViewWidget(void) {
	return this;
}

void ot::TextEditorView::setupFromConfig(const TextEditorCfg& _config, bool _isUpdate) {
	TextEditor::setupFromConfig(_config, _isUpdate);
	if (!_isUpdate) {
		this->setViewData(_config);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::TextEditorView::contentSaved(void) {
	this->setViewContentModified(false);
}

void ot::TextEditorView::contentChanged(void) {
	this->setViewContentModified(true);
}
//! @file TextEditorView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::TextEditorView> TextEditorViewRegistrar(OT_WIDGETTYPE_TextEditor);

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

bool ot::TextEditorView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;


	return true;
}
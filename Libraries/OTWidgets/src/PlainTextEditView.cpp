//! @file PlainTextEditView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/PlainTextEditView.h"

ot::PlainTextEditView::PlainTextEditView(PlainTextEdit* _textEdit)
	: WidgetView(WidgetViewBase::ViewText), m_textEdit(_textEdit)
{
	if (!m_textEdit) {
		m_textEdit = new PlainTextEdit;
	}

	this->addWidgetInterfaceToDock(m_textEdit);
}

ot::PlainTextEditView::~PlainTextEditView() {

}
	
// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlainTextEditView::getViewWidget(void) {
	return m_textEdit;
}

//! @file JSONEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "JSONEditor.h"

JSONEditor::JSONEditor() {
	ini();
}

JSONEditor::JSONEditor(const QString& _initialText) : QPlainTextEdit(_initialText) {
	ini();
}

JSONEditor::~JSONEditor() {

}

void JSONEditor::ini(void) {
	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("Consolas");
	setFont(f);
	QFontMetrics fm(f);
	setTabStopWidth(4 * fm.width(' '));
}
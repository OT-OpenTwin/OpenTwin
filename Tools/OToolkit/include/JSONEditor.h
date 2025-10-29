// @otlicense

//! @file JSONEditor.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtWidgets/qplaintextedit.h>

class JSONEditor : public QPlainTextEdit {
public:
	JSONEditor();
	JSONEditor(const QString& _initialText);
	virtual ~JSONEditor();

private:
	void ini(void);
};
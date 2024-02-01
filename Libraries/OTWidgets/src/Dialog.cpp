//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Dialog.h"

ot::Dialog::Dialog(QWidget* _parent) : QDialog(_parent), m_result(DialogResult::Cancel) {
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

ot::Dialog::~Dialog() {}

int ot::Dialog::showDialog(void) {
	if (this->parentWidget()) {
		this->centerOnParent(this->parentWidget());
	}
	return this->exec();
}

void ot::Dialog::close(DialogResult _result) {
	m_result = _result;
	this->QDialog::close();
}
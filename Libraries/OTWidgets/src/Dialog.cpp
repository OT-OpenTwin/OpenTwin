//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Dialog.h"

ot::Dialog::Dialog(QWidget* _parent) : QDialog(_parent) {
	this->setWindowFlags(this->windowFlags() & (~Qt::AA_DisableWindowContextHelpButton));
}

ot::Dialog::~Dialog() {}
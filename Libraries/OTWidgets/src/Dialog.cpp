//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Dialog.h"

ot::Dialog::Dialog(QWidget* _parent)
	: QDialog(_parent), m_flags(DialogCfg::NoFlags), m_result(DialogResult::Cancel) 
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

ot::Dialog::Dialog(const DialogCfg& _config, QWidget* _parent) 
	: QDialog(_parent), m_flags(_config.getFlags()), m_dialogName(_config.getName()), m_result(DialogResult::Cancel)
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
	this->setWindowTitle(QString::fromStdString(_config.getTitle()));
	if (_config.getMinSize().width() >= 0) { this->setMinimumWidth(_config.getMinSize().width()); };
	if (_config.getMinSize().height() >= 0) { this->setMinimumHeight(_config.getMinSize().height()); };
	if (_config.getMaxSize().width() >= 0) { this->setMaximumWidth(_config.getMaxSize().width()); };
	if (_config.getMaxSize().height() >= 0) { this->setMaximumHeight(_config.getMaxSize().height()); };
}

ot::Dialog::~Dialog() {}

ot::Dialog::DialogResult ot::Dialog::showDialog(void) {
	if (this->parentWidget()) {
		this->centerOnParent(this->parentWidget());
	}
	this->exec();
	return m_result;
}

void ot::Dialog::close(DialogResult _result) {
	m_result = _result;
	this->QDialog::close();
}

void ot::Dialog::closeOk(void) {
	this->close(Dialog::Ok);
}

void ot::Dialog::closeYes(void) {
	this->close(Dialog::Yes);
}

void ot::Dialog::closeNo(void) {
	this->close(Dialog::No);
}

void ot::Dialog::closeRetry(void) {
	this->close(Dialog::Retry);
}

void ot::Dialog::closeCancel(void) {
	this->close(Dialog::Cancel);
}
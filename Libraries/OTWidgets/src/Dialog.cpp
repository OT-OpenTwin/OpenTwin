//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Dialog.h"
#include "OTWidgets/OTQtConverter.h"

ot::Dialog::Dialog(QWidget* _parent)
	: QDialog(_parent), m_flags(DialogCfg::NoFlags), m_result(DialogResult::Cancel) 
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

ot::Dialog::Dialog(const DialogCfg& _config, QWidget* _parent) 
	: QDialog(_parent), m_flags(_config.flags()), m_dialogName(_config.name()), m_result(DialogResult::Cancel)
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
	this->setWindowTitle(QString::fromStdString(_config.title()));
	if (_config.minSize().width() >= 0) { this->setMinimumWidth(_config.minSize().width()); };
	if (_config.minSize().height() >= 0) { this->setMinimumHeight(_config.minSize().height()); };
	if (_config.maxSize().width() >= 0) { this->setMaximumWidth(_config.maxSize().width()); };
	if (_config.maxSize().height() >= 0) { this->setMaximumHeight(_config.maxSize().height()); };
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
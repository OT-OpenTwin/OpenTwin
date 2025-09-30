//! @file MessageDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/MessageDialog.h"

ot::MessageDialogCfg::BasicButton ot::MessageDialog::showDialog(const MessageDialogCfg& _config, QWidget* _parent) {
	QMessageBox msg(
		MessageDialog::convertIcon(_config.getIcon()),
		QString::fromStdString(_config.getTitle()),
		QString::fromStdString(_config.getText()),
		MessageDialog::convertButtons(_config.getButtons()), 
		_parent
	);
	
	if (!_config.getDetailedText().empty()) {
		msg.setDetailedText(QString::fromStdString(_config.getDetailedText()));
	}

	QMessageBox::StandardButton btn = (QMessageBox::StandardButton)msg.exec();
	
	switch (btn)
	{
	case QMessageBox::NoButton: return MessageDialogCfg::Cancel;
	case QMessageBox::Ok: return MessageDialogCfg::Ok;
	case QMessageBox::Save: return MessageDialogCfg::Save;
	case QMessageBox::SaveAll: return MessageDialogCfg::SaveAll;
	case QMessageBox::Open: return MessageDialogCfg::Open;
	case QMessageBox::Yes: return MessageDialogCfg::Yes;
	case QMessageBox::YesToAll: return MessageDialogCfg::YesToAll;
	case QMessageBox::No: return MessageDialogCfg::No;
	case QMessageBox::NoToAll: return MessageDialogCfg::NoToAll;
	case QMessageBox::Abort: return MessageDialogCfg::Abort;
	case QMessageBox::Retry: return MessageDialogCfg::Retry;
	case QMessageBox::Ignore: return MessageDialogCfg::Ignore;
	case QMessageBox::Close: return MessageDialogCfg::Close;
	case QMessageBox::Cancel: return MessageDialogCfg::Cancel;
	case QMessageBox::Discard: return MessageDialogCfg::Discard;
	case QMessageBox::Help: return MessageDialogCfg::Help;
	case QMessageBox::Apply: return MessageDialogCfg::Apply;
	case QMessageBox::Reset: return MessageDialogCfg::Reset;
	case QMessageBox::RestoreDefaults: return MessageDialogCfg::RestoreDefaults;
	default:
		OT_LOG_EAS("Unknown button (" + std::to_string((int)btn) + ")");
		return MessageDialogCfg::Cancel;
	}
}

QMessageBox::Icon ot::MessageDialog::convertIcon(MessageDialogCfg::BasicIcon _icon) {
	switch (_icon)
	{
	case ot::MessageDialogCfg::NoIcon: return QMessageBox::NoIcon;
	case ot::MessageDialogCfg::Information: return QMessageBox::Information;
	case ot::MessageDialogCfg::Question: return QMessageBox::Question;
	case ot::MessageDialogCfg::Warning: return QMessageBox::Warning;
	case ot::MessageDialogCfg::Critical: return QMessageBox::Critical;
	default:
		OT_LOG_EAS("Unknown icon (" + std::to_string((int)_icon) + ")");
		return QMessageBox::NoIcon;
	}
}

QMessageBox::StandardButtons ot::MessageDialog::convertButtons(const MessageDialogCfg::BasicButtons& _buttons) {
	QMessageBox::StandardButtons ret = QMessageBox::NoButton;

	if (_buttons & MessageDialogCfg::Ok) ret |= QMessageBox::Ok;
	if (_buttons & MessageDialogCfg::Save) ret |= QMessageBox::Save;
	if (_buttons & MessageDialogCfg::SaveAll) ret |= QMessageBox::SaveAll;
	if (_buttons & MessageDialogCfg::Open) ret |= QMessageBox::Open;
	if (_buttons & MessageDialogCfg::Yes) ret |= QMessageBox::Yes;
	if (_buttons & MessageDialogCfg::YesToAll) ret |= QMessageBox::YesToAll;
	if (_buttons & MessageDialogCfg::No) ret |= QMessageBox::No;
	if (_buttons & MessageDialogCfg::NoToAll) ret |= QMessageBox::NoToAll;
	if (_buttons & MessageDialogCfg::Abort) ret |= QMessageBox::Abort;
	if (_buttons & MessageDialogCfg::Retry) ret |= QMessageBox::Retry;
	if (_buttons & MessageDialogCfg::Ignore) ret |= QMessageBox::Ignore;
	if (_buttons & MessageDialogCfg::Close) ret |= QMessageBox::Close;
	if (_buttons & MessageDialogCfg::Cancel) ret |= QMessageBox::Cancel;
	if (_buttons & MessageDialogCfg::Discard) ret |= QMessageBox::Discard;
	if (_buttons & MessageDialogCfg::Help) ret |= QMessageBox::Help;
	if (_buttons & MessageDialogCfg::Apply) ret |= QMessageBox::Apply;
	if (_buttons & MessageDialogCfg::Reset) ret |= QMessageBox::Reset;
	if (_buttons & MessageDialogCfg::RestoreDefaults) ret |= QMessageBox::RestoreDefaults;

	return ret;
}
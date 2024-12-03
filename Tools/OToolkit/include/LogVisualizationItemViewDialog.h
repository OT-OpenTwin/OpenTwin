//! @file LogVisualizationItemViewDialog.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Dialog.h"

class QLabel;
class QWidget;
class QLineEdit;
class QCheckBox;
class QShortcut;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QPlainTextEdit;

class LogVisualizationItemViewDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(LogVisualizationItemViewDialog)
	OT_DECL_NODEFAULT(LogVisualizationItemViewDialog)
public:
	LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget* _parent);
	virtual ~LogVisualizationItemViewDialog();

protected:
	virtual void closeEvent(QCloseEvent* _event) override;

	virtual void mousePressEvent(QMouseEvent* _event) override;

	virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

	virtual bool event(QEvent* _event) override;

private Q_SLOTS:
	void slotRecenter(void);
	void slotDisplayMessageText(int _state);

private:
	QString findJsonSyntax(const QString& _inputString);

	ot::LogMessage m_msg;

	QShortcut* m_closeShortcut;
	QShortcut* m_recenterShortcut;
	QVBoxLayout* m_centralLayout;
	QGridLayout* m_dataLayout;
	QVBoxLayout* m_bigVLayout;
	QHBoxLayout* m_messageTitleLayout;
	QHBoxLayout* m_buttonLayout;

	QLabel* m_timeL;
	QLineEdit* m_time;
	QLabel* m_timeLocalL;
	QLineEdit* m_timeLocal;
	QLabel* m_userNameL;
	QLineEdit* m_userName;
	QLabel* m_projectNameL;
	QLineEdit* m_projectName;
	QLabel* m_senderNameL;
	QLineEdit* m_senderName;
	QLabel* m_messageTypeL;
	QLineEdit* m_messageType;
	QCheckBox* m_findMessageSyntax;

	QLabel* m_functionL;
	QLineEdit* m_function;
	QLabel* m_messageL;
	QPlainTextEdit* m_message;

	QPushButton* m_okButton;
};

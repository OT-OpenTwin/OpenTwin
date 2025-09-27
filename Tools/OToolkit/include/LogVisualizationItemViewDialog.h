//! @file LogVisualizationItemViewDialog.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Dialog.h"

class QWidget;
class QLineEdit;
class QCheckBox;
class QShortcut;
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
	void slotUpdateTimestamps();

private:
	QString findJsonSyntax(const QString& _inputString);

	ot::LogMessage m_msg;

	QShortcut* m_closeShortcut;
	QShortcut* m_recenterShortcut;

	QLineEdit* m_time;
	QCheckBox* m_timeUTC;
	QLineEdit* m_timeLocal;
	QCheckBox* m_timeLocalUTC;
	QCheckBox* m_findMessageSyntax;

	QPlainTextEdit* m_message;

	QPushButton* m_okButton;
};

//! @file LogInDialog.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Frontend header
#include "LogInGSSEntry.h"

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>

class QSettings;

namespace ot {
	class LineEdit;
	class ComboBox;
}

class LogInDialog : public ot::Dialog {
	Q_OBJECT
public:
	
	LogInDialog();
	virtual ~LogInDialog();

	virtual void resizeEvent(QResizeEvent* _event) override;

private Q_SLOTS:
	void slotLogIn(void);
	void slotCancel(void);
	void slotNewUser(void);
	void slotGSSChanged(void);
	void slotPasswordChanged(void);

private:
	enum class LogInStateFlag {
		NoState = 0x00,
		RestoredPassword = 0x01
	};
	typedef ot::Flags<LogInStateFlag> LogInState;

	LogInState m_state;
	ot::ComboBox* m_gss;
	ot::LineEdit* m_username;
	ot::LineEdit* m_password;
	QString m_lastPassword;

	std::list<LogInGSSEntry> m_gssData;

	void initializeGssData(std::shared_ptr<QSettings> _settings);
	void updateGssOptions(void);

	OT_FRIEND_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)
};

OT_ADD_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)
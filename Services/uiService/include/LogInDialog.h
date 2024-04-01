//! @file LogInDialog.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {
	class ComboButton;
}

class LogInDialog : public ot::Dialog {
	Q_OBJECT
public:
	LogInDialog();
	virtual ~LogInDialog();

private Q_SLOTS:
	void slotLogIn(void);
	void slotCancel(void);
	void slotNewUser(void);
	void slotGSSChanged(void);

private:
	ot::ComboButton* m_gss;
	QString m_lastPassword;

};
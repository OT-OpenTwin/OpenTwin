// @otlicense

//! @file LogModeSetter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>

class QWidget;
class QComboBox;
class QCheckBox;
class QLineEdit;

class LogModeSetter : public QObject {
	Q_OBJECT
public:
	LogModeSetter();
	virtual ~LogModeSetter();

	QWidget* getRootWidget(void) const { return m_root; };

	void restoreSettings(QSettings& _settings);
	void saveSettings(QSettings& _settings);

private Q_SLOTS:
	void slotApply(void);
	void slotWorkerSuccess(void);
	void slotWorkerError(void);

private:
	void stopWorker(bool _success);

	void sendWorker(std::string _gss, ot::LogFlags _newMode);

	bool sendGlobalMode(const std::string& _gss, const ot::LogFlags& _flags);

	QWidget* m_root;

	QLineEdit* m_gssUrl;

	QComboBox* m_mode;

	QCheckBox* m_msgTypeDetailed;
	QCheckBox* m_msgTypeInfo;
	QCheckBox* m_msgTypeWarning;
	QCheckBox* m_msgTypeError;
	QCheckBox* m_msgTypeMsgIn;
	QCheckBox* m_msgTypeMsgOut;
	QCheckBox* m_msgTypeTest;

};
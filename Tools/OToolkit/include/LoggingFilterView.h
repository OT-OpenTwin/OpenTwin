//! @file LoggingFilterView.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>

class QTimer;
class QWidget;
class QCheckBox;
class QLineEdit;
class QListWidget;

class LoggingFilterView : public QObject {
	Q_OBJECT
public:
	LoggingFilterView();
	virtual ~LoggingFilterView();

	QWidget* getRootWidget(void) const { return m_root; };

	void reset(void);

	//! \brief Updates the service name list and returns true if the message should be displayed.
	bool filterMessage(const ot::LogMessage& _msg);

	void restoreSettings(QSettings& _settings);
	void saveSettings(QSettings& _settings);

	void setFilterLock(bool _active) { m_filterLock = _active; };
	bool getFilterLock(void) const { return m_filterLock; };

Q_SIGNALS:
	void filterChanged(void);

public Q_SLOTS:
	void slotUpdateCheckboxColors(void);

private Q_SLOTS:
	void slotFilterChanged(void);
	void slotSelectAllServices(void);
	void slotDeselectAllServices(void);

private:
	QWidget* m_root;

	bool m_filterLock;
	QTimer* m_filterTimer;

	QCheckBox* m_msgTypeFilterDetailed;
	QCheckBox* m_msgTypeFilterInfo;
	QCheckBox* m_msgTypeFilterWarning;
	QCheckBox* m_msgTypeFilterError;
	QCheckBox* m_msgTypeFilterMsgIn;
	QCheckBox* m_msgTypeFilterMsgOut;

	QListWidget* m_serviceFilter;

	QLineEdit* m_messageFilter;
};
// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>

class QTimer;
class QWidget;
class QComboBox;
class QLineEdit;
class QListWidget;

namespace ot { class SpinBox; }
namespace ot { class CheckBox; }

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

	int getMessageLimit() const;

	bool getUseInterval() const;

	int getIntervalMilliseconds() const;
	
Q_SIGNALS:
	void filterChanged();
	void messageLimitChanged(int _limit);
	void useIntervalChaged();

public Q_SLOTS:
	void slotUpdateCheckboxColors(void);

private Q_SLOTS:
	void slotFilterChanged();
	void slotSelectAllServices();
	void slotDeselectAllServices();
	void slotMessageLimitChanged();
	void slotIntervalChanged();

private:
	void updateMessageLimitColor();
	void updateIntervalColor();

	QWidget* m_root;

	bool m_filterLock;
	QTimer* m_filterTimer;

	ot::CheckBox* m_msgTypeFilterDetailed;
	ot::CheckBox* m_msgTypeFilterInfo;
	ot::CheckBox* m_msgTypeFilterWarning;
	ot::CheckBox* m_msgTypeFilterError;
	ot::CheckBox* m_msgTypeFilterMsgIn;
	ot::CheckBox* m_msgTypeFilterMsgOut;
	ot::CheckBox* m_msgTypeFilterTest;

	ot::CheckBox* m_messageLimitEnabled;
	ot::SpinBox* m_messageLimit;

	ot::CheckBox* m_useInterval;
	ot::SpinBox* m_intervalMilliseconds;

	QComboBox* m_userFilter;
	QComboBox* m_sessionFilter;

	QListWidget* m_serviceFilter;

	QLineEdit* m_messageFilter;
};
// @otlicense
// File: LoggingFilterView.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCore/RAII/CheckpointRAII.h"

// Qt header
#include <QtCore/qtimer.h>
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
	OT_DECL_NOCOPY(LoggingFilterView)
	OT_DECL_NOMOVE(LoggingFilterView)
public:
	enum class State
	{
		None = 0 << 0,
		Modifying = 1 << 0
	};
	typedef ot::Flags<State> StateFlags;

	LoggingFilterView();
	virtual ~LoggingFilterView();

	QWidget* getRootWidget() const { return m_root; };

	void reset();

	//! \brief Updates the service name list and returns true if the message should be displayed.
	bool filterMessage(const ot::LogMessage& _msg);

	void restoreSettings(QSettings& _settings);
	void saveSettings(QSettings& _settings);

	int getMessageLimit() const;

	bool getUseInterval() const;

	int getIntervalMilliseconds() const;
	
	OT_DECL_NODISCARD ot::CheckpointRAII<StateFlags> startModification();

Q_SIGNALS:
	void filterChanged();
	void messageLimitChanged(int _limit);
	void useIntervalChaged();
	void removeOutdatedLogs(int _msSinceLog);

public Q_SLOTS:
	void slotUpdateCheckboxColors();

private Q_SLOTS:
	void slotFilterChanged();
	void slotSelectAllServices();
	void slotDeselectAllServices();
	void slotMessageLimitChanged();
	void slotIntervalChanged();
	void slotAutoRemovalActiveChanged();

	void slotRemoveOutdatedLogs();

private:
	void updateMessageLimitColor();
	void updateIntervalColor();
	void updateAutoRemovalColor();

	StateFlags m_state;

	QWidget* m_root;

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

	QTimer m_autoRemovalTimer;
	bool m_autoRemovalActive;
	ot::CheckBox* m_useAutoRemoval;
	ot::SpinBox* m_autoRemovalInterval;

	QComboBox* m_userFilter;
	QComboBox* m_sessionFilter;

	QListWidget* m_serviceFilter;
	
	QLineEdit* m_messageFilter;
};

OT_ADD_FLAG_FUNCTIONS(LoggingFilterView::State, LoggingFilterView::StateFlags)
// @otlicense
// File: DevLogger.h
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

#ifndef OTDEVLOGGER__H
#define OTDEVLOGGER__H

#include "OTCore/LogDispatcher.h"

//! @brief If set to 1 the developer logger will be used for the UI service (in debug and release mode)
//! @warning This should be used with care and 
#define OT_UISERVICE_DEV_LOGGER_ACTIVE 0

// #################################################################################################################################################################################################################

#if OT_UISERVICE_DEV_LOGGER_ACTIVE==0
#define OT_UISERVICE_DEV_LOGGER_INIT

#elif OT_UISERVICE_DEV_LOGGER_ACTIVE==1
#define OT_UISERVICE_DEV_LOGGER_INIT ot::intern::UiDevLogger::initialize()

#pragma message("The UIService DevLogger is used. Do not ship this build")

#include <QtWidgets/qplaintextedit.h>
#include <QtCore/qthread.h>

class QMainWindow;

namespace ot {

	class SimpleLogViewer : public QPlainTextEdit {
		Q_OBJECT
	public:
		SimpleLogViewer();
		virtual ~SimpleLogViewer();

		void appendLog(const ot::LogMessage& _message);

	private Q_SLOTS:
		void slotAppendLog(const ot::LogMessage& _message);

	private:
		Qt::HANDLE m_creatorThread;
	};


	namespace intern {

		class UiDevLogger : public ot::AbstractLogNotifier {
		public:
			static UiDevLogger& instance(void);
			static void initialize(void);

			virtual void log(const ot::LogMessage& _message) override;

		private:
			UiDevLogger();
			virtual ~UiDevLogger();

			QMainWindow* m_window;
			ot::SimpleLogViewer* m_viewer;
		};

	}
}

#endif // OT_UISERVICE_DEV_LOGGER_ACTIVE
#endif // OTDEVLOGGER__H

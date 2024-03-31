#ifndef OTDEVLOGGER__H
#define OTDEVLOGGER__H

#include "OTCore/Logger.h"

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

// @otlicense

//! @file LogDispatcher.h
//! @brief OpenTwin Logging system.
//! 
//! This file contains the Log functionallity, 
//! that any OpenTwin related class should use to generate their log messages.
//! 
//! @author Alexander Kuester (alexk95)
//! @date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogTypes.h"
#include "OTCore/LogMessage.h"

// std header
#include <list>
#include <string>

//! @brief OpenTwin log macros enabled
//! If undefined all the OT_LOG macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOGEnabled

//! @brief OpenTwin log information macros enabled
//! If undefined the OT_LOG_I macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOG_IEnabled

//! @brief OpenTwin log detailed information macros enabled
//! If undefined the OT_LOG_D macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOG_DEnabled

//! @brief OpenTwin log warning macros enabled
//! If undefined the OT_LOG_W macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOG_WEnabled

//! @brief OpenTwin log error macros enabled
//! If undefined the OT_LOG_E macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOG_EEnabled

//! @brief OpenTwin log test macros enabled
//! If undefined the OT_LOG_T macros wont generate any code (empty line)
#define OT_GLOBAL_LOGFLAG_LOG_TEnabled

#ifdef OT_GLOBAL_LOGFLAG_LOGEnabled

#ifdef _DEBUG

//! Log a message according to the service logger configuration and the provided flags
//! In debug mode every single log message will be dispatched.
//! In release mode only log messages with "enabled log flags" will be logged (See: LogDispatcher::dispatch(LogMessage))
//! @param ___text The log message text
//! @param ___flags LogFlags describing the type of the created log message
#define OT_LOG(___text, ___flags) ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags)

#else

//! Log a message according to the service logger configuration and the provided flags
//! In debug mode every single log message will be dispatched.
//! In release mode only log messages with "enabled log flags" will be logged (See: LogDispatcher::dispatch(LogMessage))
//! @param ___text The log message text
//! @param ___flags LogFlags describing the type of the created log message
#define OT_LOG(___text, ___flags) if (ot::LogDispatcher::mayLog(___flags)) { ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags); }

#endif

#ifdef OT_GLOBAL_LOGFLAG_LOG_IEnabled

//! @brief Log a information message according to the service logger configuration.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_I(___text) OT_LOG(___text, ot::INFORMATION_LOG)

//! @brief Log a information message according to the service logger configuration and OTAssert with the provided message.
//! Note that the provided text should be a C-String
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IA(___text) OTAssert(0, ___text); OT_LOG(___text, ot::INFORMATION_LOG)

//! @brief Log a information message according to the service logger configuration and assert.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IAS(___text) OTAssert(0, ""); OT_LOG(___text, ot::INFORMATION_LOG)

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_IEnabled

//! @brief Log a information message according to the service logger configuration.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_I(___text)

//! @brief Log a information message according to the service logger configuration and OTAssert with the provided message.
//! Note that the provided text should be a C-String
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IA(___text)

//! @brief Log a information message according to the service logger configuration and assert.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOG_IEnabled

#ifdef OT_GLOBAL_LOGFLAG_LOG_DEnabled

//! @brief Log a detailed information message according to the service logger configuration.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_D(___text) OT_LOG(___text, ot::DETAILED_LOG)

//! @brief Log a detailed information message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DA(___text) OTAssert(0, ___text); OT_LOG(___text, ot::DETAILED_LOG)

//! @brief Log a detailed information message according to the service logger configuration and assert.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DAS(___text) OTAssert(0, ""); OT_LOG(___text, ot::DETAILED_LOG)

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_DEnabled

//! @brief Log a detailed information message according to the service logger configuration.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_D(___text)

//! @brief Log a detailed information message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DA(___text)

//! @brief Log a detailed information message according to the service logger configuration and assert.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOG_DEnabled

#ifdef OT_GLOBAL_LOGFLAG_LOG_WEnabled

//! @brief Log a warning message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_W(___text) OT_LOG(___text, ot::WARNING_LOG)

//! @brief Log a warning message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_WA(___text) OTAssert(0, ___text); OT_LOG(___text, ot::WARNING_LOG)

//! @brief Log a warning message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_WAS(___text) OTAssert(0, ""); OT_LOG(___text, ot::WARNING_LOG)

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_WEnabled

//! @brief Log a warning message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_W(___text)

//! @brief Log a warning message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_WA(___text)

//! @brief Log a warning message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_WAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOG_WEnabled

#ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

//! @brief Log a error message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_E(___text) OT_LOG(___text, ot::ERROR_LOG)

//! @brief Log a error message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_EA(___text) OTAssert(0, ___text); OT_LOG(___text, ot::ERROR_LOG)

//! @brief Log a error message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_EAS(___text) OTAssert(0, ""); OT_LOG(___text, ot::ERROR_LOG)

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

//! @brief Log a error message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_E(___text)

//! @brief Log a error message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_EA(___text)

//! @brief Log a error message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_EAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

#ifdef OT_GLOBAL_LOGFLAG_LOG_TEnabled

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_T(___text) OT_LOG(___text, ot::TEST_LOG)

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_TA(___text) OTAssert(0, ___text); OT_LOG(___text, ot::TEST_LOG)

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_TAS(___text) OTAssert(0, ""); OT_LOG(___text, ot::TEST_LOG)

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_T(___text)

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_TA(___text)

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_TAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

#else // ifdef OT_GLOBAL_LOGFLAG_LOGEnabled

//! Log a message according to the service logger configuration and the provided flags
//! In debug mode every single log message will be dispatched.
//! In release mode only log messages with "enabled log flags" will be logged (See: LogDispatcher::dispatch(LogMessage))
//! @param ___text The log message text
//! @param ___flags LogFlags describing the type of the created log message
#define OT_LOG(___text, ___flags)

//! @brief Log a information message according to the service logger configuration.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_I(___text)

//! @brief Log a information message according to the service logger configuration and OTAssert with the provided message.
//! Note that the provided text should be a C-String
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IA(___text)

//! @brief Log a information message according to the service logger configuration and assert.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_IAS(___text)

//! @brief Log a detailed information message according to the service logger configuration.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_D(___text)

//! @brief Log a detailed information message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DA(___text)

//! @brief Log a detailed information message according to the service logger configuration and assert.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_DAS(___text)

//! @brief Log a warning message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_W(___text)

//! @brief Log a warning message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_WA(___text)

//! @brief Log a warning message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_WAS(___text)

//! @brief Log a error message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_E(___text)

//! @brief Log a error message according to the service logger configuration and otAssert with the provided message.
//! Note that the provided text should be a C-String
//! @param ___text The log message.
#define OT_LOG_EA(___text)

//! @brief Log a error message according to the service logger configuration and assert.
//! @param ___text The log message.
#define OT_LOG_EAS(___text)

#endif // ifdef OT_GLOBAL_LOGFLAG_LOGEnabled

#pragma warning (disable: 4251)

namespace ot {

	class AbstractLogNotifier;

	//! @brief The LogDispatcher dispatches generated log messages to all registered log notifiers.
	//! When a log message is dispatched a timestamp for log messages and then forwards them to any registered log notifier.
	class OT_CORE_API_EXPORT LogDispatcher {
	public:
		static LogDispatcher& instance(void);
		static LogDispatcher& initialize(const std::string& _serviceName, bool _addCoutReceiver = false);
		static inline bool mayLog(LogFlag _flags) { return ((_flags & LogDispatcher::instance().m_logFlags.data()) == _flags); };
		static void addFileWriter(void);

		// #################################################################################

		// Setter/Getter

		void setLogFlag(LogFlag _flag, bool _active = true) { m_logFlags.setFlag(_flag, _active); };
		void setLogFlags(LogFlags _flags) { m_logFlags = _flags; };
		LogFlags getLogFlags(void) const { return m_logFlags; };

		void setServiceName(const std::string& _name) { m_serviceName = _name; };
		const std::string& getServiceName(void) const { return m_serviceName; };

		void setUserName(const std::string& _name) { m_userName = _name; };
		const std::string& getUserName(void) const { return m_userName; };

		void setProjectName(const std::string& _name) { m_projectName = _name; };
		const std::string& getProjectName(void) const { return m_projectName; };

		//! @brief Adds the provided receiver to the message receiver list.
		//! Every registered receiver will be notified about every log message that was created.
		//! @param _receiver The receiver to add
		void addReceiver(AbstractLogNotifier* _receiver);
		void forgetReceiver(AbstractLogNotifier* _receiver);

		// #################################################################################

		//! @brief Dispatch a log message with the provided params.
		//! Will forward the created log message object to the dispatch(LogMessage) function.
		//! Will apply the current user and project names to the log message before dispatching.
		//! @param _text The message text.
		//! @param _functionName The name of the function.
		//! @param _logFlags The flags that should be set for the log message.
		void dispatch(const std::string& _text, const std::string& _functionName = std::string(), const LogFlags& _logFlags = LogFlags(ot::INFORMATION_LOG));

		//! @brief Dispatch a log message with the provided params.
		//! Will set the current system time as message creation time (local system time).
		//! Will apply the current user and project names to the log message before dispatching.
		//! @param _message The message to dispatch.
		void dispatch(const LogMessage& _message);

	private:
		void applyEnvFlag(const std::string& _str);

		LogFlags						m_logFlags;			//! @brief Allowed messages flags.
		std::string						m_serviceName;		//! @brief Service/Application name.

		std::list<AbstractLogNotifier*> m_messageReceiver;	//! @brief Receivers that will get notified about created log messages.

		std::string                     m_userName;
		std::string                     m_projectName;

		LogDispatcher();
		virtual ~LogDispatcher();

		LogDispatcher(LogDispatcher&) = delete;
		LogDispatcher& operator = (LogDispatcher&) = delete;
	};

}
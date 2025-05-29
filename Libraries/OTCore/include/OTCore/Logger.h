//! @file Logger.h
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
#include "OTSystem/OTAssert.h"
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <list>
#include <string>
#include <ostream>

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

#else // ifdef OT_GLOBAL_LOGFLAG_LOG_EEnabled

//! @brief Log a test message according to the service logger configuration.
//! @param ___text The log message.
#define OT_LOG_T(___text)

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

	//! @brief Log message verbouse level
	enum LogFlag {
		NO_LOG                          = 0 << 0, //! @brief No log flags.
		INFORMATION_LOG                 = 1 << 0, //! @brief Information log (few logs).
		DETAILED_LOG                    = 1 << 1, //! @brief Detailed log (more logs).
		WARNING_LOG                     = 1 << 2, //! @brief Warning log.
		ERROR_LOG                       = 1 << 3, //! @brief Error log.
		
		//! @brief Mask used to set all general log flags.
		ALL_GENERAL_LOG_FLAGS           = INFORMATION_LOG | DETAILED_LOG | WARNING_LOG | ERROR_LOG,

		INBOUND_MESSAGE_LOG             = 1 << 4, //! @brief Execute endpoint log.
		QUEUED_INBOUND_MESSAGE_LOG      = 1 << 5, //! @brief Queue endpoint log
		ONEWAY_TLS_INBOUND_MESSAGE_LOG  = 1 << 6, //! @brief OneWay-TLS endpoint log
		
		//! @brief Mask used to set all incoming message log flags.
		ALL_INCOMING_MESSAGE_LOG_FLAGS = INBOUND_MESSAGE_LOG | QUEUED_INBOUND_MESSAGE_LOG | ONEWAY_TLS_INBOUND_MESSAGE_LOG,
		
		OUTGOING_MESSAGE_LOG            = 1 << 7, //! \brief Outgoing message log.

		//! @brief Mask used to set all outgoing message log flags.
		ALL_OUTGOING_MESSAGE_LOG_FLAGS = OUTGOING_MESSAGE_LOG,

		TEST_LOG                        = 1 << 8,
		
		//! @brief Mask used to set all incoming and outgoing message log flags.
		ALL_MESSAGE_LOG_FLAGS           = ALL_INCOMING_MESSAGE_LOG_FLAGS | ALL_OUTGOING_MESSAGE_LOG_FLAGS,

		//! @brief Mask used to set all log flags.
		ALL_LOG_FLAGS                   = ALL_GENERAL_LOG_FLAGS | ALL_MESSAGE_LOG_FLAGS
	};

	typedef Flags<LogFlag> LogFlags;

	OT_CORE_API_EXPORT void addLogFlagsToJsonArray(const LogFlags& _flags, JsonArray& _flagsArray, JsonAllocator& _allocator);

	OT_CORE_API_EXPORT LogFlags logFlagsFromJsonArray(const ConstJsonArray& _flagsArray);

}
OT_ADD_FLAG_FUNCTIONS(ot::LogFlag)

namespace ot {

	//! \brief Contains information about the origin and the content of a log message.
	class OT_CORE_API_EXPORT LogMessage : public Serializable {
	public:
		LogMessage();
		LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const LogFlags& _flags = LogFlags(ot::INFORMATION_LOG));
		LogMessage(const LogMessage& _other);
		virtual ~LogMessage();

		LogMessage& operator = (const LogMessage& _other);

		void setServiceName(const std::string& _serviceName) { m_serviceName = _serviceName; };
		const std::string& getServiceName(void) const { return m_serviceName; };

		void setFunctionName(const std::string& _functionName) { m_functionName = _functionName; };
		const std::string& getFunctionName(void) const { return m_functionName; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setFlags(const LogFlags& _flags) { m_flags = _flags; };
		const LogFlags& getFlags(void) const { return m_flags; };

		//! @see getLocalSystemTime
		void setLocalSystemTime(const std::string& _timeString) { m_localSystemTime = _timeString; };

		//! @brief String representation of the system timestamp at message creation.
		//! @ref ot::DateTime::DateFormat::SimpleUTC
		const std::string& getLocalSystemTime(void) const { return m_localSystemTime; };

		//! @see getGlobalSystemTime
		void setGlobalSystemTime(const std::string& _timeString) { m_globalSystemTime = _timeString; };

		//! @brief String representation of the system timestamp at when the message was received by the Logger Service.
		//! @ref ot::DateTime::DateFormat::SimpleUTC
		const std::string& getGlobalSystemTime(void) const { return m_globalSystemTime; };

		//! @brief Set the current system time as message creation timestamp.
		void setCurrentTimeAsLocalSystemTime(void);

		//! @brief Set the current system time as message received by logger service timestamp.
		void setCurrentTimeAsGlobalSystemTime(void);

		void setUserName(const std::string& _userName) { m_userName = _userName; };
		const std::string& getUserName(void) const { return m_userName; };

		void setProjectName(const std::string& _projectName) { m_projectName = _projectName; };
		const std::string& getProjectName(void) const { return m_projectName; };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

	private:
		friend OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

		std::string m_serviceName;			//! @brief Name of the message creator.
		std::string m_functionName;			//! @brief Name of the function that created the message.
		std::string m_text;					//! @brief The message text.
		LogFlags    m_flags;				//! @brief Log flags tha describe the type of the message.
		std::string m_localSystemTime;		//! @brief Message creation timestamp.
		std::string m_globalSystemTime;		//! @brief Message received by LoggerService timestamp.
		std::string m_userName;             //! @brief Current user when this message was generated.
		std::string m_projectName;          //! @brief Project in which this message was generated.
	};

	//! @brief Writes the log message in a typical "log line" format to the provided output stream.
	//! @param _stream Output stream to write to.
	//! @param _msg The log message to write.
	OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

	OT_CORE_API_EXPORT std::string exportLogMessagesToString(const std::list<LogMessage>& _messages);

	OT_CORE_API_EXPORT bool importLogMessagesFromString(const std::string& _string, std::list<LogMessage>& _messages);

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	//! @brief Used to receive every log message that is generated.
	class OT_CORE_API_EXPORT AbstractLogNotifier {
	public:
		AbstractLogNotifier() : m_deleteLater(false) {};
		virtual ~AbstractLogNotifier() {};

		//! @brief Will set the delete later flag.
		//! If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
		void setDeleteLogNotifierLater(bool _deleteLater = true) { m_deleteLater = _deleteLater; };

		//! @brief Returns true if the delete later mode is set.
		bool getDeleteLogNotifierLater(void) const { return m_deleteLater; };

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) = 0;

	private:
		bool m_deleteLater; //! \brief If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
	};

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	//! @brief Used to write created log messages to std cout in a way a human could read it.
	class OT_CORE_API_EXPORT LogNotifierStdCout : public AbstractLogNotifier {
	public:
		virtual ~LogNotifierStdCout() {};

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;
	};

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	class OT_CORE_API_EXPORT LogNotifierFileWriter : public AbstractLogNotifier {
		OT_DECL_NODEFAULT(LogNotifierFileWriter)
		OT_DECL_NOCOPY(LogNotifierFileWriter)
	public:
		static std::string generateFileName(const std::string& _serviceName);

		LogNotifierFileWriter(const std::string& _filePath);
		virtual ~LogNotifierFileWriter();

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;

		void flushAndCloseStream(void);

		void closeStream(void);

	private:
		std::ofstream* m_stream;
	};

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

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
		LogFlags logFlags(void) const { return m_logFlags; };

		void setServiceName(const std::string& _name) { m_serviceName = _name; };
		const std::string& serviceName(void) const { return m_serviceName; };

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
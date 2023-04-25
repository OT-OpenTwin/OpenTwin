#pragma once

#include "OpenTwinCore/CoreAPIExport.h"

#include "OpenTwinCore/Flags.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Serializable.h"

#include <string>
#include <ostream>
#include <list>

#define OT_INTERN_LOG_TIME_FORMAT_STDSTRING "%Y-%m-%d %H:%M:%S"

#ifdef _DEBUG
//! Log a message according to the service logger configuration and the provided flags
//! In debug mode every single log message will be dispatched.
//! In release mode only log messages with "enabled log flags will be logged (See: LogDispatcher::dispatch(LogMessage))
//! @param ___text The log message text
//! @param ___flags LogFlags describing the type of the created log message
#define OT_LOG(___text, ___flags) ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags)

#else
//! Log a message according to the service logger configuration and the provided flags
//! In debug mode every single log message will be dispatched.
//! In release mode only log messages with "enabled log flags will be logged (See: LogDispatcher::dispatch(LogMessage))
//! @param ___text The log message text
//! @param ___flags LogFlags describing the type of the created log message
#define OT_LOG(___text, ___flags) if (ot::LogDispatcher::mayLog(___flags)) { ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags); }

#endif

//! Log a info message according to the service logger configuration
#define OT_LOG_I(___text) OT_LOG(___text, ot::DEFAULT_LOG)

//! Log a detailed information message according to the service logger configuration
#define OT_LOG_D(___text) OT_LOG(___text, ot::DETAILED_LOG)

//! Log a warning message according to the service logger configuration
#define OT_LOG_W(___text) OT_LOG(___text, ot::WARNING_LOG)

//! Log a error message according to the service logger configuration
#define OT_LOG_E(___text) OT_LOG(___text, ot::ERROR_LOG)

#pragma warning (disable: 4251)

namespace ot {

	//! Log message verbouse level
	enum LogFlag {
		NO_LOG                          = 0x00,
		DEFAULT_LOG                     = 0x01,
		DETAILED_LOG                    = 0x02,
		WARNING_LOG                     = 0x04,
		ERROR_LOG                       = 0x08,
		INBOUND_MESSAGE_LOG             = 0x10,
		QUEUED_INBOUND_MESSAGE_LOG      = 0x20,
		ONEWAY_TLS_INBOUND_MESSAGE_LOG  = 0x40,
		OUTGOING_MESSAGE_LOG            = 0x80,
		ALL_LOG_FLAGS                   = 0xFF
	};
}
OT_ADD_FLAG_FUNCTIONS(ot::LogFlag);

namespace ot {

	//! @brief Contains some information about the origin and the content of a log message
	class OT_CORE_API_EXPORT LogMessage : public Serializable {
	public:
		LogMessage();
		LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const ot::Flags<ot::LogFlag>& _flags = ot::Flags<ot::LogFlag>(ot::DEFAULT_LOG));
		LogMessage(const LogMessage& _other);
		virtual ~LogMessage();

		LogMessage& operator = (const LogMessage& _other);

		//! @brief Name of the service
		const std::string& serviceName(void) const { return m_serviceName; };

		//! @brief Name of the function
		const std::string& functionName(void) const { return m_functionName; };

		//! @brief Log message text
		const std::string& text(void) const { return m_text; };

		//! @brief Log flags that are set for this log message
		const ot::Flags<ot::LogFlag>& flags(void) const { return m_flags; };

		//! @brief "YYYY-MM-DD HH:MM:SS" string representation of the system timestamp at message creation
		const std::string& localSystemTime(void) const { return m_localSystemTime; };

		//! @brief "YYYY-MM-DD HH:MM:SS" string representation of the system timestamp at when the message was received by the Logger Service
		const std::string& globalSystemTime(void) const { return m_globalSystemTime; };

		//! @brief Set the current system time as message creation timestamp
		void setCurrentTimeAsLocalSystemTime(void);

		//! @brief Set the current system time as message received by logger service timestamp
		void setCurrentTimeAsGlobalSystemTime(void);


		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	private:
		friend OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

		std::string				m_serviceName;			//! @brief Name of the message creator
		std::string				m_functionName;			//! @brief Name of the function that created the message
		std::string				m_text;					//! @brief The message text
		ot::Flags<ot::LogFlag>	m_flags;				//! @brief Log flags tha describe the type of the message
		std::string				m_localSystemTime;		//! @brief Message creation timestamp
		std::string				m_globalSystemTime;		//! @brief Message received by LoggerService timestamp
	};

	//! @brief Writes the log message in a typical "log line" format to the provided output stream
	//! @param _stream Output stream to write to
	//! @param _msg The log message to write
	OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	//! @brief Used to receive every log message that is generated
	class OT_CORE_API_EXPORT AbstractLogNotifier {
	public:
		virtual ~AbstractLogNotifier() {};

		//! @brief Will set the delete later flag
		//! If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher
		void deleteLater(bool _deleteLater = true) { m_deleteLater = _deleteLater; };

		//! @brief Returns true if the delete later mode is set
		bool isDeleteLater(void) const { return m_deleteLater; };

		//! @brief Called when the a log message was created
		virtual void log(const LogMessage& _message) = 0;

	private:
		bool m_deleteLater; //! @brief If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher
	};

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	//! @brief Used to write created log messages to std cout in a way a human could read it
	class OT_CORE_API_EXPORT LogNotifierStdCout : public AbstractLogNotifier {
	public:
		virtual ~LogNotifierStdCout() {};

		//! @brief Called when the a log message was created
		inline virtual void log(const LogMessage& _message) override;
	};

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	// ######################################################################################################################################################

	//! @brief Creates a timestamp for log messages and then forwards them to any registered log notifier
	class OT_CORE_API_EXPORT LogDispatcher {
	public:
		static LogDispatcher& instance(void);
		static LogDispatcher& initialize(const std::string& _serviceName, bool _addCoutReceiver = false);
		static inline bool mayLog(const ot::Flags<ot::LogFlag>& _flags) { return ((_flags & LogDispatcher::instance().m_logFlags) == _flags); };

		// #################################################################################

		// Setter/Getter

		void setLogFlags(const Flags<ot::LogFlag>& _flags) { m_logFlags = _flags; };
		const Flags<ot::LogFlag>& logFlags(void) const { return m_logFlags; };

		void setServiceName(const std::string& _name) { m_serviceName = _name; };
		const std::string& serviceName(void) const { return m_serviceName; };

		//! @brief Adds the provided receiver to the message receiver list
		//! Every registered receiver will be notified about every log message that was created
		//! @param _receiver The receiver to add
		void addReceiver(AbstractLogNotifier* _receiver);

		// #################################################################################

		//! @brief Dispatch a log message with the provided params
		//! Will forward the created log message object to the dispatch(LogMessage) function
		//! @param _text The message thext
		//! @param _functionName The name of the function
		//! @param _logFlags The flags that should be set for the log message
		void dispatch(const std::string& _text, const std::string& _functionName = std::string(), const ot::Flags<ot::LogFlag>& _logFlags = ot::Flags<ot::LogFlag>(ot::DEFAULT_LOG));

		//! @brief Dispatch a log message with the provided params
		//! Will set the current system time as message creation time
		//! @param _message The message to dispatch
		void dispatch(const LogMessage& _message);

	private:
		Flags<LogFlag>					m_logFlags;			//! @brief Allowed messages flags
		std::string						m_serviceName;		//! @brief Service/Application name

		std::list<AbstractLogNotifier*> m_messageReceiver;	//! @brief Receivers that will get notified about created log messages

		LogDispatcher();
		virtual ~LogDispatcher();

		LogDispatcher(LogDispatcher&) = delete;
		LogDispatcher& operator = (LogDispatcher&) = delete;
	};

}
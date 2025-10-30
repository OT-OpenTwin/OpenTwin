// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	//! @brief Log message verbouse level
	enum LogFlag {
		NO_LOG = 0 << 0, //! @brief No log flags.
		INFORMATION_LOG = 1 << 0, //! @brief Information log (few logs).
		DETAILED_LOG = 1 << 1, //! @brief Detailed log (more logs).
		WARNING_LOG = 1 << 2, //! @brief Warning log.
		ERROR_LOG = 1 << 3, //! @brief Error log.

		//! @brief Mask used to set all general log flags.
		ALL_GENERAL_LOG_FLAGS = INFORMATION_LOG | DETAILED_LOG | WARNING_LOG | ERROR_LOG,

		INBOUND_MESSAGE_LOG = 1 << 4, //! @brief Execute endpoint log.
		QUEUED_INBOUND_MESSAGE_LOG = 1 << 5, //! @brief Queue endpoint log
		ONEWAY_TLS_INBOUND_MESSAGE_LOG = 1 << 6, //! @brief OneWay-TLS endpoint log

		//! @brief Mask used to set all incoming message log flags.
		ALL_INCOMING_MESSAGE_LOG_FLAGS = INBOUND_MESSAGE_LOG | QUEUED_INBOUND_MESSAGE_LOG | ONEWAY_TLS_INBOUND_MESSAGE_LOG,

		OUTGOING_MESSAGE_LOG = 1 << 7, //! @brief Outgoing message log.

		//! @brief Mask used to set all outgoing message log flags.
		ALL_OUTGOING_MESSAGE_LOG_FLAGS = OUTGOING_MESSAGE_LOG,

		TEST_LOG = 1 << 8,

		//! @brief Mask used to set all incoming and outgoing message log flags.
		ALL_MESSAGE_LOG_FLAGS = ALL_INCOMING_MESSAGE_LOG_FLAGS | ALL_OUTGOING_MESSAGE_LOG_FLAGS,

		//! @brief Mask used to set all log flags.
		ALL_LOG_FLAGS = ALL_GENERAL_LOG_FLAGS | ALL_MESSAGE_LOG_FLAGS
	};

	typedef Flags<LogFlag> LogFlags;

	OT_CORE_API_EXPORT void addLogFlagsToJsonArray(const LogFlags& _flags, JsonArray& _flagsArray, JsonAllocator& _allocator);

	OT_CORE_API_EXPORT LogFlags logFlagsFromJsonArray(const ConstJsonArray& _flagsArray);

}
OT_ADD_FLAG_FUNCTIONS(ot::LogFlag)

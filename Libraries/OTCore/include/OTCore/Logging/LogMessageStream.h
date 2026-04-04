// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogMessage.h"

// std header
#include <sstream>

namespace ot {

	class OT_CORE_API_EXPORT LogMessageStream {
		OT_DECL_NOCOPY(LogMessageStream)
		OT_DECL_NOMOVE(LogMessageStream)
		OT_DECL_NODEFAULT(LogMessageStream)
	public:
		LogMessageStream(const LogMessage& _initialMessage);
		LogMessageStream(LogMessage&& _initialMessage);
		~LogMessageStream() = default;

		//! @brief Return the current log message.
		LogMessage getLogMessage() const;

		LogMessageStream& operator << (bool _value);
		LogMessageStream& operator << (const char* _plainText);
		LogMessageStream& operator << (const std::string& _plainText);
		LogMessageStream& operator << (int32_t _value);
		LogMessageStream& operator << (uint32_t _value);
		LogMessageStream& operator << (int64_t _value);
		LogMessageStream& operator << (uint64_t _value);

		LogMessageStream& operator << (const Serializable* _serializableObject);

	private:
		std::stringstream m_stream;
		LogMessage m_message;

	};

}
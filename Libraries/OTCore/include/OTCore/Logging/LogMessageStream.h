// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogMessage.h"

// std header
#include <sstream>

namespace ot {

	class OT_CORE_API_EXPORT LogMessageStream {
		OT_DECL_NOCOPY(LogMessageStream)
		OT_DECL_DEFMOVE(LogMessageStream)
		OT_DECL_NODEFAULT(LogMessageStream)
	public:
		struct Pointer
		{
			OT_DECL_DEFCOPY(Pointer)
			OT_DECL_DEFMOVE(Pointer)
			OT_DECL_NODEFAULT(Pointer)

			explicit Pointer(const void* _ptr) : ptr(_ptr) {}

			const void* ptr;
		};

		explicit LogMessageStream(const std::string& _functionName, const LogFlags& _flags);
		~LogMessageStream() = default;

		LogMessageStream& operator << (bool _value);
		LogMessageStream& operator << (char _character);
		LogMessageStream& operator << (const char* const _plainText);
		LogMessageStream& operator << (const std::string& _plainText);
		LogMessageStream& operator << (int32_t _value);
		LogMessageStream& operator << (uint32_t _value);
		LogMessageStream& operator << (int64_t _value);
		LogMessageStream& operator << (uint64_t _value);
		LogMessageStream& operator << (float _value);
		LogMessageStream& operator << (double _value);

		LogMessageStream& operator << (const Serializable* const _serializableObject);
		LogMessageStream& operator << (const Pointer& _pointer);

		std::string getText() const;
		const std::string& getFunctionName() const { return m_functionName; };
		const LogFlags& getFlags() const { return m_logFlags; };

	private:
		std::stringstream m_stream;
		std::string m_functionName;
		LogFlags m_logFlags;

	};

}
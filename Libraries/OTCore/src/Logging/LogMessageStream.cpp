// @otlicense

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logging/LogMessageStream.h"

// std header
#include <iomanip>

ot::LogMessageStream::LogMessageStream(const std::string& _functionName, const LogFlags& _flags)
	: m_functionName(_functionName), m_logFlags(_flags)
{}

std::string ot::LogMessageStream::getText() const
{
	return m_stream.str();
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(bool _value)
{
	m_stream << (_value ? "true" : "false");
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(char _character)
{
	m_stream << _character;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const char* const _plainText)
{
	m_stream << _plainText;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(int32_t _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(uint32_t _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(int64_t _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(uint64_t _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(float _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(double _value)
{
	m_stream << _value;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::string& _plainText)
{
	m_stream << _plainText;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const Serializable* const _serializableObject) {
	JsonDocument doc;
	_serializableObject->addToJsonObject(doc, doc.GetAllocator());
	m_stream << doc.toJson();
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const Pointer& _pointer)
{
	m_stream << "0x" << String::ptrToHexString(_pointer.ptr);
	return *this;
}

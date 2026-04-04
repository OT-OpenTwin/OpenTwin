// @otlicense

// OpenTwin header
#include "OTCore/Logging/LogMessageStream.h"

ot::LogMessageStream::LogMessageStream(const LogMessage& _initialMessage)
	: m_message(_initialMessage)
{}

ot::LogMessageStream::LogMessageStream(LogMessage&& _initialMessage) 
	: m_message(std::move(_initialMessage))
{}

ot::LogMessage ot::LogMessageStream::getLogMessage() const
{
	LogMessage msg(m_message);
	msg.setText(m_message.getText() + m_stream.str());
	return msg;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(bool _value)
{
	m_stream << (_value ? "true" : "false");
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const char* _plainText)
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

ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::string& _plainText)
{
	m_stream << _plainText;
	return *this;
}

ot::LogMessageStream& ot::LogMessageStream::operator<<(const Serializable * _serializableObject) {
	JsonDocument doc;
	_serializableObject->addToJsonObject(doc, doc.GetAllocator());
	m_stream << doc.toJson();
	return *this;
}
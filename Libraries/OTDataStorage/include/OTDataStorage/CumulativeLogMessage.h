//@ otlicense
#pragma once
#include <string>
class CumulativeLogMessage
{
public:
	void addLineToMessage(const std::string& line) { m_message += m_message; }
	const std::string& getMessage() const { return m_message; }
	void clearMessage() { m_message.clear(); }
private:
	std::string m_message;

};

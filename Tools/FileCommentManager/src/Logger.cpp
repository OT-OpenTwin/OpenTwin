// Project header
#include "Logger.h"

// std header
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>

void Logger::logInfo(const QString& _sender, const QString& _message) {
	Logger::instance().log("", _sender, _message);
}

void Logger::logWarning(const QString& _sender, const QString& _message) {
	Logger::instance().log("[WARNING] ", _sender, _message);
}

void Logger::logError(const QString& _sender, const QString& _message) {
	Logger::instance().log("[ERROR] ", _sender, _message);
}

void Logger::setSilent(bool _isSilent) {
	Logger::instance().m_isSilent = _isSilent;
}

void Logger::log(const QString& _modeString, const QString& _sender, const QString& _message) {
	if (m_isSilent) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	*m_stream << "[";
	this->printCurrentTimestamp();
	*m_stream << "] " << "[" << _sender.toStdString() << "] " << _modeString.toStdString() << _message.toStdString() << std::endl;
	m_stream->flush();
}

void Logger::printCurrentTimestamp(void) {
	// Get the current time
	auto now = std::chrono::system_clock::now();

	// Convert to time_t to get calendar time
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	// Convert to milliseconds
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	// Create a tm structure to hold the local time
	struct tm timeinfo;
	localtime_s(&timeinfo, &in_time_t);  // Using localtime_s for thread safety

	// Create a string stream to format the output
	std::stringstream ss;

	// Format the time part
	ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");

	// Add milliseconds
	ss << ":" << std::setw(3) << std::setfill('0') << ms.count();

	*m_stream << ss.str();
}

Logger::Logger() 
	: m_isSilent(false), m_stream(&std::cout)
{

}

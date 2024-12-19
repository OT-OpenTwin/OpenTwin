#pragma once

// Qt header
#include <QtCore/qstring.h>

// std header
#include <mutex>
#include <ostream>

#define LOG_INFO(___sender, ___message) if (!Logger::isSilent()) { Logger::logInfo(___sender, ___message); }
#define LOG_WARNING(___sender, ___message) if (!Logger::isSilent()) { Logger::logWarning(___sender, ___message); }
#define LOG_ERROR(___sender, ___message) if (!Logger::isSilent()) { Logger::logError(___sender, ___message); }

class Logger {
public:
	static void logInfo(const QString& _sender, const QString& _message);
	static void logWarning(const QString& _sender, const QString& _message);
	static void logError(const QString& _sender, const QString& _message);
	static bool isSilent(void) { return Logger::instance().m_isSilent; };

	static void setSilent(bool _isSilent);

private:
	static Logger& instance(void) {
		static Logger g_instance;
		return g_instance;
	}

	void log(const QString& _modeString, const QString& _sender, const QString& _message);
	void printCurrentTimestamp(void);

	Logger();
	~Logger() {};

	std::mutex m_mutex;
	std::ostream* m_stream;
	bool m_isSilent;

	Logger(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator = (const Logger&) = delete;
	Logger& operator = (Logger&&) = delete;
};
// @otlicense

#pragma once
#include "FileWriter.h"
#include <assert.h>

class Logger 
{
public:
	static void Init(const std::string _loggingPath)
	{
		m_loggingPath = _loggingPath;
	}
	static FileWriter& INSTANCE()
	{
		assert(m_loggingPath != "");
		static FileWriter logger(m_loggingPath, true);
		return logger;
	}

private:
	Logger() = default;
	static std::string m_loggingPath;
};

// @otlicense

#include "stdafx.h"
#include "DataStorageLogger.h"
#include "..\include\DataStorageLogger.h"

namespace DataStorageAPI
{
	void DataStorageLogger::operator()(mongocxx::log_level level, bsoncxx::stdx::string_view domain, 
		bsoncxx::stdx::string_view message) noexcept
	{
		*_stream << '[' << mongocxx::to_string(level) << '-' << domain << "] " << message << '\n';
	}
}

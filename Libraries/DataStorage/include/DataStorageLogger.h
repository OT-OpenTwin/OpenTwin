// @otlicense

#pragma once
#include <iostream>
#include <ostream>
#include <mongocxx/logger.hpp>

namespace DataStorageAPI
{
	class DataStorageLogger final : public mongocxx::logger
	{
	public:
		explicit DataStorageLogger(std::ostream* stream): _stream(stream) {}

		void operator()(mongocxx::log_level level,
			bsoncxx::stdx::string_view domain,
			bsoncxx::stdx::string_view message) noexcept override;

	private:
		std::ostream* const _stream;
	};
}


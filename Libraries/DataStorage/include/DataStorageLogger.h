// @otlicense
// File: DataStorageLogger.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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


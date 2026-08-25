// @otlicense
// File: RuntimeTests.h
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

// OpenTwin header
#include "OTCore/RAII/RAIIBase.h"
#include "OTCore/GlobalTestingFlags.h"

// std header
#include <string>
#include <chrono>

#pragma warning(disable:4251)

namespace ot
{

	class OT_CORE_API_EXPORT RuntimeIntervalTestLog : public RAIIBase
	{
		OT_DECL_NOCOPY(RuntimeIntervalTestLog)
	public:
		explicit RuntimeIntervalTestLog(const std::string& _prefix = std::string("took "), const std::string& _suffix = std::string());
		RuntimeIntervalTestLog(RuntimeIntervalTestLog&& _other) noexcept;
		~RuntimeIntervalTestLog();

		RuntimeIntervalTestLog& operator=(RuntimeIntervalTestLog&& _other) noexcept = delete;

		void setPrefix(const std::string& _prefix) { m_prefix = _prefix; };
		void setSuffix(const std::string& _suffix) { m_suffix = _suffix; };

		//! @brief Returns the interval in ms from creation of this instance.
		uint64_t currentRuntime() const;

		std::string currentRuntimeString() const;

		virtual void execute() override;
	protected:
		virtual void invalidate() override;

	private:
		std::string m_prefix;
		std::string m_suffix;
		uint64_t m_startTime;
	};

}
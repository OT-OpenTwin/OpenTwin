// @otlicense
// File: LogNotifierFileWriter.h
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
#include "OTCore/AbstractLogNotifier.h"

// std header
#include <mutex>

namespace ot {

	class OT_CORE_API_EXPORT LogNotifierFileWriter : public AbstractLogNotifier {
		OT_DECL_NODEFAULT(LogNotifierFileWriter)
		OT_DECL_NOCOPY(LogNotifierFileWriter)
	public:
		static std::string generateFileName(const std::string& _serviceName);

		LogNotifierFileWriter(const std::string& _filePath);
		virtual ~LogNotifierFileWriter();

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;

		void flushAndCloseStream(void);

		void closeStream(void);

	private:
		std::mutex m_mutex;
		std::ofstream* m_stream;
	};

}
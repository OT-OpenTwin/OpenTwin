// @otlicense
// File: DateTime.h
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
#include "OTSystem/SystemAPIExport.h"

// std header
#include <string>

namespace ot {

    //! @brief Convenience methods for date and time operations.
    class OT_SYS_API_EXPORT DateTime {
    public:

		struct Time
		{
			std::string m_year = "";
			std::string m_month = "";
			std::string m_day = "";
			std::string m_hour = "";
			std::string m_minute = "";
			std::string m_second = "";
			std::string m_millisec = "";
		};

        //! @brief Date format.
        enum DateFormat {
            Simple,     //! @brief Simple format, current timezone ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            SimpleUTC,  //! @brief Simple format, UTC time ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            ISO8601UTC, //! @brief ISO 8601, UTC time ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000Z").
            RFC3339,    //! @brief ISO 8601, explicit timezone ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000+00:00").
			Msec        //! @brief Milliseconds since epoch, UTC time.
        };

        //! @brief Returns the current time as a string with the given format.
		//! @param _format Format of the timestamp string.
        static std::string currentTimestamp(DateFormat _format);
		
		static ot::DateTime::Time currentTimestampAsStruct();

        //! @brief Current milliseconds since epoch.
        static int64_t msSinceEpoch();

		//! @brief Converts milliseconds since epoch to a timestamp string.
		//! @param _msecSinceEpoch Milliseconds since epoch to convert.
		//! @param _format Format of the timestamp string.
        static std::string timestampFromMsec(int64_t _msecSinceEpoch, DateFormat _format);

		//! @brief Converts a timestamp string to milliseconds since epoch.
		//! @param _timestamp Timestamp string to convert.
		//! @param _format Format of the timestamp string.
		static int64_t timestampToMsec(const std::string& _timestamp, DateFormat _format);

		//! @brief Converts a time interval in milliseconds to a human-readable string.
		//! @param _msecInterval Time interval in milliseconds.
		static std::string intervalToString(int64_t _msecInterval);

    };

}
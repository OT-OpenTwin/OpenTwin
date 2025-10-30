// @otlicense
// File: DateTime.cpp
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

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <regex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace ot {
    namespace intern {
        std::string toTimeStamp(std::chrono::system_clock::time_point _timePoint, ot::DateTime::DateFormat _format) {
            auto timeT = std::chrono::system_clock::to_time_t(_timePoint);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timePoint.time_since_epoch()) % 1000;

            std::tm timeStruct;
            int timezoneOffset = 0; // Offset in minutes

            if (_format == ot::DateTime::RFC3339 || _format == ot::DateTime::Simple) {
                // Convert to local time
#ifdef OT_OS_WINDOWS
                localtime_s(&timeStruct, &timeT);
#else
                localtime_r(&timeT, &timeStruct);
#endif
                // Get timezone offset
                std::time_t localNow = std::mktime(&timeStruct);
                std::time_t utcNow = std::mktime(std::gmtime(&timeT)); // Convert to UTC
                timezoneOffset = static_cast<int>(std::difftime(localNow, utcNow) / 60);
            }
            else {
                // Convert to UTC time
#ifdef OT_OS_WINDOWS
                gmtime_s(&timeStruct, &timeT);
#else
                gmtime_r(&timeStruct, &timeT);
#endif
            }

            // Format timestamp
            std::ostringstream oss;

            switch (_format) {
            case ot::DateTime::Simple:
            case ot::DateTime::SimpleUTC:
                oss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
                break;

            case ot::DateTime::ISO8601UTC:
                oss << std::put_time(&timeStruct, "%Y-%m-%dT%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count() << "Z";
                break;

            case ot::DateTime::RFC3339:
            {
                int hours = timezoneOffset / 60;
                int minutes = std::abs(timezoneOffset % 60);

                oss << std::put_time(&timeStruct, "%Y-%m-%dT%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();

                oss << (hours >= 0 ? "+" : "-")
                    << std::setfill('0') << std::setw(2) << std::abs(hours) << ":"
                    << std::setfill('0') << std::setw(2) << minutes;
            }
            break;

            case ot::DateTime::Msec:
                oss << std::chrono::duration_cast<std::chrono::milliseconds>(_timePoint.time_since_epoch()).count();
                break;

            default:
                throw std::invalid_argument("Unknown DateFormat");
            }

            return oss.str();
        }
    }
}

std::string ot::DateTime::currentTimestamp(DateFormat _format) {
    auto now = std::chrono::system_clock::now();
	return intern::toTimeStamp(now, _format);
}

int64_t ot::DateTime::msSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string ot::DateTime::timestampFromMsec(int64_t msecSinceEpoch, DateFormat format) {
    auto tp = std::chrono::system_clock::time_point(std::chrono::milliseconds(msecSinceEpoch));
	return intern::toTimeStamp(tp, format);
}

int64_t ot::DateTime::timestampToMsec(const std::string& _timestamp, DateFormat _format) {
    if (_format == Msec) {
        // Direct conversion
        return std::stoll(_timestamp);
    }

    std::tm timeStruct = {};
    int milliseconds = 0;
    int tzOffsetMinutes = 0; // only for RFC3339

    switch (_format) {
    case Simple:
    case SimpleUTC:
    {
        // Format: "yyyy-mm-dd hh:MM:ss.zzz"
        std::istringstream ss(_timestamp.substr(0, 19));
        ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse timestamp");
        }

        // parse milliseconds (after '.')
        if (_timestamp.size() > 20 && _timestamp[19] == '.') {
            milliseconds = std::stoi(_timestamp.substr(20, 3));
        }

        // Interpret as UTC
#if defined(_WIN32)
        time_t t = _mkgmtime(&timeStruct);
#else
        time_t t = timegm(&timeStruct);
#endif
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(t).time_since_epoch()).count() + milliseconds;
    }

    case ISO8601UTC:
    {
        // Format: "yyyy-mm-ddThh:MM:ss.zzzZ"
        std::istringstream ss(_timestamp.substr(0, 19));
        ss >> std::get_time(&timeStruct, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse timestamp");
        }

        if (_timestamp.size() > 20 && _timestamp[19] == '.') {
            milliseconds = std::stoi(_timestamp.substr(20, 3));
        }

        // Must end with 'Z' (UTC)
#if defined(_WIN32)
        time_t t = _mkgmtime(&timeStruct);
#else
        time_t t = timegm(&timeStruct);
#endif
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(t).time_since_epoch()).count() + milliseconds;
    }

    case RFC3339:
    {
        // Example: "2000-01-01T00:00:00.000+02:00"
        // Split date/time and timezone
        std::smatch match;
        std::regex re(R"(^(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})(?:\.(\d{3}))?([+-]\d{2}:\d{2}|Z)$)");
        if (!std::regex_match(_timestamp, match, re)) {
            throw std::runtime_error("Invalid RFC3339 timestamp: " + _timestamp);
        }

        // base time
        std::istringstream ss(match[1].str());
        ss >> std::get_time(&timeStruct, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse timestamp");
        }

        if (match[2].matched) {
            milliseconds = std::stoi(match[2].str());
        }

        std::string tzStr = match[3].str();
        if (tzStr != "Z") {
            int hours = std::stoi(tzStr.substr(0, 3));   // +02 or -05
            int minutes = std::stoi(tzStr.substr(4, 2));
            tzOffsetMinutes = hours * 60 + (hours >= 0 ? minutes : -minutes);
        }

        // Convert to UTC
#if defined(_WIN32)
        time_t t = _mkgmtime(&timeStruct);
#else
        time_t t = timegm(&timeStruct);
#endif
        auto baseMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(t).time_since_epoch()).count() + milliseconds;

        // Adjust by timezone offset (in minutes)
        return baseMs - tzOffsetMinutes * 60 * 1000LL;
    }

    default:
        throw std::invalid_argument("Unknown DateFormat");
    }
}

std::string ot::DateTime::intervalToString(int64_t _msecInterval) {
    if (_msecInterval < 0) {
        return "-" + DateTime::intervalToString(-_msecInterval);
    }

    int64_t msec = _msecInterval % 1000;
    int64_t totalSeconds = _msecInterval / 1000;

    int64_t seconds = totalSeconds % 60;
    int64_t totalMinutes = totalSeconds / 60;

    int64_t minutes = totalMinutes % 60;
    int64_t totalHours = totalMinutes / 60;

    int64_t hours = totalHours % 24;
    int64_t days = totalHours / 24;

    std::ostringstream oss;
    
    if (days > 0) {
        oss << days;
        if (days == 1) {
            oss << " day ";
        }
        else {
            oss << " days ";
		}
    }

    if (hours > 0 || days > 0) {
        oss << hours;
        if (hours == 1) {
            oss << " hour ";
        }
        else {
            oss << " hours ";
        }
    }

    if (minutes > 0 || hours > 0 || days > 0) {
        oss << minutes;
        if (minutes == 1) {
            oss << " minute ";
        }
        else {
            oss << " minutes ";
		}
    }
    
    oss << seconds;
    if (msec > 0) {
        oss << "." << msec << " seconds";
    }
    else if (seconds == 1) {
        oss << " second";
    }
    else {
		oss << " seconds";
    }

    return oss.str();
}

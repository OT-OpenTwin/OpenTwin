//! @file DateTime.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
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

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
        //! @brief Date format.
        enum DateFormat {
            Simple,     //! @brief Simple format, current timezone ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            SimpleUTC,  //! @brief Simple format, UTC time ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            ISO8601UTC, //! @brief ISO 8601, UTC time ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000Z").
            RFC3339,    //! @brief ISO 8601, explicit timezone ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000+00:00").
			Msec        //! @brief Milliseconds since epoch, UTC time.
        };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static methods

        //! @brief Returns the current time as a string with the given format.
		//! @param _format Format of the timestamp string.
        static std::string currentTimestamp(DateFormat _format);
		
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

		//! @brief Get current date and time.
		//! @param _useLocalTime If true local time is used, otherwise UTC time is used.
		static DateTime current(bool _useLocalTime = false);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		inline constexpr DateTime() noexcept = default;
		inline constexpr DateTime(int _year, int _month, int _day, int _hour = 0, int _minute = 0, int _second = 0, int _millisec = 0) noexcept :
			m_year(_year), m_month(_month), m_day(_day), m_hour(_hour), m_minute(_minute), m_second(_second), m_millisec(_millisec) {
		};
		inline constexpr DateTime(const DateTime& _other) noexcept = default;
		inline constexpr DateTime(DateTime&& _other) noexcept = default;
		inline ~DateTime() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		inline constexpr DateTime& operator=(const DateTime& _other) noexcept = default;
		inline constexpr DateTime& operator=(DateTime&& _other) noexcept = default;

		inline constexpr bool operator==(const DateTime& _other) const noexcept {
			return m_year == _other.m_year &&
				m_month == _other.m_month &&
				m_day == _other.m_day &&
				m_hour == _other.m_hour &&
				m_minute == _other.m_minute &&
				m_second == _other.m_second &&
				m_millisec == _other.m_millisec;
		};

		inline constexpr bool operator!=(const DateTime& _other) const noexcept { return !(*this == _other); };

		inline constexpr bool operator<(const DateTime& _other) const noexcept {
			if (m_year != _other.m_year) {
				return m_year < _other.m_year;
			}
			else if (m_month != _other.m_month) {
				return m_month < _other.m_month;
			}
			else if (m_day != _other.m_day) {
				return m_day < _other.m_day;
			}
			else if (m_hour != _other.m_hour) {
				return m_hour < _other.m_hour;
			}
			else if (m_minute != _other.m_minute) {
				return m_minute < _other.m_minute;
			}
			else if (m_second != _other.m_second) {
				return m_second < _other.m_second;
			}
			else {
				return m_millisec < _other.m_millisec;
			}
		};

		inline constexpr bool operator<=(const DateTime& _other) const noexcept { return (*this < _other) || (*this == _other); };
		inline constexpr bool operator>(const DateTime& _other) const noexcept { return !(*this <= _other); };
		inline constexpr bool operator>=(const DateTime& _other) const noexcept { return !(*this < _other); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline bool isValid() const noexcept {
			if (m_month < 1 || m_month > 12) 
			{
				return false;
			}
			int dim = daysInMonth(m_year, m_month);
			if (m_day < 1 || m_day > dim) 
			{
				return false;
			}
			if (m_hour < 0 || m_hour > 23) 
			{
				return false;
			}
			if (m_minute < 0 || m_minute > 59) 
			{
				return false;
			}
			if (m_second < 0 || m_second > 59) 
			{
				return false;
			}
			if (m_millisec < 0 || m_millisec > 999) 
			{
				return false;
			}
			return true;
		};

		inline bool isLeapYear() const noexcept { return isLeapYear(m_year); };

		inline void setYear(int _year) noexcept { m_year = _year; };
		inline constexpr int getYear() const noexcept { return m_year; };

		inline void setMonth(int _month) noexcept { m_month = _month; };
		inline constexpr int getMonth() const noexcept { return m_month; };

		inline void setDay(int _day) noexcept { m_day = _day; };
		inline constexpr int getDay() const noexcept { return m_day; };

		inline void setHour(int _hour) noexcept { m_hour = _hour; };
		inline constexpr int getHour() const noexcept { return m_hour; };

		inline void setMinute(int _minute) noexcept { m_minute = _minute; };
		inline constexpr int getMinute() const noexcept { return m_minute; };

		inline void setSecond(int _second) noexcept { m_second = _second; };
		inline constexpr int getSecond() const noexcept { return m_second; };

		inline void setMillisecond(int _millisec) noexcept { m_millisec = _millisec; };
		inline constexpr int getMillisecond() const noexcept { return m_millisec; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Increment / Decrement

		inline void addYears(int _years) noexcept { m_year += _years; };
		inline void removeYears(int _years) noexcept { m_year -= _years; };

		inline void addMonths(int _months) noexcept { m_month += _months; normalizeDay(); };
		inline void removeMonths(int _months) noexcept { addMonths(-_months); };

		inline void addDays(int _days) noexcept { m_day += _days; normalizeDay(); };
		inline void removeDays(int _days) noexcept { addDays(-_days); };

		inline void addHours(int _hours) noexcept { m_hour += _hours; normalizeTime(); };
		inline void removeHours(int _hours) noexcept { addHours(-_hours); };

		inline void addMinutes(int _minutes) noexcept { m_minute += _minutes; normalizeTime(); };
		inline void removeMinutes(int _minutes) noexcept { addMinutes(-_minutes); };

		inline void addSeconds(int _seconds) noexcept { m_second += _seconds; normalizeTime(); };
		inline void removeSeconds(int _seconds) noexcept { addSeconds(-_seconds); };

		inline void addMilliseconds(int _ms) noexcept { m_millisec += _ms; normalizeTime(); };
		inline void removeMilliseconds(int _ms) noexcept { addMilliseconds(-_ms); };
		
	private:
		int m_year = 0;
		int m_month = 0;
		int m_day = 0;
		int m_hour = 0;
		int m_minute = 0;
		int m_second = 0;
		int m_millisec = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		//! @brief Checks if the given year is a leap year.
		inline bool isLeapYear(int _year) const noexcept {
			return ((_year % 4 == 0) && (_year % 100 != 0)) || (_year % 400 == 0);
		}

		//! @brief Returns the number of days in the given month of the given year.
		inline int daysInMonth(int _year, int _month) const noexcept {
			static const int mdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

			if (_month == 2) {
				return isLeapYear(_year) ? 29 : 28;
			}
			else {
				return mdays[_month - 1];
			}
		}

		//! @brief Normalizes month with year overflow.
		inline void normalizeMonth() noexcept {
			if (m_month <= 0 || m_month > 12) {
				int yshift = (m_month - 1) / 12;
				if (m_month < 1) {
					yshift = (m_month - 12) / 12;
				}
				m_year += yshift;
				m_month -= yshift * 12;
			}
		}

		//! @brief Normalizes day with month and year overflow.
		inline void normalizeDay() noexcept {
			normalizeMonth();

			int dim = daysInMonth(m_year, m_month);

			while (m_day > dim) {
				m_day -= dim;
				m_month += 1;
				normalizeMonth();
				dim = daysInMonth(m_year, m_month);
			}
			while (m_day <= 0) {
				m_month -= 1;
				normalizeMonth();
				dim = daysInMonth(m_year, m_month);
				m_day += dim;
			}
		}

		//! @brief Normalizes time with date overflow.
		inline void normalizeTime() noexcept {
			// Milliseconds
			if (m_millisec < 0 || m_millisec >= 1000) {
				int secShift = m_millisec / 1000;
				if (m_millisec < 0) {
					secShift = (m_millisec - 999) / 1000;
				}

				m_second += secShift;
				m_millisec -= secShift * 1000;
			}

			// Seconds
			if (m_second < 0 || m_second >= 60) {
				int minShift = m_second / 60;
				if (m_second < 0) {
					minShift = (m_second - 59) / 60;
				}

				m_minute += minShift;
				m_second -= minShift * 60;
			}

			// Minutes
			if (m_minute < 0 || m_minute >= 60) {
				int hourShift = m_minute / 60;
				if (m_minute < 0) {
					hourShift = (m_minute - 59) / 60;
				}

				m_hour += hourShift;
				m_minute -= hourShift * 60;
			}

			// Hours
			if (m_hour < 0 || m_hour >= 24) {
				int dayShift = m_hour / 24;
				if (m_hour < 0) {
					dayShift = (m_hour - 23) / 24;
				}

				m_day += dayShift;
				m_hour -= dayShift * 24;
			}

			// Date rollover
			normalizeDay();
		}
    };

}
/*
 *	File:		aTime.h
 *	Package:	akCore
 *
 *  Created on: July 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // uiCore header
#include <akCore/akCore.h>
#include <akCore/globalDataTypes.h>

// QT header
#include <qstring.h>
#include <qdatetime.h>

// C++ header
#include <string>

namespace ak {

	class UICORE_API_EXPORT aTime {
	public:
		aTime();
		aTime(int _h, int _m, int _s = 0, int _ms = 0);
		aTime(const QTime & _time);
		aTime(const aTime & _other);
		virtual ~aTime();

		// ############################################################################################################

		// Getter

		//! @brief Returns the current hour
		int hour(void) const { return m_h; }

		//! @brief Returns the current minute
		int minute(void) const { return m_m; }

		//! @brief Returns the current second
		int second(void) const { return m_s; }

		//! @brief Returns the current millisecond
		int msec(void) const { return m_ms; }

		//! @brief Will return a String representation of this time
		//! @param _delimiter The delimiter that is used to seperate the hour, minute, second and millisecond
		//! @param _timeFormat The timeFormat of the created string
		std::string toString(const std::string _delimiter = ":", timeFormat _timeFormat = tfHHMM) const;

		//! @brief Will return a String representation of this time
		//! @param _delimiter The delimiter that is used to seperate the hour, minute, second and millisecond
		//! @param _timeFormat The timeFormat of the created string
		std::wstring toWString(const std::wstring _delimiter = L":", timeFormat _timeFormat = tfHHMM) const;

		//! @brief Will return a String representation of this time
		//! @param _delimiter The delimiter that is used to seperate the hour, minute, second and millisecond
		//! @param _timeFormat The timeFormat of the created string
		QString toQString(const QString _delimiter = ":", timeFormat _timeFormat = tfHHMM) const;

		//! @brief Will return the time
		QTime toQTime(void) const;

		//! @brief Will return the difference to the the other time
		//! @param _other The other time
		aTime difference(const aTime & _other);

		//! @brief Will return the difference to the the other time
		//! @param _other The other time
		aTime difference(const QTime & _other);

		// ############################################################################################################

		// Setter

		//! @brief Will set the current hour
		//! @param _h The value to set
		void setHour(int _h) { m_h = _h; adjustValues(); }

		//! @brief Will set the current minute
		//! @param _m The value to set
		void setMinute (int _m) { m_m = _m; adjustValues(); }

		//! @brief Will set the current second
		//! @param _s The value to set
		void setSecond(int _s) { m_s = _s; adjustValues(); }

		//! @brief Will set the current millisecond
		//! @param _ms The value to set
		void setMilliSecond(int _ms) { m_ms = _ms; adjustValues(); }

		//! @brief Will set the current time
		//! @param _h The hour to set
		//! @param _m The minute to set
		//! @param _s The second to set
		//! @param _ms The millisecond
		void set(int _h, int _m, int _s = 0, int _ms = 0);

		//! @brief Will set the current time
		//! @param _time The time to set
		void set(const QTime & _time);

		//! @brief Will set the current time
		//! @param _time The time to set
		void set(const aTime & _time);

		// ############################################################################################################

		// Static parser

		//! @brief Will parse the provided string and create a new time object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the hour, minute, second and milliseconds
		static aTime parseString(const char * _string, const char * _delimiter = ":");

		//! @brief Will parse the provided string and create a new time object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the hour, minute, second and milliseconds
		static aTime parseString(const wchar_t * _string, const wchar_t * _delimiter = L":");

		//! @brief Will parse the provided string and create a new time object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the hour, minute, second and milliseconds
		static aTime parseString(const std::string & _string, const std::string & _delimiter = ":");

		//! @brief Will parse the provided string and create a new time object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the hour, minute, second and milliseconds
		static aTime parseString(const std::wstring & _string, const std::wstring & _delimiter = L":");

		//! @brief Will parse the provided string and create a new time object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the hour, minute, second and milliseconds
		static aTime parseString(const QString & _string, const QString & _delimiter = ":");

		//! @brief Will adjust the provided time values
		//! Will return true if an overflow happened (day changed)
		//! @param _h The hour value
		//! @param _m The minute value
		//! @param _s The second value
		//! @param _ms The millisecond value
		static bool adjustValues(int & _h, int & _m, int & _s, int & _ms);

		//! @brief Will adjust the currently set values
		bool adjustValues(void);

		// ############################################################################################################

		// Operators

		aTime & operator = (const aTime & _time);
		aTime & operator = (const QTime & _time);
		aTime & operator += (const aTime & _time);
		aTime & operator += (const QTime & _time);
		aTime & operator -= (const aTime & _time);
		aTime & operator -= (const QTime & _time);

	private:
		int		m_h;		//! The current hour value
		int		m_m;		//! The current minute value
		int		m_s;		//! The current second value
		int		m_ms;		//! The current millisecond value
	};

	UICORE_API_EXPORT ak::aTime operator + (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT ak::aTime operator + (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT ak::aTime operator + (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT ak::aTime operator - (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT ak::aTime operator - (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT ak::aTime operator - (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator == (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator == (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator == (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator != (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator != (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator != (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator < (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator < (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator < (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator <= (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator <= (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator <= (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator > (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator > (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator > (const QTime & _lhv, const ak::aTime & _rhv);

	UICORE_API_EXPORT bool operator >= (const ak::aTime & _lhv, const ak::aTime & _rhv);
	UICORE_API_EXPORT bool operator >= (const ak::aTime & _lhv, const QTime & _rhv);
	UICORE_API_EXPORT bool operator >= (const QTime & _lhv, const ak::aTime & _rhv);

}
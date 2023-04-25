/*
 *	File:		aDate.h
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

	class UICORE_API_EXPORT aDate {
	public:
		aDate();
		aDate(int _d, int _m, int _y);
		aDate(const QDate & _date);
		aDate(const aDate & _other);
		virtual ~aDate();

		// ############################################################################################################

		// Getter

		//! @brief Will return the year
		int year(void) const { return m_y; }

		//! @brief Will return the month
		int month(void) const { return m_m; }

		//! @brief Will return the day
		int day(void) const { return m_d; }

		//! @brief Will return a String representation of this date
		//! @param _delimiter The delimiter that is used to seperate the day, month and year
		//! @param _dateFormat The dateFormat of the created string
		std::string toString(const std::string _delimiter = "-", dateFormat _dateFormat = dfDDMMYYYY) const;

		//! @brief Will return a String representation of this date
		//! @param _delimiter The delimiter that is used to seperate the day, month and year
		//! @param _dateFormat The dateFormat of the created string
		std::wstring toWString(const std::wstring _delimiter = L"-", dateFormat _dateFormat = dfDDMMYYYY) const;

		//! @brief Will return a String representation of this date
		//! @param _delimiter The delimiter that is used to seperate the day, month and year
		//! @param _dateFormat The dateFormat of the created string
		QString toQString(const QString _delimiter = "-", dateFormat _dateFormat = dfDDMMYYYY) const;

		//! @brief Will return a QDate object representing this object
		QDate toQDate(void) const;

		//! @brief Will return the difference to the the other date
		//! @param _other The other date
		aDate difference(const aDate & _other);

		//! @brief Will return the difference to the the other date
		//! @param _other The other date
		aDate difference(const QDate & _other);

		// ############################################################################################################

		// Setter

		//! @brief Will set the year
		//! @param _y The year to set
		void setYear(int _y) { m_y = _y; }

		//! @brief Will set the month
		//! @param _m The month to set
		void setMonth(int _m) { m_m = _m; }

		//! @brief Will set the day
		//! @param _d The day to set
		void setDay(int _d) { m_d = _d; }

		//! @brief Will set the current date
		//! @param _y The year to set
		//! @param _m The month to set
		//! @param _d The day to set
		void set(int _d, int _m, int _y);

		//! @brief Will set the current date
		//! @param _date The date to set
		void set(const QDate & _date);

		// ############################################################################################################

		// Static parser

		//! @brief Will parse the provided string and create a new date object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the day, month and year
		static aDate parseString(const char * _string, const char * _delimiter = "-", dateFormat _dateFormat = dfDDMMYYYY);

		//! @brief Will parse the provided string and create a new date object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the day, month and year
		static aDate parseString(const wchar_t * _string, const wchar_t * _delimiter = L"-", dateFormat _dateFormat = dfDDMMYYYY);

		//! @brief Will parse the provided string and create a new date object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the day, month and year
		static aDate parseString(const std::string & _string, const std::string & _delimiter = "-", dateFormat _dateFormat = dfDDMMYYYY);

		//! @brief Will parse the provided string and create a new date object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the day, month and year
		static aDate parseString(const std::wstring & _string, const std::wstring & _delimiter = L"-", dateFormat _dateFormat = dfDDMMYYYY);

		//! @brief Will parse the provided string and create a new date object
		//! @param _string The input string
		//! @param _delimiter The delimiter that seperates the day, month and year
		static aDate parseString(const QString & _string, const QString & _delimiter = "-", dateFormat _dateFormat = dfDDMMYYYY);
		
		// ############################################################################################################

		// Operators

		aDate & operator = (const aDate & _other);
		aDate & operator = (const QDate & _other);
		aDate & operator += (const aDate & _time);
		aDate & operator += (const QDate & _time);
		aDate & operator -= (const aDate & _time);
		aDate & operator -= (const QDate & _time);

	private:
		int		m_y;		//! The year value
		int		m_m;		//! The month value
		int		m_d;		//! The day value
	};

	UICORE_API_EXPORT ak::aDate operator + (const ak::aDate & _lhv, const ak::aDate & _rhv);
	UICORE_API_EXPORT ak::aDate operator + (const ak::aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT ak::aDate operator + (const QDate & _lhv, const ak::aDate & _rhv);

	UICORE_API_EXPORT ak::aDate operator - (const ak::aDate & _lhv, const ak::aDate & _rhv);
	UICORE_API_EXPORT ak::aDate operator - (const ak::aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT ak::aDate operator - (const QDate & _lhv, const ak::aDate & _rhv);

	UICORE_API_EXPORT bool operator == (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator == (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator == (const QDate & _lhv, const aDate & _rhv);

	UICORE_API_EXPORT bool operator != (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator != (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator != (const QDate & _lhv, const aDate & _rhv);
	
	UICORE_API_EXPORT bool operator < (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator < (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator < (const QDate & _lhv, const aDate & _rhv);
	
	UICORE_API_EXPORT bool operator > (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator > (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator > (const QDate & _lhv, const aDate & _rhv);
	
	UICORE_API_EXPORT bool operator <= (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator <= (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator <= (const QDate & _lhv, const aDate & _rhv);
	
	UICORE_API_EXPORT bool operator >= (const aDate & _lhv, const aDate & _rhv);
	UICORE_API_EXPORT bool operator >= (const aDate & _lhv, const QDate & _rhv);
	UICORE_API_EXPORT bool operator >= (const QDate & _lhv, const aDate & _rhv);

}
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

#include <akCore/aDate.h>

#include <qstringlist.h>

ak::aDate::aDate()
	: m_d(1), m_m(1), m_y(1900)
{}

ak::aDate::aDate(int _d, int _m, int _y)
	: m_d(_d), m_m(_m), m_y(_y)
{}

ak::aDate::aDate(const QDate & _other)
	: m_d(_other.day()), m_m(_other.month()), m_y(_other.year())
{}

ak::aDate::aDate(const aDate & _other) 
	: m_d(_other.m_d), m_m(_other.m_m), m_y(_other.m_y)
{}

ak::aDate::~aDate() {}

// ############################################################################################################

// Getter

std::string ak::aDate::toString(const std::string _delimiter, dateFormat _dateFormat) const {
	std::string d;
	std::string m;
	std::string y = std::to_string(m_y);

	if (m_d < 10) { d = "0" + std::to_string(m_d); }
	else { d = std::to_string(m_d); }

	if (m_m < 10) { m = "0" + std::to_string(m_m); }
	else { m = std::to_string(m_m); }

	std::string ret;

	switch (_dateFormat)
	{
	case ak::dfDDMMYYYY: ret.append(d).append(_delimiter).append(m).append(_delimiter).append(y); break;
	case ak::dfMMDDYYYY: ret.append(m).append(_delimiter).append(d).append(_delimiter).append(y); break;
	case ak::dfYYYYMMDD: ret.append(y).append(_delimiter).append(m).append(_delimiter).append(d); break;
	case ak::dfYYYYDDMM: ret.append(y).append(_delimiter).append(d).append(_delimiter).append(m); break;
	default:
		assert(0);
		return "01-01-1900";
		break;
	}
	return ret;
}

std::wstring ak::aDate::toWString(const std::wstring _delimiter, dateFormat _dateFormat) const {
	std::wstring d;
	std::wstring m;
	std::wstring y = std::to_wstring(m_y);

	if (m_d < 10) { d = L"0" + std::to_wstring(m_d); }
	else { d = std::to_wstring(m_d); }

	if (m_m < 10) { m = L"0" + std::to_wstring(m_m); }
	else { m = std::to_wstring(m_m); }

	std::wstring ret;

	switch (_dateFormat)
	{
	case ak::dfDDMMYYYY: ret.append(d).append(_delimiter).append(m).append(_delimiter).append(y); break;
	case ak::dfMMDDYYYY: ret.append(m).append(_delimiter).append(d).append(_delimiter).append(y); break;
	case ak::dfYYYYMMDD: ret.append(y).append(_delimiter).append(m).append(_delimiter).append(d); break;
	case ak::dfYYYYDDMM: ret.append(y).append(_delimiter).append(d).append(_delimiter).append(m); break;
	default:
		assert(0);
		return L"01-01-1900";
		break;
	}
	return ret;
}

QString ak::aDate::toQString(const QString _delimiter, dateFormat _dateFormat) const {
	QString d;
	QString m;
	QString y = QString::number(m_y);

	if (m_d < 10) { d = "0" + QString::number(m_d); }
	else { d = QString::number(m_d); }

	if (m_m < 10) { m = "0" + QString::number(m_m); }
	else { m = QString::number(m_m); }

	QString ret;

	switch (_dateFormat)
	{
	case ak::dfDDMMYYYY: ret.append(d).append(_delimiter).append(m).append(_delimiter).append(y); break;
	case ak::dfMMDDYYYY: ret.append(m).append(_delimiter).append(d).append(_delimiter).append(y); break;
	case ak::dfYYYYMMDD: ret.append(y).append(_delimiter).append(m).append(_delimiter).append(d); break;
	case ak::dfYYYYDDMM: ret.append(y).append(_delimiter).append(d).append(_delimiter).append(m); break;
	default:
		assert(0);
		return "01-01-1900";
		break;
	}
	return ret;
}

QDate ak::aDate::toQDate(void) const { return QDate(m_y, m_m, m_d); }

ak::aDate ak::aDate::difference(const aDate & _other) { if (*this >= _other) { return *this - _other; } else { return _other - *this; } }

ak::aDate ak::aDate::difference(const QDate & _other) { return difference(aDate(_other)); }

// ############################################################################################################

// Setter

void ak::aDate::set(int _d, int _m, int _y) {
	m_y = _y;
	m_m = _m;
	m_d = _d;
}

void ak::aDate::set(const QDate & _date) {
	m_y = _date.year();
	m_m = _date.month();
	m_d = _date.day();
}

// ############################################################################################################

// Static parser

ak::aDate ak::aDate::parseString(const char * _string, const char * _delimiter, dateFormat _dateFormat) {
	return parseString(QString(_string), QString(_delimiter), _dateFormat);
}

ak::aDate ak::aDate::parseString(const wchar_t * _string, const wchar_t * _delimiter, dateFormat _dateFormat) {
	return parseString(QString::fromWCharArray(_string), QString::fromWCharArray(_delimiter), _dateFormat);
}

ak::aDate ak::aDate::parseString(const std::string & _string, const std::string & _delimiter, dateFormat _dateFormat) {
	return parseString(QString(_string.c_str()), QString(_delimiter.c_str()), _dateFormat);
}

ak::aDate ak::aDate::parseString(const std::wstring & _string, const std::wstring & _delimiter, dateFormat _dateFormat) {
	return parseString(QString::fromStdWString(_string), QString::fromStdWString(_delimiter), _dateFormat);
}

ak::aDate ak::aDate::parseString(const QString & _string, const QString & _delimiter, dateFormat _dateFormat) {
	QStringList lst{ _string.split(_delimiter, Qt::SkipEmptyParts) };
	if (lst.length() != 3) {
		assert(0);
		return aDate();
	}

	switch (_dateFormat)
	{
	case ak::dfDDMMYYYY: return aDate(lst[0].toInt(), lst[1].toInt(), lst[2].toInt());
	case ak::dfMMDDYYYY: return aDate(lst[1].toInt(), lst[0].toInt(), lst[2].toInt());
	case ak::dfYYYYMMDD: return aDate(lst[2].toInt(), lst[1].toInt(), lst[0].toInt());
	case ak::dfYYYYDDMM: return aDate(lst[2].toInt(), lst[0].toInt(), lst[1].toInt());
	default:
		assert(0);
		return aDate();
	}
}

// ############################################################################################################

// Operators

ak::aDate & ak::aDate::operator = (const aDate & _other) {
	m_y = _other.m_y;
	m_m = _other.m_m;
	m_d = _other.m_d;
	return *this;
}

ak::aDate & ak::aDate::operator = (const QDate & _other) {
	m_y = _other.year();
	m_m = _other.month();
	m_d = _other.day();
	return *this;
}


ak::aDate & ak::aDate::operator += (const aDate & _time) {
	m_y += _time.year();
	m_m += _time.month();
	m_d += _time.day();
	return *this;
}

ak::aDate & ak::aDate::operator += (const QDate & _time) {
	m_y += _time.year();
	m_m += _time.month();
	m_d += _time.day();
	return *this;
}
ak::aDate & ak::aDate::operator -= (const aDate & _time) {
	m_y -= _time.year();
	m_m -= _time.month();
	m_d -= _time.day();
	return *this;
}
ak::aDate & ak::aDate::operator -= (const QDate & _time) {
	m_y -= _time.year();
	m_m -= _time.month();
	m_d -= _time.day();
	return *this;
}

ak::aDate ak::operator + (const ak::aDate & _lhv, const ak::aDate & _rhv) {
	ak::aDate ret{ _lhv.year() + _rhv.year(), _lhv.month() + _rhv.month(), _lhv.day() + _rhv.day() };
	return ret;
}
ak::aDate ak::operator + (const ak::aDate & _lhv, const QDate & _rhv) {
	ak::aDate ret{ _lhv.year() + _rhv.year(), _lhv.month() + _rhv.month(), _lhv.day() + _rhv.day() };
	return ret;
}
ak::aDate ak::operator + (const QDate & _lhv, const ak::aDate & _rhv) {
	ak::aDate ret{ _lhv.year() + _rhv.year(), _lhv.month() + _rhv.month(), _lhv.day() + _rhv.day() };
	return ret;
}

ak::aDate ak::operator - (const ak::aDate & _lhv, const ak::aDate & _rhv) {
	ak::aDate ret{ _lhv.year() - _rhv.year(), _lhv.month() - _rhv.month(), _lhv.day() - _rhv.day() };
	return ret;
}
ak::aDate ak::operator - (const ak::aDate & _lhv, const QDate & _rhv) {
	ak::aDate ret{ _lhv.year() - _rhv.year(), _lhv.month() - _rhv.month(), _lhv.day() - _rhv.day() };
	return ret;
}
ak::aDate ak::operator - (const QDate & _lhv, const ak::aDate & _rhv) {
	ak::aDate ret{ _lhv.year() - _rhv.year(), _lhv.month() - _rhv.month(), _lhv.day() - _rhv.day() };
	return ret;
}

bool ak::operator == (const aDate & _lhv, const aDate & _rhv) {
	return _lhv.year() == _rhv.year() && _lhv.month() == _rhv.month() && _lhv.day() == _rhv.day();
}
bool ak::operator == (const aDate & _lhv, const QDate & _rhv) {
	return _lhv.year() == _rhv.year() && _lhv.month() == _rhv.month() && _lhv.day() == _rhv.day();
}
bool ak::operator == (const QDate & _lhv, const aDate & _rhv) {
	return _lhv.year() == _rhv.year() && _lhv.month() == _rhv.month() && _lhv.day() == _rhv.day();
}

bool ak::operator != (const aDate & _lhv, const aDate & _rhv) {
	return _lhv.year() != _rhv.year() || _lhv.month() != _rhv.month() || _lhv.day() != _rhv.day();
}
bool ak::operator != (const aDate & _lhv, const QDate & _rhv) {
	return _lhv.year() != _rhv.year() || _lhv.month() != _rhv.month() || _lhv.day() != _rhv.day();
}
bool ak::operator != (const QDate & _lhv, const aDate & _rhv) {
	return _lhv.year() != _rhv.year() || _lhv.month() != _rhv.month() || _lhv.day() != _rhv.day();
}

bool ak::operator < (const aDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() < _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator < (const aDate & _lhv, const QDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() < _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator < (const QDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() < _rhv.day()) { return true; }
		}
	}
	return false;
}

bool ak::operator > (const aDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() > _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator > (const aDate & _lhv, const QDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() > _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator > (const QDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() > _rhv.day()) { return true; }
		}
	}
	return false;
}

bool ak::operator <= (const aDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() <= _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator <= (const aDate & _lhv, const QDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() <= _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator <= (const QDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() < _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() < _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() <= _rhv.day()) { return true; }
		}
	}
	return false;
}

bool ak::operator >= (const aDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() >= _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator >= (const aDate & _lhv, const QDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() >= _rhv.day()) { return true; }
		}
	}
	return false;
}
bool ak::operator >= (const QDate & _lhv, const aDate & _rhv) {
	if (_lhv.year() > _rhv.year()) { return true; }
	else if (_lhv.year() == _rhv.year()) {
		if (_lhv.month() > _rhv.month()) { return true; }
		else if (_lhv.month() == _rhv.month()) {
			if (_lhv.day() >= _rhv.day()) { return true; }
		}
	}
	return false;
}
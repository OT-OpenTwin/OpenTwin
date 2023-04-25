/*
 *	File:		aTime.cpp
 *	Package:	akCore
 *
 *  Created on: July 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akCore/aTime.h>

// Qt header
#include <qstringlist.h>

ak::aTime::aTime() 
	: m_h(0), m_m(0), m_s(0), m_ms(0) {}

ak::aTime::aTime(int _h, int _m, int _s, int _ms) 
	: m_h(_h), m_m(_m), m_s(_s), m_ms(_ms) {
	adjustValues();
}

ak::aTime::aTime(const QTime & _time)
	: m_h(_time.hour()), m_m(_time.minute()), m_s(_time.second()), m_ms(_time.msec()) {}

ak::aTime::aTime(const aTime & _other)
	: m_h(_other.m_h), m_m(_other.m_m), m_s(_other.m_s), m_ms(_other.m_ms) {}

ak::aTime::~aTime() {}

// ############################################################################################################

// Getter

std::string ak::aTime::toString(const std::string _delimiter, timeFormat _timeFormat) const {
	std::string h;
	std::string m;
	
	if (m_h < 10) { h = "0" + std::to_string(m_h); }
	else { h = std::to_string(m_h); }

	if (m_m < 10) { m = "0" + std::to_string(m_m); }
	else { m = std::to_string(m_m); }

	switch (_timeFormat)
	{
	case ak::tfHHMM: return h + _delimiter + m;
	case ak::tfHHMMSS:
	{
		std::string s;
		if (m_s < 10) { s = "0" + std::to_string(m_s); }
		else { s = std::to_string(m_s); }
		return h + _delimiter + m + _delimiter + s;
	}
	case ak::tfHHMMSSMMMM:
	{
		std::string s;
		std::string ms;

		if (m_s < 10) { s = "0" + std::to_string(m_s); }
		else { s = std::to_string(m_s); }

		if (m_ms < 10) { ms = "000" + std::to_string(m_ms); }
		else if (m_ms < 100) { ms = "00" + std::to_string(m_ms); }
		else if (m_ms < 1000) { ms = "0" + std::to_string(m_ms); }
		else { ms = std::to_string(m_ms); }

		return h + _delimiter + m + _delimiter + s + _delimiter + ms;
	}
	default:
		assert(0);
		return "00:00";
	}
}

std::wstring ak::aTime::toWString(const std::wstring _delimiter, timeFormat _timeFormat) const {
	std::wstring h;
	std::wstring m;

	if (m_h < 10) { h = L"0" + std::to_wstring(m_h); }
	else { h = std::to_wstring(m_h); }

	if (m_m < 10) { m = L"0" + std::to_wstring(m_m); }
	else { m = std::to_wstring(m_m); }

	switch (_timeFormat)
	{
	case ak::tfHHMM: return h + _delimiter + m;
	case ak::tfHHMMSS:
	{
		std::wstring s;
		if (m_s < 10) { s = L"0" + std::to_wstring(m_s); }
		else { s = std::to_wstring(m_s); }
		return h + _delimiter + m + _delimiter + s;
	}
	case ak::tfHHMMSSMMMM:
	{
		std::wstring s;
		std::wstring ms;

		if (m_s < 10) { s = L"0" + std::to_wstring(m_s); }
		else { s = std::to_wstring(m_s); }

		if (m_ms < 10) { ms = L"000" + std::to_wstring(m_ms); }
		else if (m_ms < 100) { ms = L"00" + std::to_wstring(m_ms); }
		else if (m_ms < 1000) { ms = L"0" + std::to_wstring(m_ms); }
		else { ms = std::to_wstring(m_ms); }

		return h + _delimiter + m + _delimiter + s + _delimiter + ms;
	}
	default:
		assert(0);
		return L"00:00";
	}
}

QString ak::aTime::toQString(const QString _delimiter, timeFormat _timeFormat) const {
	QString h;
	QString m;

	if (m_h < 10) { h = "0" + QString::number(m_h); }
	else { h = QString::number(m_h); }

	if (m_m < 10) { m = "0" + QString::number(m_m); }
	else { m = QString::number(m_m); }

	switch (_timeFormat)
	{
	case ak::tfHHMM: return h + _delimiter + m;
	case ak::tfHHMMSS:
	{
		QString s;
		if (m_s < 10) { s = "0" + QString::number(m_s); }
		else { s = QString::number(m_s); }
		return h + _delimiter + m + _delimiter + s;
	}
	case ak::tfHHMMSSMMMM:
	{
		QString s;
		QString ms;

		if (m_s < 10) { s = "0" + QString::number(m_s); }
		else { s = QString::number(m_s); }

		if (m_ms < 10) { ms = "000" + QString::number(m_ms); }
		else if (m_ms < 100) { ms = "00" + QString::number(m_ms); }
		else if (m_ms < 1000) { ms = "0" + QString::number(m_ms); }
		else { ms = QString::number(m_ms); }

		return h + _delimiter + m + _delimiter + s + _delimiter + ms;
	}
	default:
		assert(0);
		return "00:00";
	}
}

QTime ak::aTime::toQTime(void) const { return QTime(m_h, m_m, m_s, m_ms); }

ak::aTime ak::aTime::difference(const aTime & _other) { if (*this >= _other) { return *this - _other; } else { return _other - *this; } }

ak::aTime ak::aTime::difference(const QTime & _other) { return difference(aTime(_other)); }

// ############################################################################################################

// Setter

void ak::aTime::set(int _h, int _m, int _s, int _ms) {
	m_h = _h;
	m_m = _m;
	m_s = _s;
	m_ms = _ms;
	adjustValues();
}

void ak::aTime::set(const QTime & _time) {
	m_h = _time.hour();
	m_m = _time.minute();
	m_s = _time.second();
	m_ms = _time.msec();
	adjustValues();
}

void ak::aTime::set(const aTime & _time) {
	m_h = _time.m_h;
	m_m = _time.m_m;
	m_s = _time.m_s;
	m_ms = _time.m_ms;
	adjustValues();
}

// ############################################################################################################

// Static parser

ak::aTime ak::aTime::parseString(const char * _string, const char * _delimiter) {
	return parseString(QString(_string), QString(_delimiter));
}

ak::aTime ak::aTime::parseString(const wchar_t * _string, const wchar_t * _delimiter) {
	return parseString(QString::fromWCharArray(_string), QString::fromWCharArray(_delimiter));
}

ak::aTime ak::aTime::parseString(const std::string & _string, const std::string & _delimiter) {
	return parseString(QString(_string.c_str()), QString(_delimiter.c_str()));
}

ak::aTime ak::aTime::parseString(const std::wstring & _string, const std::wstring & _delimiter) {
	return parseString(QString::fromStdWString(_string), QString::fromStdWString(_delimiter));
}

ak::aTime ak::aTime::parseString(const QString & _string, const QString & _delimiter) {
	QStringList lst{ _string.split(_delimiter, QString::KeepEmptyParts) };
	switch (lst.length())
	{
	case 1: return aTime(lst[0].toInt(), 0, 0, 0);
	case 2: return aTime(lst[0].toInt(), lst[1].toInt(), 0, 0);
	case 3: return aTime(lst[0].toInt(), lst[1].toInt(), lst[2].toInt(), 0);
	case 4: return aTime(lst[0].toInt(), lst[1].toInt(), lst[2].toInt(), lst[3].toInt());
	default:
		assert(0);
		return aTime();
	}
}

bool ak::aTime::adjustValues(int & _h, int & _m, int & _s, int & _ms) {
	while (_ms < 0) { _ms += 1000; _s--; }
	while (_ms > 999) { _ms -= 1000; _s++; }
	while (_s < 0) { _s += 60; _m--; }
	while (_s > 59) { _s -= 60; _m++; }
	while (_m < 0) { _m += 60; _h--; }
	while (_m > 59) { _m -= 60; _h++; }
	bool ov{ false };
	while (_h < 0) { _h += 24; ov = true; }
	while (_h > 23) { _h -= 24; ov = true; }
	return ov;
}

bool ak::aTime::adjustValues(void) {
	return adjustValues(m_h, m_m, m_s, m_ms);
}

// ############################################################################################################

// Operators

ak::aTime & ak::aTime::operator = (const aTime & _time) {
	m_h = _time.m_h;
	m_m = _time.m_m;
	m_s = _time.m_s;
	m_ms = _time.m_ms;
	return *this;
}

ak::aTime & ak::aTime::operator = (const QTime & _time) {
	m_h = _time.hour();
	m_m = _time.minute();
	m_s = _time.second();
	m_ms = _time.msec();
	return *this;
}

ak::aTime & ak::aTime::operator += (const aTime & _time) {
	m_h += _time.m_h;
	m_m += _time.m_m;
	m_s += _time.m_s;
	m_ms += _time.m_ms;
	adjustValues();
	return *this;
}

ak::aTime & ak::aTime::operator += (const QTime & _time) {
	m_h += _time.hour();
	m_m += _time.minute();
	m_s += _time.second();
	m_ms += _time.msec();
	adjustValues();
	return *this;
}

ak::aTime & ak::aTime::operator -= (const aTime & _time) {
	m_h -= _time.m_h;
	m_m -= _time.m_m;
	m_s -= _time.m_s;
	m_ms -= _time.m_ms;
	adjustValues();
	return *this;
}

ak::aTime & ak::aTime::operator -= (const QTime & _time) {
	m_h -= _time.hour();
	m_m -= _time.minute();
	m_s -= _time.second();
	m_ms -= _time.msec();
	adjustValues();
	return *this;
}

ak::aTime ak::operator + (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	int h{ _lhv.hour() + _rhv.hour() };
	int m{ _lhv.minute() + _rhv.minute() };
	int s{ _lhv.second() + _rhv.second() };
	int ms{ _lhv.msec() + _rhv.msec() };
	return aTime(h, m, s, ms);
}
ak::aTime ak::operator + (const ak::aTime & _lhv, const QTime & _rhv) {
	int h{ _lhv.hour() + _rhv.hour() };
	int m{ _lhv.minute() + _rhv.minute() };
	int s{ _lhv.second() + _rhv.second() };
	int ms{ _lhv.msec() + _rhv.msec() };
	return aTime(h, m, s, ms);
}
ak::aTime ak::operator + (const QTime & _lhv, const ak::aTime & _rhv) {
	int h{ _rhv.hour() + _lhv.hour() };
	int m{ _rhv.minute() + _lhv.minute() };
	int s{ _rhv.second() + _lhv.second() };
	int ms{ _rhv.msec() + _lhv.msec() };
	return aTime(h, m, s, ms);
}

ak::aTime ak::operator - (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	int h{ _lhv.hour() - _rhv.hour() };
	int m{ _lhv.minute() - _rhv.minute() };
	int s{ _lhv.second() - _rhv.second() };
	int ms{ _lhv.msec() - _rhv.msec() };
	return aTime(h, m, s, ms);
}
ak::aTime ak::operator - (const ak::aTime & _lhv, const QTime & _rhv) {
	int h{ _lhv.hour() - _rhv.hour() };
	int m{ _lhv.minute() - _rhv.minute() };
	int s{ _lhv.second() - _rhv.second() };
	int ms{ _lhv.msec() - _rhv.msec() };
	return aTime(h, m, s, ms);
}
ak::aTime ak::operator - (const QTime & _lhv, const ak::aTime & _rhv) {
	int h{ _rhv.hour() - _lhv.hour() };
	int m{ _rhv.minute() - _lhv.minute() };
	int s{ _rhv.second() - _lhv.second() };
	int ms{ _rhv.msec() - _lhv.msec() };
	return aTime(h, m, s, ms);
}

bool ak::operator == (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	return _lhv.hour() == _rhv.hour() && _lhv.minute() == _rhv.minute() && _lhv.second() == _rhv.second() && _lhv.msec() == _rhv.msec();
}
bool ak::operator == (const ak::aTime & _lhv, const QTime & _rhv) {
	return _lhv.hour() == _rhv.hour() && _lhv.minute() == _rhv.minute() && _lhv.second() == _rhv.second() && _lhv.msec() == _rhv.msec();
}
bool ak::operator == (const QTime & _lhv, const ak::aTime & _rhv) {
	return _rhv.hour() == _lhv.hour() && _rhv.minute() == _lhv.minute() && _rhv.second() == _lhv.second() && _rhv.msec() == _lhv.msec();
}

bool ak::operator != (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	return _lhv.hour() != _rhv.hour() || _lhv.minute() != _rhv.minute() || _lhv.second() != _rhv.second() || _lhv.msec() != _rhv.msec();
}
bool ak::operator != (const ak::aTime & _lhv, const QTime & _rhv) {
	return _lhv.hour() != _rhv.hour() || _lhv.minute() != _rhv.minute() || _lhv.second() != _rhv.second() || _lhv.msec() != _rhv.msec();
}
bool ak::operator != (const QTime & _lhv, const ak::aTime & _rhv) {
	return _rhv.hour() != _lhv.hour() || _rhv.minute() != _lhv.minute() || _rhv.second() != _lhv.second() || _rhv.msec() != _lhv.msec();
}

bool ak::operator < (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() < _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator < (const ak::aTime & _lhv, const QTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() < _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator < (const QTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() < _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}

bool ak::operator <= (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() <= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator <= (const ak::aTime & _lhv, const QTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() <= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator <= (const QTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() < _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() < _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() < _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() <= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}

bool ak::operator > (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() > _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator > (const ak::aTime & _lhv, const QTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() > _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator > (const QTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() > _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}

bool ak::operator >= (const ak::aTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() >= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator >= (const ak::aTime & _lhv, const QTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() >= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
bool ak::operator >= (const QTime & _lhv, const ak::aTime & _rhv) {
	if (_lhv.hour() > _rhv.hour()) { return true; }
	else if (_lhv.hour() == _rhv.hour()) {
		if (_lhv.minute() > _rhv.minute()) { return true; }
		else if (_lhv.minute() == _rhv.minute()) {
			if (_lhv.second() > _rhv.second()) { return true; }
			else if (_lhv.second() == _rhv.second()) {
				if (_lhv.msec() >= _rhv.msec()) { return true; }
			}
		}
	}
	return false;
}
/*
 *	File:		aColor.cpp
 *	Package:	akGui
 *
 *  Created on: April 02, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColor.h>
#include <akCore/aException.h>

// C++ header
#include <string>				// C++ string
#include <sstream>				// string stream

ak::aColor::aColor()
:	m_a(255),
	m_r(0),
	m_g(0),
	m_b(0),
	m_wasChanged(false)
{}

ak::aColor::aColor(
	int					_r,
	int					_g,
	int					_b,
	int					_a
) : m_wasChanged(false) {
	try {
		if (_a < 0 || _a > 255) { assert(0); throw aException("Value out of range", "Check alpha channel value"); }
		if (_r < 0 || _r > 255) { assert(0); throw aException("Value out of range", "Check red channel value"); }
		if (_g < 0 || _g > 255) { assert(0); throw aException("Value out of range", "Check green channel value"); }
		if (_b < 0 || _b > 255) { assert(0); throw aException("Value out of range", "Check blue channel value"); }
		m_a = _a;
		m_r = _r;
		m_g = _g;
		m_b = _b;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::aColor()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::aColor()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::aColor()"); }
}

ak::aColor::aColor(
	const ak::aColor &	_other
)
	: m_a(_other.a()),
	m_b(_other.b()),
	m_g(_other.g()),
	m_r(_other.r()),
	m_wasChanged(false)
{}

ak::aColor::aColor(
	const QColor &		_other
)
	: m_a(_other.alpha()),
	m_b(_other.blue()),
	m_g(_other.green()),
	m_r(_other.red()),
	m_wasChanged(false)
{}

ak::aColor & ak::aColor::operator = (
	const ak::aColor & _other
) {
	m_a = _other.a();
	m_r = _other.r();
	m_g = _other.g();
	m_b = _other.b();
	m_wasChanged = false;
	return *this;
}

ak::aColor & ak::aColor::operator = (
	const QColor & _other
	) {
	m_a = _other.alpha();
	m_r = _other.red();
	m_g = _other.green();
	m_b = _other.blue();
	m_wasChanged = false;
	return *this;
}

ak::aColor ak::aColor::operator + (
	const ak::aColor & _other
) {
	ak::aColor out;
	int a = m_a + _other.a();
	if (a > 0) { a /= 2; }
	int r = m_r + _other.r();
	if (r > 0) { r /= 2; }
	int g = m_g + _other.g();
	if (g > 0) { g /= 2; }
	int b = m_b + _other.b();
	if (b > 0) { b /= 2; }
	return aColor(r, g, b, a);
}

bool ak::aColor::operator == (const ak::aColor & _other) const {
	if (m_a != _other.a()) { return false; }
	if (m_r != _other.r()) { return false; }
	if (m_g != _other.g()) { return false; }
	if (m_b != _other.b()) { return false; }
	return true;
}

bool ak::aColor::operator != (const aColor & _other) const { return !(*this == _other); }

ak::aColor::~aColor(){}

// Set aColor

void ak::aColor::setA(
	int					_a
) {
	try {
		if (_a < 0 || _a > 255) { assert(0); throw aException("Value out of range", "Check alpha channel value"); }
		m_a = _a;
		m_wasChanged = true;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::setA()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::setA()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::setA()"); }
}

void ak::aColor::setR(int
	_r
) {
	try {
		if (_r < 0 || _r > 255) { assert(0); throw aException("Value out of range", "Check red channel value"); }
		m_r = _r;
		m_wasChanged = true;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::setR()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::setR()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::setR()"); }
}

void ak::aColor::setG(
	int					_g
) {
	try {
		if (_g < 0 || _g > 255) { assert(0); throw aException("Value out of range", "Check green channel value"); }
		m_g = _g;
		m_wasChanged = true;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::setG()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::setG()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::setG()"); }
}

void ak::aColor::setB(int
	_b
) {
	try {
		if (_b < 0 || _b > 255) { assert(0); throw aException("Value out of range", "Check blue channel value"); }
		m_b = _b;
		m_wasChanged = true;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::setB()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::setB()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::setB()"); }
}

void ak::aColor::setRGBA(
	int				_r,
	int				_g,
	int				_b,
	int				_a
) {
	try {
		if (_a < 0 || _a > 255) { assert(0); throw aException("Value out of range", "Check alpha channel value"); }
		if (_r < 0 || _r > 255) { assert(0); throw aException("Value out of range", "Check red channel value"); }
		if (_g < 0 || _g > 255) { assert(0); throw aException("Value out of range", "Check green channel value"); }
		if (_b < 0 || _b > 255) { assert(0); throw aException("Value out of range", "Check blue channel value"); }
		m_a = _a;
		m_r = _r;
		m_g = _g;
		m_b = _b;
		m_wasChanged = true;
	}
	catch (const aException & e) { throw aException(e, "ak::aColor::setRGBA()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColor::setRGBA()"); }
	catch (...) { throw aException("Unknown error", "ak::aColor::setRGBA()"); }
}

// Get aColor

int ak::aColor::a(void) const { return m_a; }

int ak::aColor::r(void) const { return m_r; }

int ak::aColor::g(void) const { return m_g; }

int ak::aColor::b(void) const { return m_b; }

// Get aColor string

QString ak::aColor::toHexString(
	bool				_includeA,
	const QString &		_prefix
) const {
	return toHexString(*this, _includeA, _prefix);
}

QString ak::aColor::toHexString(
	const aColor &		_color,
	bool				_includeA,
	const QString &		_prefix
) {
	QString out = _prefix;
	QString v = "";
	if (_includeA) {
		v = ak::valueToHexString(_color.m_a, 2);
		out.append(v);
	}
	v = ak::valueToHexString(_color.m_r, 2);
	out.append(v);
	v = ak::valueToHexString(_color.m_g, 2);
	out.append(v);
	v = ak::valueToHexString(_color.m_b, 2);
	out.append(v);
	return out;
}

QString ak::aColor::toHexString(
	const QColor &		_color,
	bool				_includeA,
	const QString &		_prefix
) {
	QString out = _prefix;
	QString v = "";
	if (_includeA) {
		v = ak::valueToHexString(_color.alpha(), 2);
		out.append(v);
	}
	v = ak::valueToHexString(_color.red(), 2);
	out.append(v);
	v = ak::valueToHexString(_color.green(), 2);
	out.append(v);
	v = ak::valueToHexString(_color.blue(), 2);
	out.append(v);
	return out;
}

QString ak::aColor::toRGBString(
	const QString &		_delimiter
) const {
	QString out = QString::number(m_r);
	out.append(_delimiter);
	out.append(QString::number(m_g));
	out.append(_delimiter);
	out.append(QString::number(m_b));
	return out;
}

QString ak::aColor::toARGBString(
	const QString &		_delimiter
) const {
	QString out = QString::number(m_a);
	out.append(_delimiter);
	out.append(QString::number(m_r));
	out.append(_delimiter);
	out.append(QString::number(m_g));
	out.append(_delimiter);
	out.append(QString::number(m_b));
	return out;
}

QColor ak::aColor::toQColor(void) const
{ return QColor(m_r, m_g, m_b, m_a); }

void ak::aColor::changeBy(int _value, bool _invertValueIfOutOfLimit, bool _includeAlpha) {
	int a = m_a + _value;
	int r = m_r + _value;
	int g = m_g + _value;
	int b = m_b + _value;

	if (_invertValueIfOutOfLimit) {
		if (a < 0 || a > 255) { a = m_a - _value; }
		if (r < 0 || r > 255) { r = m_r - _value; }
		if (g < 0 || g > 255) { g = m_g - _value; }
		if (b < 0 || b > 255) { b = m_b - _value; }
	}
	if (_includeAlpha) { setRGBA(r, g, b, a); }
	else { setRGBA(r, g, b, m_a); }
}

bool ak::aColor::wasChanged(void) const { return m_wasChanged; }

QString ak::valueToHexString(
	int					_value,
	int					_minimumLength
) {
	std::stringstream stream;
	stream << std::hex << _value;
	QString out = "";
	std::string str = stream.str();
	for (int i = str.length(); i < _minimumLength; i++) { out.append("0"); }
	return QString(out.append(str.c_str()));
}

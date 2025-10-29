// @otlicense

/*
 *	File:		aException.h
 *	Package:	akCore
 *
 *  Created on: January 29, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>

ak::aException::aException()
{
	m_where = "exception_where";
	m_what = "exception_what";
	m_out = "ex : exception";
	m_type = Undefined;
}

ak::aException::aException(
	const char *	 				_exWhat,
	const char *				 	_exWhere,
	ak::aException::exceptionType				_exType
) : m_what(_exWhat), m_where(_exWhere), m_type(_exType) { buildOut(); }


ak::aException::aException(
	const std::string &				_exWhat,
	const char *					_exWhere,
	exceptionType					_exType
) : m_what(_exWhat), m_where(_exWhere), m_type(_exType) { buildOut(); }

ak::aException::aException(
	const std::string &				_exWhat,
	const std::string &				_exWhere,
	exceptionType					_exType
) : m_what(_exWhat), m_where(_exWhere), m_type(_exType) { buildOut(); }

ak::aException::aException(
	const char *					_exWhat,
	const std::string &				_exWhere,
	exceptionType					_exType
) : m_what(_exWhat), m_where(_exWhere), m_type(_exType) { buildOut(); }

ak::aException::aException(
	const ak::aException &			_other
) : m_what(_other.getWhat()),
	m_where(_other.getWhere()),
	m_type(_other.getType())
{
	buildOut();
	m_type = _other.getType();
}

ak::aException::aException(
	const ak::aException & 			_other,
	const char * 					_exWhere
) : m_what(_other.getWhat()),
	m_where(_other.getWhere()),
	m_type(_other.getType())
{
	m_where += AK_CSTR_NL "@ ";
	m_where += _exWhere;
	buildOut();
	m_type = _other.getType();
}

ak::aException::~aException() throw() {}

const char * ak::aException::what(void) const throw() { return m_out.c_str(); }

std::string ak::aException::getWhat(void) const { return m_what; }

std::string ak::aException::getWhere(void) const {return m_where; }

ak::aException::exceptionType ak::aException::getType(void) const { return m_type; }

void ak::aException::buildOut(void) {
	m_out = m_what;
	m_out += AK_CSTR_NL "@ ";
	m_out += m_where;
}

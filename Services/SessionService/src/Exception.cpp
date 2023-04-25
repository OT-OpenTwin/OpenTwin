/*
 * Exception.cpp
 *
 *  Created on: December 04, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// SessionService header
#include <Exception.h>

ErrorException::ErrorException(
	const char *				_what
) : std::exception(_what)
{}

ErrorException::ErrorException(
	const std::exception &		_other
) : std::exception(_other.what())
{}

WarningException::WarningException(
	const char *				_what
) : std::exception(_what)
{}

WarningException::WarningException(
	const std::exception &		_other
) : std::exception(_other.what())
{}
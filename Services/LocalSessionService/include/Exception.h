/*
 * Exception.h
 *
 *  Created on: December 04, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// C++ header
#include <exception>		// Base class

class ErrorException : public std::exception {
public:

	//! @brief Default constructor
	ErrorException() {}

	//! @brief Assignment constructor
	//! @param _what The exception text
	ErrorException(const char * _what);

	//! @brief Copy constructor
	//! @param The other exception
	ErrorException(const std::exception& _other);

	//! @brief Deconstructor
	virtual ~ErrorException() throw() {}
};

// ##################################################################################

class WarningException : public std::exception {
public:

	//! @brief Default constructor
	WarningException() {}

	//! @brief Assignment constructor
	//! @param _what The exception text
	WarningException(
		const char *				_what
	);

	//! @brief Copy constructor
	//! @param The other exception
	WarningException(
		const std::exception &		_other
	);

	//! @brief Deconstructor
	virtual ~WarningException() throw() {}
};

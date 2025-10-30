// @otlicense
// File: aException.h
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

 // AK header
#include <akCore/globalDataTypes.h>

// C++ header
#include <exception>			// Base class
#include <string>				// std::string

namespace ak {

	//! The ak::Exception class can be used as an universal exception which may used to provide nicely readable exception messages.
	//! The provided exception message 'ex_what' is representing the exception message
	//! The provided where message 'ex_where' is representing the location where this exception is thrown from.
	//!		If the exception will be catched by a try block, this exception can be thrown to the next higher level by providing
	//!		just the new location. At the beginning of each provided location a '@ ' will be added
	//!		A ak::Exception.what() message can look like this:
	//!			Exception Message
	//!			@ Inner function
	//!			@ Outter function
	//!			@ Initial calling function
	class UICORE_API_EXPORT aException : public std::exception {
	public:

		//! Used to determine the type of the exception
		enum exceptionType {
			Undefined = 0,
			Arithmetic,
			ArrayOutOfBounds,
			FileNotFound,
			IO,
			Nullptr,
			NumberFormat,
			InvalidInput,
			UnknownCommand,
			BadAlloc
		};

		//! @brief Default constructor. Will initialize the type with undefined
		aException();

		//! @brief Assignment constructor. Will set the internal variables
		//! @param _exWhat The exception message
		//! @param _exWhere The location where the exception was thrown from (e.g. function name)
		//! @param _exType The type of the exception
		aException(
			const char *				_exWhat,
			const char *				_exWhere,
			exceptionType				_exType = exceptionType::Undefined
		);

		//! @brief Assignment constructor. Will set the internal variables
		//! @param _exWhat The exception message
		//! @param _exWhere The location where the exception was thrown from (e.g. function name)
		//! @param _exType The type of the exception
		aException(
			const std::string &			_exWhat,
			const char *				_exWhere,
			exceptionType				_exType = exceptionType::Undefined
		);

		//! @brief Assignment constructor. Will set the internal variables
		//! @param _exWhat The exception message
		//! @param _exWhere The location where the exception was thrown from (e.g. function name)
		//! @param _exType The type of the exception
		aException(
			const std::string &			_exWhat,
			const std::string &			_exWhere,
			exceptionType				_exType = exceptionType::Undefined
		);

		//! @brief Assignment constructor. Will set the internal variables
		//! @param _exWhat The exception message
		//! @param _exWhere The location where the exception was thrown from (e.g. function name)
		//! @param _exType The type of the exception
		aException(
			const char *				_exWhat,
			const std::string &			_exWhere,
			exceptionType				_exType = exceptionType::Undefined
		);

		//! @brief Copy constructor. Will copy all information from the other exception.
		//! @param _other The other exception
		aException(
			const aException &			_other
		);

		//! @brief Forward constructor. Will copy all information from the other exception and append the new location
		//! Will copy all the information contained in the other exception. Afterwards the new where location will be appended.
		//! @brief _other The other exception the information will be copied from
		//! @brief _exWhere The new location message which will be appended
		aException(
			const aException &			_other,
			const char *				_exWhere
		);

		//! @brief Deconstructor
		virtual ~aException() throw();

		//!	@brief Will return the exception and location text
		virtual const char * what() const throw() override;

		//!	@brief Returns a copy of the what message which represents the exception text
		std::string getWhat(void) const;

		//! @brief Return a copy of the what message which represents the location
		std::string getWhere(void) const;

		//! @brief Returns a copy of the exception type		
		exceptionType getType(void) const;

	protected:
		std::string						m_what;								//! Contains the exception message
		std::string						m_where;								//! Contains the location desription
		exceptionType					m_type;								//! Contains the exception type which is initialized as undefined

		void buildOut(void);													//! Used internally to build the output string

	private:
		std::string						m_out;									//! Contains the output message which will be returned in the what() function
	};

	class UICORE_API_EXPORT InvalidObjectTypeException : public std::exception {
	public:
		InvalidObjectTypeException() noexcept : std::exception("Invalid object type") {};
	};

	class UICORE_API_EXPORT ItemNotFoundException : public std::exception {
	public:
		ItemNotFoundException() noexcept : std::exception("Item not found") {};
	};

} // namespace ak

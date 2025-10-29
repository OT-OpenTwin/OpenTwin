// @otlicense

/*****************************************************************//**
 * \file   PythonException.h
 * \brief  Embedded python functions don't throw exceptions. If the return value holds an error code that says that the function failed in execution, it is possible to 
 *			extract an error description. Actually it is mandatory to extract the error description, since the internal error flag is reset by doing so. If the error flag 
 *			is still active in the nextt execution of an embedded python function it will fail.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <string>
#include <exception>

class PythonException : public std::exception
{
public:
	PythonException();
	PythonException(const std::string& _msg);

	const char* what(void) const override;

private:
	std::string m_message;
};
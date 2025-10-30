// @otlicense
// File: PythonException.h
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
#include <string>
#include <exception>

//! @brief Embedded python functions don't throw exceptions. If the return value holds an error code that says that the function failed in execution, it is possible to 
//! extract an error description.Actually it is mandatory to extract the error description, since the internal error flag is reset by doing so. If the error flag
//! is still active in the nextt execution of an embedded python function it will fail.
class PythonException : public std::exception
{
public:
	PythonException();
	PythonException(const std::string& _msg);

	const char* what(void) const override;

private:
	std::string m_message;
};
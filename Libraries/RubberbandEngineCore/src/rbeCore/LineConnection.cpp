// @otlicense
// File: LineConnection.cpp
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

// RBE header
#include <rbeCore/LineConnection.h>
#include <rbeCore/Point.h>
#include <rbeCore/jsonMember.h>
#include <rbeCore/rbeAssert.h>

rbeCore::LineConnection::LineConnection() 
	: AbstractConnection(ctLine), m_from(nullptr), m_to(nullptr)
{

}

rbeCore::LineConnection::~LineConnection() {

}

// ###############################################################################

// Base class functions

std::string rbeCore::LineConnection::debugInformation(const std::string& _prefix) {
	std::string ret{ "{\n" };
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_Type "\": \"" RBE_JSON_VALUE_ConnectionType_Line "\",\n");
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_LINE_From "\": ");
	if (m_from) {
		ret.append(m_from->debugInformation(_prefix + "\t")).append("\n");
	}
	else {
		ret.append("NULL,\n");
	}
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_LINE_To "\": ");
	if (m_to) {
		ret.append(m_to->debugInformation(_prefix + "\t")).append("\n");
	}
	else {
		ret.append("NULL,\n");
	}
	ret.append(_prefix).append("}");

	return ret;
}

void rbeCore::LineConnection::addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) {
	if (_first) { _first = false; }
	else { _array << ","; }
	_array << "{\"" RBE_JSON_CONNECTION_Type "\":\"" RBE_JSON_VALUE_ConnectionType_Line "\",\"" RBE_JSON_CONNECTION_LINE_From "\":";	
	if (m_from) {
		m_from->addAsJsonObject(_array);
	}
	else {
		rbeAssert(0, "Data error: Requested connection value but no from was provided");
		_array << "0";
	}
	_array << ",\"" RBE_JSON_CONNECTION_LINE_To "\":";
	if (m_to) {
		m_to->addAsJsonObject(_array);
	}
	else {
		rbeAssert(0, "Data error: Requested connection value but no to was provided");
		_array << "0";
	}
	_array << "}";
}

// ###############################################################################
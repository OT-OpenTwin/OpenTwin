// @otlicense
// File: HistoryConnection.cpp
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
#include <rbeCore/HistoryConnection.h>
#include <rbeCore/Step.h>
#include <rbeCore/jsonMember.h>

rbeCore::HistoryConnection::HistoryConnection(Step * _ref)
	: AbstractConnection(ctHistory), m_ref(_ref)
{}

rbeCore::HistoryConnection::~HistoryConnection() {}

// ###############################################################################

// Base class functions

std::string rbeCore::HistoryConnection::debugInformation(const std::string& _prefix) {
	std::string ret{ "{\n" };
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_Type "\": \"" RBE_JSON_VALUE_ConnectionType_History "\",\n");
	ret.append(_prefix).append("\t\"" RBE_JSON_STEP_Step "\": ");
	if (m_ref) {
		ret.append(std::to_string(m_ref->id())).append("\n");
	}
	else {
		ret.append("NULL,\n");
	}
	ret.append(_prefix).append("}");

	return ret;
}

void rbeCore::HistoryConnection::addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) {
	if (m_ref) {
		m_ref->addConnectionsToJsonArray(_engine, _array, _first, true);
	}
}

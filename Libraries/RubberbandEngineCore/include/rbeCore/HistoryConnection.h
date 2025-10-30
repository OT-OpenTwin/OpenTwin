// @otlicense
// File: HistoryConnection.h
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

// RBE header
#include <rbeCore/AbstractConnection.h>

namespace rbeCore {

	class Step;

	class RBE_API_EXPORT HistoryConnection : public AbstractConnection {
	public:
		HistoryConnection(Step * _ref);

		virtual ~HistoryConnection();

		Step * reference(void) { return m_ref; }

		// ###############################################################################

		// Base class functions

		virtual std::string debugInformation(const std::string& _prefix) override;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) override;

	private:
		Step *		m_ref;

		HistoryConnection() = delete;
		HistoryConnection(HistoryConnection&) = delete;
		HistoryConnection& operator = (HistoryConnection&) = delete;
	};

}
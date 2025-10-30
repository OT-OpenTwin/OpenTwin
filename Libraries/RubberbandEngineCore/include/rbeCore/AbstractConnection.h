// @otlicense
// File: AbstractConnection.h
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
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>
#include <sstream>

namespace rbeCore {

	class RubberbandEngine;

	class RBE_API_EXPORT AbstractConnection {
	public:

		enum connectionType {
			ctLine,
			ctCircle,
			ctHistory
		};

		AbstractConnection(connectionType _type);
		virtual ~AbstractConnection() {}

		// ######################################################################################

		// Getter

		connectionType type(void) const { return m_type; }

		bool ignoreInHistory(void) const { return m_ignoreInHistory; }

		// ######################################################################################

		// Setter

		void setIgnoreInHistory(bool _ignore) { m_ignoreInHistory = _ignore; }

		// ######################################################################################

		// Abstract functions

		virtual std::string debugInformation(const std::string& _prefix) = 0;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) = 0;

	private:
		connectionType		m_type;
		bool				m_ignoreInHistory;

		AbstractConnection() = delete;
		AbstractConnection(AbstractConnection&) = delete;
		AbstractConnection& operator = (AbstractConnection&) = delete;
	};

}
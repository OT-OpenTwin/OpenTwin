// @otlicense
// File: Connector.h
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
#pragma warning(disable:4251)

#include <string>
#include "SerializeWithBSON.h"

namespace ot
{
	enum ConnectorType {In = 0, Out = 1, InOptional = 2, Any = 3, UNKNOWN = -1};

	class __declspec(dllexport) Connector : public SerializeWithBSON
	{
	public:
		Connector();
		~Connector()
		{

		}
		Connector(ConnectorType type, const std::string& connectorName, const std::string& connectorTitle);
		bool operator==(const Connector& other) { return _connectorName == other._connectorName; }
		
		const ConnectorType getConnectorType() const { return _connectorType; }
		const std::string& getConnectorName() const { return _connectorName; }
		const std::string& getConnectorTitle() const { return _connectorTitle; }
		
		virtual bsoncxx::builder::basic::document SerializeBSON() const  override;
		virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;

	private:
		ConnectorType _connectorType;
		std::string _connectorName;
		std::string _connectorTitle;

	};
}
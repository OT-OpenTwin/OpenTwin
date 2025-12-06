// @otlicense
// File: Connector.cpp
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

#include "Connector.h"
#include <bsoncxx/builder/basic/kvp.hpp>

ot::Connector::Connector(ConnectorType type, const std::string& connectorName, const std::string& connectorTitle)
	:_connectorType(type), _connectorName(connectorName), _connectorTitle(connectorTitle)
{
}

ot::Connector::Connector()
	:_connectorType(UNKNOWN), _connectorName("")
{
}

bsoncxx::builder::basic::document ot::Connector::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubDocument;
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorName", _connectorName));
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorType", _connectorType));
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorTitle", _connectorTitle));
	return newSubDocument;
}

void ot::Connector::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connectorName = std::string(storage.view()["connectorName"].get_string().value.data());
	_connectorType = static_cast<ConnectorType>(storage.view()["connectorType"].get_int32().value);
	_connectorTitle = std::string(storage.view()["connectorTitle"].get_string().value.data());
}

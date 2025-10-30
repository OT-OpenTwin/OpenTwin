// @otlicense
// File: DataBuffer.h
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
#include <list>
#include <optional>
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/ReturnValues.h"
#include "PortData.h"
#include "CPythonObject.h"

class DataBuffer {
public:
	static DataBuffer& instance(void);

	void addNewPortData(const std::string& _portName, const std::string& _serialisedData, const std::string& _serialisedMetaData);
	void overridePortData(const std::string& _portName, const std::string& _serialisedData);
	void overridePortMetaData(const std::string& _portName, const std::string& _serialisedMetaData);

	void clearData();
	PyObject* getPortData(const std::string& _portName);
	PyObject* getPortMetaData(const std::string& _portName);

	std::list<PortData*> getModifiedPortData();
	void addReturnData(const std::string& _scriptName,const std::string& _returnData);
	std::map<std::string, ot::JsonDocument>& getReturnDataByScriptName() { return m_returnDataByScriptName; }

private:
	std::map<std::string, PortData> m_portData;
	std::map<std::string,ot::JsonDocument> m_returnDataByScriptName;
};

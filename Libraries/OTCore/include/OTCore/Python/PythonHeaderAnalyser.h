// @otlicense
// File: PythonHeaderAnalyser.h
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
#include <map>
#include "OTCore/JSON/JSON.h"
#include "OTCore/Python/PythonHeaderEntryType.h"
#include "OTCore/CoreAPIExport.h"

//! @brief Extracts python header of the form: #@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
//! The header type is separated from the following json document. All keys in the json document are cleaned of special characters and whitespaces and converted to lower case.
class OT_CORE_API_EXPORT PythonHeaderAnalyser
{
public:
	struct ExtractedEntry
	{
		int m_lineNumber;
		ot::JsonDocument m_content;
	};

	PythonHeaderAnalyser();
	~PythonHeaderAnalyser() = default;
	PythonHeaderAnalyser(const PythonHeaderAnalyser& _other) = delete;
	PythonHeaderAnalyser(PythonHeaderAnalyser&& _other) = default;
	PythonHeaderAnalyser& operator=(const PythonHeaderAnalyser& _other) = delete;
	PythonHeaderAnalyser& operator=(PythonHeaderAnalyser&& _other) = delete;

	bool analysePythonScript(const std::string& _scriptContent);
	const std::list<ExtractedEntry>& getEntriesOfType(PythonHeaderEntryType _type) const;
	const std::string& getAnalysisReport() const { return m_report; }

private:
	std::map< PythonHeaderEntryType, std::list<ExtractedEntry>> m_entriesByType;
	const std::map<std::string, PythonHeaderEntryType> m_types;
	std::string m_report;

	PythonHeaderEntryType extractType(const std::string& _lineContent);
	ot::JsonDocument extractDetails(const std::string& _lineContent);
	ot::JsonDocument cleanJSONKeys(ot::JsonDocument&& _doc);
};

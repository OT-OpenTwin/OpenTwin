// @otlicense
// File: GenericDocument.h
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
#include <map>
#include <stdint.h>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include "OTCore/Variable.h"

class __declspec(dllexport) GenericDocument
{
public:
	const std::string getDocumentName() const { return m_documentName; }
	const std::vector<const GenericDocument *> getSubDocuments() const { return m_subDocuments; }
	void setDocumentName(std::string _documentName) { m_documentName = _documentName; };
	const std::map<std::string, std::list<ot::Variable>>* getFields() const { return &m_fields; };

	void addSubDocument(GenericDocument* _subDocument) { m_subDocuments.push_back(_subDocument); };
	void clearSubDocuments() { m_subDocuments.clear(); }
	void insertInDocumentField(const std::string& _fieldName, const std::list<ot::Variable>& _values);
	void insertInDocumentField(const std::string& _fieldName, std::list<ot::Variable>&& _values);

protected:

	std::map<std::string, std::list<ot::Variable>> m_fields;
	std::string m_documentName;
	std::vector<const GenericDocument*> m_subDocuments;

	virtual void checkForIlligalName(std::string _fieldName) {};
};
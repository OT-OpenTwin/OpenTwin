// @otlicense
// File: Application.h
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

// projct header
#include "Service.h"

// std header
#include <list>
#include <map>

struct ParseError {
	std::string file;
	size_t lineNumber;
	std::string serviceName;
	std::string errorMessage;
	std::string context;

	std::string toString() const {
		std::string parseErrorAsString = "at " + serviceName + " - " + file + " - line " + std::to_string(lineNumber) + " - " + errorMessage;

		if (!context.empty()) {
			parseErrorAsString += " - Context: " + context;
		}

		return parseErrorAsString;
	}
};

class Application {
public:
	enum ParameterType{
		FunctionParam,
		ReturnParam
	};

	// Constructors
	Application() = default;
	Application(const Application& _other) = default;
	Application(Application&& _other) noexcept = default;

	~Application() = default;

	Application& operator = (const Application& _other) = default;
	Application& operator = (Application&& _other) noexcept = default;

	// Getter, Setter
	void setServices(const std::list<Service>& _services) { m_services = _services; };
	const std::list<Service>& getServices() const { return m_services; };

	// Other methods
	int run(void);

	bool searchForServices(void);
	
	bool searchIncludeAndSrcDirectoryFiles(const std::string& _file, Service& _service);

	bool searchSrcDirectoryFiles(const std::string& _srcDirectory, Service& _service);

	bool searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service);

	bool parseFile(const std::string& _file, Service& _service);

	static bool startsWith(const std::string& _line, const std::string& _prefix);

	bool parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, Service& _service, const std::string& _file, const size_t& _lineNumber, ParameterType _parameterType);

	void addService(const Service& _service);

	void importActionTypes(void);

	void parseMacroDefinition(const std::string& _content);

	void addDescriptionToLastParameter(std::list<Parameter>& _paramList, const std::string& _description);
	
	bool generateDocumentation(const std::list<Service>& m_services);

	std::string generateServiceRstContent(const Service& _service);

	bool writeServiceRstFile(const std::string& _path, const std::string& _rst);

	bool documentParseErrors(void);

	void reportError(const ParseError& _error);
	
	std::string generateAllErrorsTxtContent(void);

	bool writeAllErrorsTxtFile(const std::string& _txt);

	// helper functions
	std::string getPathFromEnvironmentVariable(const std::string& _envVar, const std::string& _subPath);
	std::string getPathToOTServices(void);
	std::string getPathToOTDocumentation(void);
	std::string getPathToOTEndPointDocParser(void);

	std::string serviceNameToSnakeCase(const std::string& _serviceName);

private:
	std::list<Service> m_services;
	std::map<std::string, std::string> m_actionMacros;
	std::list<ParseError> m_parseErrors;
};
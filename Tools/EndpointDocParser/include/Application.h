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
	std::string errorMessage;
	std::string context;

	std::string toString() const {
		std::string parseErrorAsString = "at " + file + " - line " + std::to_string(lineNumber) + " - " + errorMessage;

		if (!context.empty()) {
			parseErrorAsString += " - Context: " + context;
		}

		return parseErrorAsString;
	}
};

struct UndocumentedEndpoint {
	std::string action;
	std::string serviceName;
	std::string file;
	size_t lineNumber;

	std::string toString() const {
		return " at " + file + " - line " + std::to_string(lineNumber) + " - " + "Action: " + action;
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

	//! @brief Goes through all include and src directory files of the OpenTwin services
	//! 
	//! Searches line by line for:
	//! - Prefix "//api":
	//!   - Indicates the beginning of an API documentation block
	//!   - Or indicates that you are in an API documentation block if the "inApiBlock" flag is also set
	//!   - Recognizes the end of an API documentation block if the prefix is missing but the flag is still set
	//! - Prefix "connectAction":
	//!   - As an indicator for endpoints to be documented
	//! 
	//! Operations performed:
	//! - Parses an API documentation block and adds an error-free documented endpoint to the list of endpoints "m_endpoints" in the service
	//! - Adds a service that has documented endpoints to the list of services "m_services"
	//! - Collects incorrectly documented endpoints in the "m_parseErrors" list
	//! - Collects endpoints to be documented in the "m_endpointsToBeDocumented" list
	//! 
	//! @return True if there were serious errors (unable to open the file, etc.), false otherwise
	bool searchForServices(void);
	
	bool searchIncludeAndSrcDirectoryFiles(const std::string& _file, Service& _service);

	bool searchSrcDirectoryFiles(const std::string& _srcDirectory, Service& _service);

	bool searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service);

	bool parseFile(const std::string& _file, Service& _service);

	static bool startsWith(const std::string& _line, const std::string& _prefix);

	bool parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, Service& _service, const std::string& _file, const size_t& _lineNumber, ParameterType _parameterType);

	void addService(const Service& _service);

	//! @brief Parses the ActionTypes.h file of the OTCommunication library
	//! 
	//! Operations performed:
	//! - Searches for the prefix "#define" to identify the relevant lines
	//! - Fills the map "m_actionMacros" with entries consisting of Macro : Definition
	//! 
	//! Takes various cases into account:
	//! - OT_ACTION_PASSWORD_SUBTEXT "Password"
	//! - OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE "CST Studio Suite"
	//! - OT_ACTION_RETURN_UnknownError OT_ACTION_RETURN_INDICATOR_Error "Unknown error"
	//! - OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD "LoggedInUser" OT_ACTION_PASSWORD_SUBTEXT
	//! - OT_PARAM_AUTH_PASSWORD OT_ACTION_PASSWORD_SUBTEXT
	void importActionTypes(void);

	void parseMacroDefinition(const std::string& _content);

	void addDescriptionToLastParameter(std::list<Parameter>& _paramList, const std::string& _description);
	
	//! @brief Generates the endpoint documentation in Sphinx 
	//! 
	//! Operations performed:
	//! - For each service from the list of services:
	//!   - creates a syntactically correct Sphinx documentation
	//!   - writes its content to an .rst file carrying the name of the service
	//! - Then creates the index file "documented_endpoints.rst" and fills the table of contents with references to all .rst files created for the services
	//! 
	//! @return True if there were serious errors (unable to open the file, etc.), false otherwise
	bool generateDocumentation(const std::list<Service>& m_services);

	std::string generateServiceRstContent(const Service& _service);

	bool writeServiceRstFile(const std::string& _path, const std::string& _rst);

	bool updateDocumentedEndpointsIndex(const std::list<Service>& m_services);

	std::string generateDocumentedEndpointsIndexContent(const std::list<std::string>& _serviceFileNames);

	bool writeDocumentedEndpointsIndexFile(const std::string& _path, const std::string& _content);

	//! @brief Reports incorrectly documented endpoints
	//! 
	//! Writes all incorrectly documented endpoints collected in "m_parseErrors" to a file named "parseErrors.txt" and stores it in the EnpointDocParser directory
	bool documentParseErrors(void);

	void reportError(const ParseError& _error);
	
	std::string generateAllErrorsTxtContent(void);

	bool writeAllErrorsTxtFile(const std::string& _txt);

	//! @brief Searches for endpoints to be documented and reports those that are still undocumented
	//! 
	//! Operations performed:
	//! - Uses the endpoint action to compare whether the endpoints to be documented, which are contained in the "m_endpointsToBeDocumented" list, are also present in the list of documented endpoints "m_endpoints" in the relevant service
	//! - If not present, an OT_LOG_W is thrown for undocumented endpoints and the number of undocumented endpoints is output as OT_LOG_D
	void reportEndpointsToBeDocumented();

	// helper functions
	std::string getPathFromEnvironmentVariable(const std::string& _envVar, const std::string& _subPath);
	std::string getPathToOTServices(void);
	std::string getPathToOTDocumentation(void);
	std::string getPathToOTEndPointDocParser(void);

	std::string serviceNameToSnakeCase(const std::string& _serviceName);
	std::string endpointNameToKebabCase(const std::string& _endpointName);

private:
	std::list<Service> m_services;
	std::map<std::string, std::string> m_actionMacros;
	std::list<ParseError> m_parseErrors;
	std::list<UndocumentedEndpoint> m_endpointsToBeDocumented;
};
// @otlicense
// File: Application.cpp
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

// Open Twin header
#include "OTSystem/FileSystem.h"
#include "OTSystem/Exception.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"

// project header
#include "Application.h"
#include "Service.h"

// std header
#include <iostream>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;


int Application::run(void) {
	int exitCode = 0;

	OT_LOG_D("Application is running.");
	importActionTypes();
	
	if (searchForServices()) {
		exitCode = 1;
	}

	if (generateDocumentation()) {
		exitCode = 2;
	}

	if (documentParseErrors()) {
		exitCode = 3;
	}

	reportEndpointsToBeDocumented();

	return exitCode;
}

bool Application::searchForServices(void) {
	OT_LOG_D("Searching for Services.");

	// search in Open Twin Services
	const std::string path = getPathToOTServices();
	std::list<Service> services;
	bool hasError = false;

	try {
		std::list<std::string> allServices = ot::FileSystem::getFiles(path, { ".vcxproj" }, ot::FileSystem::FileSystemOption::Recursive);

		for (const std::string& file : allServices) {

			// get the name of the service
			fs::path p = file;
			std::string serviceName = p.stem().string();

			// create the service
			Service service;
			service.setName(serviceName);
			OT_LOG_D("Created Service " + service.getName() + ".");

			hasError |= searchIncludeAndSrcDirectoryFiles(file, service);

			service.printService();

			// add the service to the list of services if it has correctly documented endpoints
			if (!service.getEndpoints().empty()) {
				addService(service);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		OT_LOG_E("Error by getFiles: " + std::string(e.what()));
		return true;
	}
	// print the list of services
	OT_LOG_D(std::to_string(m_services.size()) + " Services added to List of services: ");
	for (const Service& service : m_services) {
		service.printService();
		// print the endpoints contained in the service
		for (const Endpoint& endpoint : service.getEndpoints()) {
			endpoint.printEndpoint();
		}
	}
	return hasError;
}

bool Application::searchIncludeAndSrcDirectoryFiles(const std::string& _file, Service& _service) {
	OT_LOG_D("Searching for include und src directories of the given file: " + _file);

	// get the path to the .vcxproj file
	fs::path p = _file;
	fs::path parentDirectory = p.parent_path();

	// get the path to the include-directory
	fs::path includeDir = parentDirectory / "include";

	bool hasError = false;

	if (fs::exists(includeDir) && fs::is_directory(includeDir)) {
		hasError |= searchIncludeDirectoryFiles(includeDir.string(), _service);
	}

	// get the path to the src-directory
	fs::path srcDir = parentDirectory / "src";

	if (fs::exists(srcDir) && fs::is_directory(srcDir)) {
		hasError |= searchSrcDirectoryFiles(srcDir.string(), _service);
	}
	return hasError;
}

bool Application::searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service) {
	try {
		// collect all files in the include directory
		std::list<std::string>  includeFiles = ot::FileSystem::getFiles(_includeDirectory, {});

		bool hasError = false;

		// parse each file
		for (const std::string& file : includeFiles) {
			hasError |= parseFile(file, _service);
		}
		return hasError;
	}
	catch (const std::filesystem::filesystem_error& e) {
		OT_LOG_E("Error by getFiles: " + std::string(e.what()));
		return true;
	}
}

bool Application::searchSrcDirectoryFiles(const std::string& _srcDirectory, Service& _service) {
	try {
		// collect all files in the src directory
		std::list<std::string> srcFiles = ot::FileSystem::getFiles(_srcDirectory, {});

		bool hasError = false;

		// parse each file
		for (const std::string& file : srcFiles) {
			hasError |= parseFile(file, _service);
		}
		return hasError;
	}
	catch (const std::filesystem::filesystem_error& e) {
		OT_LOG_E("Error by getFiles: " + std::string(e.what()));
		return true;
	}
}

bool Application::parseFile(const std::string& _file, Service& _service) {
	OT_LOG_D("Parsing file: " + _file);
	std::string blackList = " \t\n";

	Endpoint endpoint;
	Parameter parameter;
	bool inApiBlock = false;
	bool inBriefDescriptionBlock = false;
	bool inResponseDescriptionBlock = false;
	bool inParameterBlock = false;
	bool inReturnParameterBlock = false;
	bool inNoteBlock = false;
	bool inWarningBlock = false;
	bool hasError = false;  // track errors but don't stop parsing
	size_t lineNumber = 0;

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(_file);

		for (const std::string& line : lines) {
			lineNumber++;
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
			std::string lowerCaseTrimmedLine = ot::String::toLower(trimmedLine);

			// check if the parser is in an api documentation block
			if (startsWith(lowerCaseTrimmedLine, "//api")) {
				if (!inApiBlock) {
					inApiBlock = true;
					endpoint = Endpoint();  // new endpoint
					OT_LOG_D("-----------------------------------------------------------------------------------");
					OT_LOG_D("Detected start of api documentation block.");
				}

				// remove "//api " prefix
				std::string apiContent = trimmedLine.substr(5);
				if (!apiContent.empty()) {
					apiContent = ot::String::removePrefixSuffix(apiContent, " \t");
				}
				OT_LOG_D(apiContent);

				std::string lowerCaseApiContent = ot::String::toLower(apiContent);

				// check which "@commando" is given
				// detect @security
				if (startsWith(lowerCaseApiContent, "@security ")) {
					std::string security = lowerCaseApiContent.substr(10);
					security = ot::String::removePrefixSuffix(security, " \t");

					if (security == "tls") {
						endpoint.setMessageType(Endpoint::TLS);
						OT_LOG_D("Message Type TLS set in endpoint: " + endpoint.getMessageTypeString());
					}
					else if (security == "mtls") {
						endpoint.setMessageType(Endpoint::mTLS);
						OT_LOG_D("Message Type mTLS set in endpoint: " + endpoint.getMessageTypeString());
					}
					else {
						ParseError error{
							_file,
							lineNumber,
							"Message Type must be TLS or mTLS, got: " + security,
							apiContent
						};
						reportError(error);
						hasError = true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect @action
				else if (startsWith(lowerCaseApiContent, "@action ")) {
					std::string action = apiContent.substr(8);
					action = ot::String::removePrefixSuffix(action, " \t");

					// endpoint action
					endpoint.setAction(action);
					OT_LOG_D("Action set in endpoint: " + endpoint.getAction());

					// check wether the endpoint action exists and get the name of the endpoint
					if (m_actionMacros.find(action) != m_actionMacros.end()) {
						std::string actionName = m_actionMacros.at(action);
						endpoint.setName(actionName);
						OT_LOG_D("Name set in endpoint: " + endpoint.getName());
					}
					else {
						ParseError error{
							_file,
							lineNumber,
							"Key not found in m_actionMacros: " + action,
							apiContent
						};
						reportError(error);
						hasError = true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect @brief which starts a brief description block
				else if (startsWith(lowerCaseApiContent, "@brief ")) {
					std::string brief = apiContent.substr(7);
					brief = ot::String::removePrefixSuffix(brief, " \t");

					endpoint.setBriefDescription(brief);
					OT_LOG_D("Brief description set in endpoint: " + endpoint.getBriefDescription());

					inBriefDescriptionBlock = true;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect @param
				else if (startsWith(lowerCaseApiContent, "@param ")) {
					parameter = Parameter();

					std::string param = apiContent.substr(7);
					param = ot::String::removePrefixSuffix(param, " \t");
					
					if (parseParameter(parameter, param, endpoint, _service, _file, lineNumber, ParameterType::FunctionParam)) {
						ParseError error{
							_file,
							lineNumber,
							"Failed to parse parameter.",
							apiContent
						};
						reportError(error);
						hasError = true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = true;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect @return which starts a response description block
				else if (startsWith(lowerCaseApiContent, "@return ")) {
					std::string response = apiContent.substr(8);
					response = ot::String::removePrefixSuffix(response, " \t");

					endpoint.addResponseDescription(response);
					OT_LOG_D("Response set in endpoint:");
					endpoint.printResponseDescription();

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = true;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect @rparam
				else if (startsWith(lowerCaseApiContent, "@rparam ")) {
					parameter = Parameter();

					std::string rparam = apiContent.substr(8);
					rparam = ot::String::removePrefixSuffix(rparam, " \t");

					if (parseParameter(parameter, rparam, endpoint, _service, _file, lineNumber, ParameterType::ReturnParam)) {
						ParseError error{
							_file,
							lineNumber,
							"Failed to parse response parameter.",
							apiContent
						};
						reportError(error);
						hasError = true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = true;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				// detect a brief- or response description block
				else if (inBriefDescriptionBlock || inResponseDescriptionBlock) {
					// detect @note which starts a note block
					if (startsWith(lowerCaseApiContent, "@note ")) {
						OT_LOG_D("Detected @note.");
						inNoteBlock = true;
						inWarningBlock = false;

						apiContent = apiContent.substr(6);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");
						apiContent = "@note " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					// detect @warning which starts a warning block
					else if (startsWith(lowerCaseApiContent, "@warning ")) {
						OT_LOG_D("Detected @warning.");
						inNoteBlock = false;
						inWarningBlock = true;

						apiContent = apiContent.substr(9);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");
						apiContent = "@warning " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					// detect @endnote which ends a note block
					else if (startsWith(lowerCaseApiContent, "@endnote")) {
						apiContent = apiContent.substr(8);

						if (!apiContent.empty()) {
							ParseError error{
								_file,
								lineNumber,
								"Invalid input: @endnote must be empty.",
								apiContent
							};
							reportError(error);
							hasError = true;
						}

						inNoteBlock = false;
						inWarningBlock = false;
					}
					// detect @endwarning which ends a warning block
					else if (startsWith(lowerCaseApiContent, "@endwarning")) {
						apiContent = apiContent.substr(11);

						if (!apiContent.empty()) {
							ParseError error{
								_file,
								lineNumber,
								"Invalid input: @endwarning must be empty.",
								apiContent
							};
							reportError(error);
							hasError = true;
						}

						inNoteBlock = false;
						inWarningBlock = false;
					}
					// detect a note block
					else if (inNoteBlock) {
						apiContent = "@note " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					// detect a warning block
					else if (inWarningBlock) {
						apiContent = "@warning " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					// detect a normal description
					else {
						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}

					if (inBriefDescriptionBlock) {
						OT_LOG_D("Detailed description set in endpoint:");
						endpoint.printDetailedDescription();
					}
					else {
						OT_LOG_D("Response description set in endpoint:");
						endpoint.printResponseDescription();
					}
					OT_LOG_D("---");
				}
				// detect a parameter- or return parameter description block
				else if (inParameterBlock || inReturnParameterBlock) {
					std::list<Parameter>& paramList = inParameterBlock ?
						endpoint.getParameters() :
						endpoint.getResponseParameters();

					// detect @note which starts a note block
					if (startsWith(lowerCaseApiContent, "@note ")) {
						OT_LOG_D("Detected @note.");
						inNoteBlock = true;
						inWarningBlock = false;

						apiContent = apiContent.substr(6);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");

						addDescriptionToLastParameter(paramList, "@note " + apiContent);
					}
					// detect @warning which starts a warning block
					else if (startsWith(lowerCaseApiContent, "@warning ")) {
						OT_LOG_D("Detected @warning.");
						inNoteBlock = false;
						inWarningBlock = true;

						apiContent = apiContent.substr(9);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");

						addDescriptionToLastParameter(paramList, "@warning " + apiContent);
					}
					// detect @endnote which ends a note block
					else if (startsWith(lowerCaseApiContent, "@endnote")) {
						apiContent = apiContent.substr(8);

						if (!apiContent.empty()) {
							ParseError error{
								_file,
								lineNumber,
								"Invalid input: @endnote must be empty.",
								apiContent
							};
							reportError(error);
							hasError = true;
						}

						inNoteBlock = false;
						inWarningBlock = false;
					}
					// detect @endwarning which ends a warning block
					else if (startsWith(lowerCaseApiContent, "@endwarning")) {
						apiContent = apiContent.substr(11);

						if (!apiContent.empty()) {
							ParseError error{
								_file,
								lineNumber,
								"Invalid input: @endwarning must be empty.",
								apiContent
							};
							reportError(error);
							hasError = true;
						}

						inNoteBlock = false;
						inWarningBlock = false;
					}
					// detect a note block
					else if (inNoteBlock) {
						addDescriptionToLastParameter(paramList, "@note " + apiContent);
					}
					// detect a warning block
					else if (inWarningBlock) {
						addDescriptionToLastParameter(paramList, "@warning " + apiContent);
					}
					// detect a normal description
					else {
						addDescriptionToLastParameter(paramList, apiContent);
					}
				}
				else {
					ParseError error{
								_file,
								lineNumber,
								"Unknown content.",
								apiContent
					};
					reportError(error);
					hasError = true;
				}
			}
			// detect the end of the api block
			else if (inApiBlock) {

				// report any syntax errors in the documentation and do not add the endpoint to the service
				if (hasError) {
					ParseError error{
							_file,
							lineNumber,
							"Error(s) by parsing " + endpoint.getAction() + ". Enpoint was not added to service.",
							""
					};
					reportError(error);
				}

				// add the endpoint to the service if name and brief description have been set
				else if (!endpoint.getName().empty() && !endpoint.getBriefDescription().empty()) {
					// set mTLS as the default message type, if no message type has been set
					if (endpoint.getMessageType() == Endpoint::unknown) {
						endpoint.setMessageType(Endpoint::mTLS);

						ParseError error{
							_file,
							lineNumber,
							"Message type is missing, default message type mTLS was set.",
							""
						};
						reportError(error);
					}

					OT_LOG_D("The parsed endpoint is:");
					endpoint.printEndpoint();

					OT_LOG_D("The service is " + _service.getName() + ".");

					_service.addEndpoint(endpoint);
					OT_LOG_D("Added endpoint to service.");
					_service.printService();
				}

				// give an error message if name, brief description or message type have not been set
				// and do not add the endpoint to the service
				else {
					std::vector<std::string> missingItems;

					if (endpoint.getName().empty()) {
						missingItems.push_back("name");
					}
					if (endpoint.getBriefDescription().empty()) {
						missingItems.push_back("brief description");
					}
					if (endpoint.getMessageType() == Endpoint::unknown) {
						missingItems.push_back("message type");
					}

					std::string missing;
					for (size_t i = 0; i < missingItems.size(); ++i) {
						if (i > 0) {
							missing += (i == missingItems.size() - 1) ? " and " : ", ";
						}
						missing += missingItems[i];
					}

					ParseError error{
							_file,
							lineNumber,
							"Invalid Endpoint can't be added to service - missing: " + missing,
							""
					};
					reportError(error);
				}

				// Reset the state for the next endpoint
				inApiBlock = false;
				OT_LOG_D("Detected end of api documentation block.");
				OT_LOG_D("-----------------------------------------------------------------------------------");

				inBriefDescriptionBlock = false;
				inResponseDescriptionBlock = false;
				inParameterBlock = false;
				inReturnParameterBlock = false;
				inNoteBlock = false;
				inWarningBlock = false;
				hasError = false;
			}			
			else {
				// detected connectAction as indicator for an endpoint that needs to be documented
				if (startsWith(trimmedLine, "connectAction(")) {
					OT_LOG_D("Detected connectAction: " + trimmedLine);

					// extract endpoint action
					std::string content = trimmedLine.substr(14);					
					std::list<std::string> splittedContentList = ot::String::split(content, " ");
					std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
					std::string endpointAction = splittedContentVector[0];
					endpointAction = ot::String::removeSuffix(endpointAction, ",");
					
					UndocumentedEndpoint undocumented{
							endpointAction,
							_service.getName(),
							_file,
							lineNumber
					};

					// save the endpoint in the list of endpoints to be documented
					m_endpointsToBeDocumented.push_back(undocumented);

					OT_LOG_D("Stored undocumented endpoint: " + undocumented.toString());
				}
			}
		}
	}
	catch (const ot::FileOpenException& e) {
		OT_LOG_E("Error by readLines: " + std::string(e.what()));
		return true;
	}
	catch (const std::ios_base::failure& e) {
		OT_LOG_E("IO-failure: " + std::string(e.what()));
		return true;
	}

	return false;
}

// returns true if the line starts with the given prefix
bool Application::startsWith(const std::string& _line, const std::string& _prefix) {
	return _line.compare(0, _prefix.size(), _prefix) == 0;
}

bool Application::parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, Service& _service, const std::string& _file, const size_t& _lineNumber, ParameterType _parameterType) {
	OT_LOG_D("Parsing parameter: " + _param);
	std::list<std::string> splittedParamList = ot::String::split(_param, " ");
	std::vector<std::string> splittedParamVector(splittedParamList.begin(), splittedParamList.end());

	// first string = macro
	std::string macro = splittedParamVector[0];
	std::size_t sizeOfMacroString = macro.size() + 1;

	_parameter.setMacro(macro);

	// parameter name 
	if (m_actionMacros.find(macro) != m_actionMacros.end()) {
		std::string name = m_actionMacros.at(macro);
		_parameter.setName(name);
	}
	else {
		ParseError error{
			_file,
			_lineNumber,
			"Key not found in m_actionMacros: " + macro,
			""
		};
		reportError(error);
		return true;
	}

	// second string = data type
	std::string dataType = splittedParamVector[1];
	dataType = ot::String::toLower(dataType);

	// data type is Unsigned Integer 64
	if (dataType == "unsigned") {
		std::string dataType2 = splittedParamVector[2];
		std::string dataType3 = splittedParamVector[3];
		std::string unsignedInt64 = dataType + " " + dataType2 + " " + dataType3;

		std::size_t sizeOfUnsignedInt64String = unsignedInt64.size() + 1;

		_parameter.setDataType(Parameter::UnsignedInteger64);

		// third string = description
		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfUnsignedInt64String);
		_parameter.addDescription(paramDescription);
	}
	else {
		std::size_t sizeOfDataTypeString = dataType.size() + 1;

		// data type is Unsigned Integer 64
		if (dataType == "uid") {
			_parameter.setDataType(Parameter::UnsignedInteger64);
		}
		// data type is boolean
		else if (dataType == "boolean" || dataType == "bool") {
			_parameter.setDataType(Parameter::Boolean);
		}
		// data type is character
		else if (dataType == "char" || dataType == "character") {
			_parameter.setDataType(Parameter::Char);
		}
		// data type is integer
		else if (dataType == "integer" || dataType == "int") {
			_parameter.setDataType(Parameter::Integer);
		}
		// data type is float
		else if (dataType == "float") {
			_parameter.setDataType(Parameter::Float);
		}
		// data type is double
		else if (dataType == "double") {
			_parameter.setDataType(Parameter::Double);
		}
		// data type is string
		else if (dataType == "string" || dataType == "str") {
			_parameter.setDataType(Parameter::String);
		}
		// data type is array
		else if (dataType == "array") {
			_parameter.setDataType(Parameter::Array);
		}
		// data type is object
		else if (dataType == "object" || dataType == "obj") {
			_parameter.setDataType(Parameter::Object);
		}
		// data type is enum
		else if (dataType == "enum") {
			_parameter.setDataType(Parameter::Enum);
		}
		// data type is invalid
		else {
			ParseError error{
				_file,
				_lineNumber,
				"Invalid parameter datatype: " + dataType,
				""
			};
			reportError(error);
			return true;
		}

		// third string = description
		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfDataTypeString);
		_parameter.addDescription(paramDescription);
	}

	_parameter.printParameter();

	// add the parsed parameter to the list of parameters or return parameters
	switch (_parameterType) {
	case Application::FunctionParam:
		_endpoint.addParameter(_parameter);
		OT_LOG_D("Added Parameter to parameters.");
		break;
	case Application::ReturnParam:
		_endpoint.addResponseParameter(_parameter);
		OT_LOG_D("Added Parameter to response parameters.");
		break;
	default:
		ParseError error{
				_file,
				_lineNumber,
				"Unknown parameter type: " + _parameterType,
				""
		};
		reportError(error);
		return true;
		break;
	}
	return false;
}

void Application::addService(const Service& _service) {
	OT_LOG_D("Adding Service " + _service.getName() + " to List of Services.");
	m_services.push_back(_service);
}

void Application::importActionTypes(void) {
	// search in ActionTypes.h in Open Twin OTCommunication Library
	const std::string pathToActionTypesHeaderFile = getPathToOTLibraryOTCommunication();
	std::string blackList = " \t\n";

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(pathToActionTypesHeaderFile);

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);

			// detect Macro-definition
			if (startsWith(trimmedLine, "#define")) {
				// remove "#define " prefix
				std::string content = trimmedLine.substr(8);

				parseMacroDefinition(content);
			}
		}

		OT_LOG_D("The parsed map of macro-name and -definition is:");
		for (const auto macro : m_actionMacros) {
			OT_LOG_D(macro.first + " : >>" + macro.second + "<<");
		}
	}
	catch (const ot::FileOpenException& e) {
		OT_LOG_E("Error by readLines: " + std::string(e.what()));
	}
	catch (const std::ios_base::failure& e) {
		OT_LOG_E("IO-failure: " + std::string(e.what()));
	}
}

void Application::parseMacroDefinition(const std::string& _content) {
	std::string blackList = " ";

	// macro = MACRO_NAME + "macro definition"

	// macro contains macro definition in quotes
	// e.g. OT_ACTION_PASSWORD_SUBTEXT "Password"
	if (_content.find("\"") != std::string::npos) {
		std::list<std::string> splittedContentList = ot::String::split(_content, "\"");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = ot::String::removeSuffix(splittedContentVector[0], blackList);
		std::string macroDefinition = splittedContentVector[1];
//		OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

		// macro contains two macro names at the beginning
		if (macroName.find(" ") != std::string::npos) {
			std::list<std::string> splittedMacroNameList = ot::String::split(macroName, " ");
			std::vector<std::string> splittedMacroNameVector(splittedMacroNameList.begin(), splittedMacroNameList.end());
			
			macroName = splittedMacroNameVector[0];

//			OT_LOG_D(">>" + macroName + "<<:>>" + splittedMacroNameVector[1] + "<<:>>" + macroDefinition + "<<");
			
			// second macro name is OT_ACTION_RETURN_INDICATOR_Error, replace with "Error: "
			// e.g. OT_ACTION_RETURN_UnknownError OT_ACTION_RETURN_INDICATOR_Error "Unknown error"
			if (splittedMacroNameVector[1] == "OT_ACTION_RETURN_INDICATOR_Error") {
				macroDefinition = "Error: " + macroDefinition;

//				OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");
				
				m_actionMacros[macroName] = macroDefinition;
			}
		}
		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		// e.g. OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD "LoggedInUser" OT_ACTION_PASSWORD_SUBTEXT
		else if (splittedContentVector.size() > 2 && splittedContentVector[2] != "") {
			splittedContentVector[2] = ot::String::removePrefix(splittedContentVector[2], blackList);
//			OT_LOG_D(splittedContentVector[2]);
			
			if (splittedContentVector[2] == "OT_ACTION_PASSWORD_SUBTEXT") {
				macroDefinition = splittedContentVector[1] + "Password";

//				OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

				m_actionMacros[macroName] = macroDefinition;
			}	
		}
		else {
			m_actionMacros[macroName] = macroDefinition;
		}
	}
	// macro definition is not in quotation marks
	else {
		std::list<std::string> splittedContentList = ot::String::split(_content, " ");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = splittedContentVector[0];

		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		// e.g. OT_PARAM_AUTH_PASSWORD OT_ACTION_PASSWORD_SUBTEXT
		if (splittedContentVector[1] == "OT_ACTION_PASSWORD_SUBTEXT") {
			std::string macroDefinition = "Password";

//			OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

			m_actionMacros[macroName] = macroDefinition;
		}
	}
}

void Application::addDescriptionToLastParameter(std::list<Parameter>& _paramList, const std::string& _description) {
	if (!_paramList.empty()) {
		Parameter& lastParam = _paramList.back();
		lastParam.addDescription(_description);
		OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
		lastParam.printDescription();
		OT_LOG_D("---");
	}
}

bool Application::generateDocumentation() {
	OT_LOG_D("Generating the documentation:");

	bool hasError = false;

	for (const Service& service : m_services) {
	std::string rst = generateServiceRstContent(service);
    std::string path = getPathToOTDocumentation();
	std::string serviceName = serviceNameToSnakeCase(service.getName());
	hasError |= writeServiceRstFile(path + "\\" + serviceName + ".rst", rst);
	}

	hasError |= updateDocumentedEndpointsIndex(m_services);

	return hasError;
}

std::string Application::generateServiceRstContent(const Service& _service) {
	OT_LOG_D("The service is:" + _service.getName());

	std::ostringstream out;

	// Title
	out << _service.getName() << "\n"
		<< std::string(_service.getName().size(), '=') << "\n\n";

	// Actions Overview
	std::string actions = "Actions";
	out << actions << "\n"
		<< std::string(actions.size(), '-') << "\n\n"
		<< ".. list-table:: Actions Overview\n"
		<< "    :widths: 25 25 50\n"
		<< "    :header-rows: 1\n\n"
		<< "    * - Endpoint\n"
		<< "      - Brief Description\n"
		<< "      - Macro\n";

	for (const Endpoint& ep : _service.getEndpoints()) {
		out << "    * - :ref:`" << ep.getName() << " <reference-" << endpointNameToKebabCase(ep.getName()) << ">`" << "\n"
			<< "      - " << ep.getBriefDescription() << "\n"
			<< "      - " << ep.getAction() << "\n";
	}

	out << "\n----\n\n";

	// Actions
	std::string briefDescription = "Brief description";
	std::string detailedDescription = "Detailed Description";
	std::string messageType = "Message Type";
	std::string parameters = "Parameters";
	std::string responseDescription = "Response Description";
	std::string response = "Response";

	for (const Endpoint& ep : _service.getEndpoints()) {
		out << ".. _reference-" << endpointNameToKebabCase(ep.getName()) << ":\n\n"	// reference
			<< ep.getName() << "\n"			// name
			<< std::string(ep.getName().size(), '^') << "\n\n"
			<< briefDescription << "\n"		// brief description
			<< std::string(briefDescription.size(), '"') << "\n\n"
			<< ep.getBriefDescription() << "\n\n";

		if (!ep.getDetailedDescription().empty()) {
			out << detailedDescription << "\n"	// detailed description
				<< std::string(detailedDescription.size(), '"') << "\n\n"
				<< ep.getDetailedDescriptionFormattedForSphinx(Endpoint::detailedDescription) << "\n\n";
		}

		out	<< messageType << "\n"			// message type
			<< std::string(messageType.size(), '"') << "\n\n"
			<< ep.getMessageTypeString() << "\n\n";

		if (!ep.getParameters().empty()) {
			out << parameters << "\n"		// parameters
				<< std::string(parameters.size(), '"') << "\n\n"
				<< ".. list-table::\n"
				<< "    :widths: 25 25 50 50\n"
				<< "    :header-rows: 1\n\n"
				<< "    * - Name\n"
				<< "      - Type\n"
				<< "      - Description\n"
				<< "      - Macro\n";
			
			for (const Parameter& param : ep.getParameters()) {
				out << "    * - " << param.getName() << "\n"
					<< "      - " << param.getDataTypeString() << "\n"
					<< "      - " << param.getDetailedDescriptionFormattedForSphinx() << "\n"
					<< "      - " << param.getMacro() << "\n";				
			}
			out << "\n";
		}

		if (!ep.getResponseDescription().empty()) {
			out << responseDescription << "\n"	// response description
				<< std::string(responseDescription.size(), '"') << "\n\n"
				<< ep.getDetailedDescriptionFormattedForSphinx(Endpoint::detailedResponseDescription) << "\n\n";
		}

		if (!ep.getResponseParameters().empty()) {
			out << response << "\n"			// response parameters
				<< std::string(response.size(), '"') << "\n\n"
				<< ".. list-table::\n"
				<< "    :widths: 25 25 50 50\n"
				<< "    :header-rows: 1\n\n"
				<< "    * - Name\n"
				<< "      - Type\n"
				<< "      - Description\n"
				<< "      - Macro\n";

			for (const Parameter& rparam : ep.getResponseParameters()) {
				out << "    * - " << rparam.getName() << "\n"
					<< "      - " << rparam.getDataTypeString() << "\n"
					<< "      - " << rparam.getDetailedDescriptionFormattedForSphinx() << "\n"
					<< "      - " << rparam.getMacro() << "\n";
			}
		}
		out << "\n----\n\n";
	}

	OT_LOG_D("The generated documentation is:\n" + out.str());

	return out.str();
}

bool Application::writeServiceRstFile(const std::string& _path, const std::string& _rst) {
	OT_LOG_D("Writing into " + _path);

	if (!_rst.empty()) {
		std::ofstream file(_path);

		if (!file.is_open()) {
			OT_LOG_E("Could not write file: " + _path);
			return true;
		}

		file << _rst;
		file.close();
	}
	else {
		OT_LOG_E("Could not write into " + _path + "because rst content is empty.");
		return true;
	}
	
	return false;
}

bool Application::updateDocumentedEndpointsIndex(const std::list<Service>& m_services) {
	OT_LOG_D("Updating documented_endpoints.rst index file:");

	std::list<std::string> serviceFileNames;
	for (const Service& service : m_services) {
		std::string serviceName = serviceNameToSnakeCase(service.getName());
		serviceFileNames.push_back(serviceName);
	}

	serviceFileNames.sort();
	std::string content = generateDocumentedEndpointsIndexContent(serviceFileNames);
	std::string path = getPathToOTDocumentation();
	std::string indexFilePath = path + "\\documented_endpoints.rst";

	return writeDocumentedEndpointsIndexFile(indexFilePath, content);
}

std::string Application::generateDocumentedEndpointsIndexContent(const std::list<std::string>& _serviceFileNames) {
	OT_LOG_D("Generating documented_endpoints.rst content.");

	std::ostringstream out;

	// Header
	out << "Documented Endpoints\n"
		<< "====================\n\n"
		<< "This section contains documented endpoints for the OpenTwin services.\n\n"
		<< ".. toctree::\n"
		<< "   :maxdepth: 3\n"
		<< "   :caption: Documented Endpoints\n\n";

	// Service file names (without .rst extension)
	for (const std::string& fileName : _serviceFileNames) {
		out << "   " << fileName << "\n";
	}

	OT_LOG_D("Generated content:\n" + out.str());

	return out.str();
}

bool Application::writeDocumentedEndpointsIndexFile(const std::string& _path, const std::string& _content) {
	OT_LOG_D("Writing documented_endpoints.rst index file to: " + _path);

	if (_content.empty()) {
		OT_LOG_E("Could not write into " + _path + " because content is empty.");
		return true;
	}

	std::ofstream file(_path);

	if (!file.is_open()) {
		OT_LOG_E("Could not write file: " + _path);
		return true;
	}

	file << _content;
	file.close();

	OT_LOG_D("Successfully wrote documented_endpoints.rst index file.");
	return false;
}

bool Application::documentParseErrors(void) {
	std::string txt = generateAllErrorsTxtContent();
	bool hasError = writeAllErrorsTxtFile(txt);
	
	return hasError;
}

void Application::reportError(const ParseError& _error) {
	m_parseErrors.push_back(_error);
	OT_LOG_E(_error.toString());
}

std::string Application::generateAllErrorsTxtContent(void) {
	std::ostringstream out;

	if (!m_parseErrors.empty()) {
		out << "======== PARSE ERRORS SUMMARY ========\n\n";
		for (ParseError e : m_parseErrors) {
			out << e.toString() << "\n";
		}
		out << "======================================";
	}

	return out.str();
}

bool Application::writeAllErrorsTxtFile(const std::string& _txt) {
	std::string path = getPathToOTEndPointDocParser();
	OT_LOG_D("Writing into " + path);

	if (!_txt.empty()) {
		std::ofstream file(path);

		if (!file.is_open()) {
			OT_LOG_E("Could not write file: " + path);
			return true;
		}

		file << _txt;
		file.close();
	}
	else {
		OT_LOG_D("Could not write into " + path + "because txt content is empty.");
	}

	return false;
}

void Application::reportEndpointsToBeDocumented() {
	OT_LOG_D("Checking for undocumented endpoints...");

	if (m_endpointsToBeDocumented.empty()) {
		OT_LOG_D("No undocumented endpoints found.");
		return;
	}

	size_t count = 0;

	for (const UndocumentedEndpoint& undocumented : m_endpointsToBeDocumented) {
		bool found = false;

		// search the service
		for (const Service& service : m_services) {
			if (service.getName() == undocumented.serviceName) {
				// search the endpoint
				for (const Endpoint& endpoint : service.getEndpoints()) {
					if (endpoint.getAction() == undocumented.action) {
						found = true;
						OT_LOG_D("Found documentation for: " + undocumented.action + " in service " + service.getName());
						break;
					}
				}
				break;
			}
		}

		if (!found) {
			OT_LOG_W(undocumented.toString());
			count++;
		}
	}
	OT_LOG_W("Found " + std::to_string(count) + " undocumented endpoint(s).");
}

// helper functions
std::string Application::getPathFromEnvironmentVariable(const std::string& _envVar, const std::string& _subPath) {
	std::string fullPath;
	char* env_p = nullptr;
	size_t len = 0;

	if (_dupenv_s(&env_p, &len, _envVar.c_str()) == 0 && env_p != nullptr) {
		OT_LOG_D(std::string("Environment variable ") + _envVar + " path is: " + env_p);
		fullPath = std::string(env_p) + "\\" + _subPath;
		OT_LOG_D(std::string("Full path is: ") + fullPath);
		free(env_p);
	}

	return fullPath;
}

std::string Application::getPathToOTLibraryOTCommunication(void) {
	return getPathFromEnvironmentVariable("OPENTWIN_DEV_ROOT", "Libraries\\OTCommunication\\include\\OTCommunication\\ActionTypes.h");
}

std::string Application::getPathToOTServices(void) {
	return getPathFromEnvironmentVariable("OPENTWIN_DEV_ROOT", "Services");
}

std::string Application::getPathToOTDocumentation(void) {
	return getPathFromEnvironmentVariable("OPENTWIN_DEV_ROOT", "Documentation\\Developer\\documented_endpoints");
}

std::string Application::getPathToOTEndPointDocParser(void) {
	std::filesystem::path currentPath = std::filesystem::current_path();
	std::filesystem::path filePath = currentPath / "parseErrors.txt";

	return filePath.string();
}

// Convert the name of the service into snake_case
std::string Application::serviceNameToSnakeCase(const std::string& _serviceName) {
	OT_LOG_D("Converting " + _serviceName + " into snake_case:");

	std::string result;
	result.reserve(_serviceName.length() * 2);

	for (size_t i = 0; i < _serviceName.length(); i++) {
		char currentChar = _serviceName[i];

		if (isupper(currentChar)) {
			if (i > 0) {
				char prevChar = _serviceName[i - 1];
				bool needUnderscore = islower(prevChar) || (isupper(prevChar) && i + 1 < _serviceName.length() && islower(_serviceName[i + 1]));

				if (needUnderscore) {
					result += '_';
				}
			}
			result += tolower(currentChar);
		}
		else {
			result += currentChar;
		}
	}

	OT_LOG_D(result);
	return result;
}

// Convert the name of the service into kebab-case
std::string Application::endpointNameToKebabCase(const std::string& _endpointName) {
	OT_LOG_D("Converting " + _endpointName + " into kebab-case:");

	std::string result;
	result.reserve(_endpointName.length() * 2);

	for (size_t i = 0; i < _endpointName.length(); i++) {
		char currentChar = _endpointName[i];

		// Replace dots with hyphens
		if (currentChar == '.') {
			result += '-';
		}
		// Handle uppercase letters
		else if (isupper(currentChar)) {
			if (i > 0) {
				char prevChar = _endpointName[i - 1];
				// Don not add a hyphen if the previous char was a dot (already converted to hyphen)
				if (prevChar != '.') {
					bool needHyphen = islower(prevChar) ||
						(isupper(prevChar) && i + 1 < _endpointName.length() && islower(_endpointName[i + 1]));

					if (needHyphen) {
						result += '-';
					}
				}
			}
			result += tolower(currentChar);
		}
		// Handle lowercase letters and other characters
		else {
			result += currentChar;
		}
	}

	OT_LOG_D(result);
	return result;
}

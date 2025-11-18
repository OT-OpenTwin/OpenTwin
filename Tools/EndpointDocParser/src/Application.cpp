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

	if (generateDocumentation(m_services)) {
		exitCode = 2;
	}

	if (documentParseErrors()) {
		exitCode = 3;
	}

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

//		OT_LOG_D("\nC++-Files:");
		for (const std::string& file : allServices) {
//			OT_LOG_D(file);

			// get the name of the service
			fs::path p = file;
			std::string serviceName = p.stem().string();
//			OT_LOG_D("Name of the service: " + serviceName);

			// create the service
			Service service;
			service.setName(serviceName);
			OT_LOG_D("Created Service " + service.getName() + ".");

			hasError |= searchIncludeAndSrcDirectoryFiles(file, service);

			service.printService();

			// add the service to the list of services if it has endpoints
			if (!service.getEndpoints().empty()) {
				addService(service);
			}
		}
		// output: Services\...
		

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
//	OT_LOG_D("The service is " + _service.getName() + ".");

	// get the path to the .vcxproj file
	fs::path p = _file;
	fs::path parentDirectory = p.parent_path();
//	OT_LOG_D("The path to " + p + " is " + parentDirectory);

	// get the path to the include-directory
	fs::path includeDir = parentDirectory / "include";
//	OT_LOG_D("Path to include directory is: " + includeDir);

	bool hasError = false;

	if (fs::exists(includeDir) && fs::is_directory(includeDir)) {
		hasError |= searchIncludeDirectoryFiles(includeDir.string(), _service);
	}

	// get the path to the src-directory
	fs::path srcDir = parentDirectory / "src";
//	OT_LOG_D("Path to src directory is: " + srcDir);

	if (fs::exists(srcDir) && fs::is_directory(srcDir)) {
		hasError |= searchSrcDirectoryFiles(srcDir.string(), _service);
	}
	return hasError;
}

bool Application::searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service) {
//	OT_LOG_D("The service is " + _service.getName() + ".");
	try {
		std::list<std::string>  includeFiles = ot::FileSystem::getFiles(_includeDirectory, {});
		OT_LOG_D("Collected Files in include-directory");

		bool hasError = false;

		for (const std::string& file : includeFiles) {
//			OT_LOG_D(file);
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
//	OT_LOG_D("The service is " + _service.getName() + ".");
	try {
		std::list<std::string> srcFiles = ot::FileSystem::getFiles(_srcDirectory, {});
		OT_LOG_D("Collected Files in src-directory");

		bool hasError = false;

		for (const std::string& file : srcFiles) {
//			OT_LOG_D(file);
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
//	OT_LOG_D("The service is " + _service.getName() + ".");
	
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
//		OT_LOG_D("Read lines:");

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
				if (startsWith(lowerCaseApiContent, "@security ")) {
					std::string security = lowerCaseApiContent.substr(10);
					security = ot::String::removePrefixSuffix(security, " \t");
//					OT_LOG_D("[SECURITY] -> " + security);

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
				else if (startsWith(lowerCaseApiContent, "@action ")) {
					std::string action = apiContent.substr(8);
					action = ot::String::removePrefixSuffix(action, " \t");
//					OT_LOG_D([ACTION] -> >>" + action + "<<");

					endpoint.setAction(action);
					OT_LOG_D("Action set in endpoint: " + endpoint.getAction());

					if (m_actionMacros.find(action) != m_actionMacros.end()) {
						std::string actionName = m_actionMacros.at(action);
//						OT_LOG_D("[ACTIONNAME] -> " + actionName);
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
				else if (startsWith(lowerCaseApiContent, "@brief ")) {
					std::string brief = apiContent.substr(7);
					brief = ot::String::removePrefixSuffix(brief, " \t");
//					OT_LOG_D("[BRIEF] -> " + brief);

					endpoint.setBriefDescription(brief);
					OT_LOG_D("Brief description set in endpoint: " + endpoint.getBriefDescription());

					inBriefDescriptionBlock = true;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(lowerCaseApiContent, "@param ")) {
					parameter = Parameter();

					std::string param = apiContent.substr(7);
					param = ot::String::removePrefixSuffix(param, " \t");
//					OT_LOG_D("[PARAM] -> " + param);
					
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
				else if (startsWith(lowerCaseApiContent, "@return ")) {
					std::string response = apiContent.substr(8);
					response = ot::String::removePrefixSuffix(response, " \t");
//					OT_LOG_D("[RETURN] -> " + response);

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
				else if (startsWith(lowerCaseApiContent, "@rparam ")) {
					parameter = Parameter();

					std::string rparam = apiContent.substr(8);
					rparam = ot::String::removePrefixSuffix(rparam, " \t");
//					OT_LOG_D("[RPARAM] -> " + rparam);

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
				else if (inBriefDescriptionBlock || inResponseDescriptionBlock) {
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
					else if (inNoteBlock) {
						apiContent = "@note " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					else if (inWarningBlock) {
						apiContent = "@warning " + apiContent;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
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
				else if (inParameterBlock || inReturnParameterBlock) {
					std::list<Parameter>& paramList = inParameterBlock ?
						endpoint.getParameters() :
						endpoint.getResponseParameters();

					if (startsWith(lowerCaseApiContent, "@note ")) {
						OT_LOG_D("Detected @note.");
						inNoteBlock = true;
						inWarningBlock = false;

						apiContent = apiContent.substr(6);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");

						addDescriptionToLastParameter(paramList, "@note " + apiContent);
					}
					else if (startsWith(lowerCaseApiContent, "@warning ")) {
						OT_LOG_D("Detected @warning.");
						inNoteBlock = false;
						inWarningBlock = true;

						apiContent = apiContent.substr(9);
						apiContent = ot::String::removePrefixSuffix(apiContent, " \t");

						addDescriptionToLastParameter(paramList, "@warning " + apiContent);
					}
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
					else if (inNoteBlock) {
						addDescriptionToLastParameter(paramList, "@note " + apiContent);
					}
					else if (inWarningBlock) {
						addDescriptionToLastParameter(paramList, "@warning " + apiContent);
					}
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
			else {
				if (inApiBlock) {
					// end of api block

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

					// Reset state for next endpoint
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
//	OT_LOG_D("Macro set in parameter: " + _parameter.getMacro());

	if (m_actionMacros.find(macro) != m_actionMacros.end()) {
		std::string name = m_actionMacros.at(macro);
		_parameter.setName(name);
//		OT_LOG_D("Macroname set in parameter: " + _parameter.getName());
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
//		OT_LOG_D("Data type Unsigned Integer 64 set in parameter: " + _parameter.getDataTypeString());

		// third string = description
		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfUnsignedInt64String);

		_parameter.addDescription(paramDescription);
//		OT_LOG_D("Description set in parameter: " + _parameter.printDescription());
	}
	else {
		std::size_t sizeOfDataTypeString = dataType.size() + 1;

		if (dataType == "uid") {
			_parameter.setDataType(Parameter::UnsignedInteger64);
//			OT_LOG_D("Data type Unsigned Integer 64 set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "boolean" || dataType == "bool") {
			_parameter.setDataType(Parameter::Boolean);
//			OT_LOG_D("Data type Boolean set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "char" || dataType == "character") {
			_parameter.setDataType(Parameter::Char);
//			OT_LOG_D("Data type Char set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "integer" || dataType == "int") {
			_parameter.setDataType(Parameter::Integer);
//			OT_LOG_D("Data type Integer set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "float") {
			_parameter.setDataType(Parameter::Float);
//			OT_LOG_D("Data type Float set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "double") {
			_parameter.setDataType(Parameter::Double);
//			OT_LOG_D("Data type Double set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "string" || dataType == "str") {
			_parameter.setDataType(Parameter::String);
//			OT_LOG_D("Data type String set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "array") {
			_parameter.setDataType(Parameter::Array);
//			OT_LOG_D("Data type Array set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "object" || dataType == "obj") {
			_parameter.setDataType(Parameter::Object);
//			OT_LOG_D("Data type Object set in parameter: " + _parameter.getDataTypeString());
		}
		else if (dataType == "enum") {
			_parameter.setDataType(Parameter::Enum);
//			OT_LOG_D("Data type Enum set in parameter: " + _parameter.getDataTypeString());
		}
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
//		OT_LOG_D("Description set in parameter: " + _parameter.printDescription());
	}

	_parameter.printParameter();

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
	const std::string pathToActionTypesHeaderFile = "C:\\OT\\OpenTwin\\Libraries\\OTCommunication\\include\\OTCommunication\\ActionTypes.h";
	std::string blackList = " \t\n";

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(pathToActionTypesHeaderFile);
//		OT_LOG_D("Read lines:");

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
			//OT_LOG_D(trimmedLine);

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
///	OT_LOG_D("Parsing content: " + _content);
	std::string blackList = " ";

	// macro definition contains macro definition in quotes
	if (_content.find("\"") != std::string::npos) {
		std::list<std::string> splittedContentList = ot::String::split(_content, "\"");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = ot::String::removeSuffix(splittedContentVector[0], blackList);
		std::string macroDefinition = splittedContentVector[1];
//		OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

		// macro definition contains two macro names at the beginning
		if (macroName.find(" ") != std::string::npos) {
			std::list<std::string> splittedMacroNameList = ot::String::split(macroName, " ");
			std::vector<std::string> splittedMacroNameVector(splittedMacroNameList.begin(), splittedMacroNameList.end());
			
			macroName = splittedMacroNameVector[0];

//			OT_LOG_D(">>" + macroName + "<<:>>" + splittedMacroNameVector[1] + "<<:>>" + macroDefinition + "<<");
			
			// second macro name is OT_ACTION_RETURN_INDICATOR_Error, replace with "Error: "
			if (splittedMacroNameVector[1] == "OT_ACTION_RETURN_INDICATOR_Error") {
				macroDefinition = "Error: " + macroDefinition;

//				OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");
				
				m_actionMacros[macroName] = macroDefinition;
///				OT_LOG_D("Added " + macroName + " and " + macroDefinition + " to Map.");
			}
		}
		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		else if (splittedContentVector.size() > 2 && splittedContentVector[2] != "") {
			splittedContentVector[2] = ot::String::removePrefix(splittedContentVector[2], blackList);
//			OT_LOG_D(splittedContentVector[2]);
			
			if (splittedContentVector[2] == "OT_ACTION_PASSWORD_SUBTEXT") {
				macroDefinition = splittedContentVector[1] + "Password";

//				OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

				m_actionMacros[macroName] = macroDefinition;
///				OT_LOG_D("Added " + macroName + " and " + macroDefinition + " to Map.");
			}	
		}
		else {
			m_actionMacros[macroName] = macroDefinition;
///			OT_LOG_D("Added " + macroName + " and " + macroDefinition + " to Map.");
		}
	}
	// macro definition contains no macro definition in quotes
	else {
		std::list<std::string> splittedContentList = ot::String::split(_content, " ");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = splittedContentVector[0];
//		OT_LOG_D(">>" + macroName + "<<:>>" + splittedContentVector[1] + "<<");

		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		if (splittedContentVector[1] == "OT_ACTION_PASSWORD_SUBTEXT") {
			std::string macroDefinition = "Password";

//			OT_LOG_D(">>" + macroName + "<<:>>" + macroDefinition + "<<");

			m_actionMacros[macroName] = macroDefinition;
///			OT_LOG_D("Added " + macroName + " and " + macroDefinition + " to Map.");
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

bool Application::generateDocumentation(const std::list<Service>& m_services) {
	OT_LOG_D("Generating the documentation:");

	bool hasError = false;

	for (const Service& service : m_services) {
	std::string rst = generateServiceRstContent(service);
    std::string path = getPathToOTDocumentation();	// C:\OT\OpenTwin\Documentation\Developer\documented_endpoints
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

		out	<< messageType << "\n"			// message Type
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
			out << responseDescription << "\n"	// response Description
				<< std::string(responseDescription.size(), '"') << "\n\n"
				<< ep.getDetailedDescriptionFormattedForSphinx(Endpoint::detailedResponseDescription) << "\n\n";
		}

		if (!ep.getResponseParameters().empty()) {
			out << response << "\n"			// response Parameters
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

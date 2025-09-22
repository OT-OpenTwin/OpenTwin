// Open Twin header
#include "OTSystem/FileSystem.h"
#include "OTSystem/Exception.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"

// project header
#include "Application.h"
#include "Service.h"

// std header
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


int Application::run(void) {
	int exitCode = 0;

	OT_LOG_D("Application is running.");
	importActionTypes();
	
	if (searchForServices()) {
		exitCode = 1;
	}

	// generate documentation exitCode = 2 if error occured

	return exitCode;
}

bool Application::searchForServices(void) {
	OT_LOG_D("Searching for Services.");

	// search in Open Twin Services
	const std::string path = "C:\\OT\\OpenTwin\\Services";
	
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
	// output: The parent path ... is "C:\\OT\\OpenTwin\\Services\\AuthorisationService"

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
//		OT_LOG_D("Collected Files in include-directory");

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
	bool inReturnParameterBlock = true;
	bool inNoteBlock = false;
	bool inWarningBlock = false;

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(_file);
//		OT_LOG_D("Read lines:");

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
//			OT_LOG_D(trimmedLine);

			std::string lowerCaseTrimmedLine = ot::String::toLower(trimmedLine);
//			OT_LOG_D("Lower case: " + trimmedLine);

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
					apiContent = ot::String::removePrefixSuffix(apiContent, " ");
				}
				OT_LOG_D(apiContent);

				// check which "@commando" is given
				if (startsWith(apiContent, "@security")) {
					std::string security = apiContent.substr(10);
//					OT_LOG_D("[SECURITY] -> " + security);

					if (ot::String::toLower(security) == "tls") {
						endpoint.setMessageType(Endpoint::TLS);
						OT_LOG_D("Message Type TLS set in endpoint: " + endpoint.getMessageTypeString());
					}
					else if (security == "mTLS" || security == "mtls") {
						endpoint.setMessageType(Endpoint::mTLS);
						OT_LOG_D("Message Type mTLS set in endpoint: " + endpoint.getMessageTypeString());
					}
					else {
						OT_LOG_E("Invalid security input.");
						return true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@action")) {
					std::string action = apiContent.substr(8);
					action = ot::String::removePrefixSuffix(action, " ");

//  				OT_LOG_D("[ACTION] -> >>" + action + "<<");
					endpoint.setAction(action);
					OT_LOG_D("Action set in endpoint: " + endpoint.getAction());

					std::string actionName = m_actionMacros.at(action);
//					OT_LOG_D("[ACTIONNAME] -> " + actionName);
					endpoint.setName(actionName);
					OT_LOG_D("Name set in endpoint: " + endpoint.getName());
				
					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@brief")) {
					std::string brief = apiContent.substr(7);
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
				else if (startsWith(apiContent, "@param")) {
					parameter = Parameter();

					std::string param = apiContent.substr(7);
//					OT_LOG_D("[PARAM] -> " + param);

					parseParameter(parameter, param, endpoint, ParameterType::FunctionParam);

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = true;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@return")) {
					std::string response = apiContent.substr(8);
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
				else if (startsWith(apiContent, "@rparam")) {
					parameter = Parameter();

					std::string rparam = apiContent.substr(8);
//					OT_LOG_D("[RETURNPARAM] -> " + rparam);

					if (parseParameter(parameter, rparam, endpoint, ParameterType::ReturnParam)) {
						return true;
					}

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = true;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (inBriefDescriptionBlock || inResponseDescriptionBlock) {
					if (startsWith(apiContent, "@note")) {
						OT_LOG_D("Detected @note.");
						inNoteBlock = true;
						inWarningBlock = false;
						
						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					else if (startsWith(apiContent, "@warning")) {
						OT_LOG_D("Detected @warning.");						
						inNoteBlock = false;
						inWarningBlock = true;

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					else if (startsWith(apiContent, "@detail")) {
						apiContent = apiContent.substr(8);

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
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
//						OT_LOG_D("Detailed Description: >>" + apiContent + "<<");

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
					if (startsWith(apiContent, "@note")) {
						OT_LOG_D("Detected @note.");
						inNoteBlock = true;
						inWarningBlock = false;
						
						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}
					}
					else if (startsWith(apiContent, "@warning")) {
						OT_LOG_D("Detected @warning.");
						inWarningBlock = true;
						inNoteBlock = false;

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}
					}
					else if (startsWith(apiContent, "@detail")) {
						apiContent = apiContent.substr(8);

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}

						inNoteBlock = false;
						inWarningBlock = false;
					}
					else if (inNoteBlock) {
						apiContent = "@note " + apiContent;

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}
					}
					else if (inWarningBlock) {
						apiContent = "@warning " + apiContent;

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}
					}
					else {
//						OT_LOG_D("Detailed parameter description: >>" + apiContent + "<<");

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							OT_LOG_D("Last parameter: " + lastParam.getMacro());

							lastParam.addDescription(apiContent);
							OT_LOG_D("Added description to " + lastParam.getMacro() + ":");
							lastParam.printDescription();
							OT_LOG_D("---");
						}
					}
				}
				else {
					OT_LOG_D("[UNKNOWN] -> " + apiContent);
				}
			}
			else {
				if (inApiBlock) {
					// end of api block, add endpoint to service
					OT_LOG_D("The parsed endpoint is:");
					endpoint.printEndpoint();				
					
					OT_LOG_D("The service is " + _service.getName() + ".");
					_service.addEndpoint(endpoint);
					OT_LOG_D("Added endpoint to service.");
					_service.printService();

					inApiBlock = false;
					OT_LOG_D("Detected end of api documentation block.");
					OT_LOG_D("-----------------------------------------------------------------------------------");
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

bool Application::parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, ParameterType _parameterType) {
	OT_LOG_D("Parsing parameter: " + _param);
	std::list<std::string> splittedParamList = ot::String::split(_param, " ");
	std::vector<std::string> splittedParamVector(splittedParamList.begin(), splittedParamList.end());

	std::string macro = splittedParamVector[0];
//	OT_LOG_D("The first string is: " + macro);
	std::size_t sizeOfMacroString = macro.size() + 1;
//	OT_LOG_D(sizeOfMacroString);

	_parameter.setMacro(macro);
//	OT_LOG_D("Macro set in parameter: " + _parameter.getMacro());

	std::string name = m_actionMacros.at(macro);
	_parameter.setName(name);
//	OT_LOG_D("Macroname set in parameter: " + _parameter.getName());

	std::string dataType = splittedParamVector[1];

	dataType = ot::String::toLower(dataType);
//	OT_LOG_D("Lowercase string: " + dataType);

	// data type is Unsigned Integer 64
	if (dataType == "unsigned") {
		std::string dataType2 = splittedParamVector[2];
		std::string dataType3 = splittedParamVector[3];
		std::string unsignedInt64 = dataType + " " + dataType2 + " " + dataType3;
//		OT_LOG_D("The second string is: " + unsignedInt64);

		std::size_t sizeOfUnsignedInt64String = unsignedInt64.size() + 1;
//		OT_LOG_D(sizeOfUnsignedInt64String);

		_parameter.setDataType(Parameter::UnsignedInteger64);
//		OT_LOG_D("Data type Unsigned Integer 64 set in parameter: " + _parameter.getDataTypeString());

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfUnsignedInt64String);
//		OT_LOG_D("The third string is: " + paramDescription);

		_parameter.addDescription(paramDescription);
//		OT_LOG_D("Description set in parameter: " + _parameter.printDescription());
	}
	else {
//		OT_LOG_D("The second string is: " + dataType);

		std::size_t sizeOfDataTypeString = dataType.size() + 1;
//		OT_LOG_D(sizeOfDataTypeString);

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

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfDataTypeString);
//		OT_LOG_D("The third string is: " + paramDescription);
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
		OT_LOG_E("Unknown parameter type.");
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
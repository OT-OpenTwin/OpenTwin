// Open Twin header
#include "OTSystem/FileSystem.h"
#include "OTSystem/Exception.h"
#include "OTCore/String.h"

// project header
#include "Application.h"
#include "Service.h"

// std header
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


void Application::run(void) {
	std::cout << "Application is running.\n";
	searchInLibrary();
	searchForServices();
}

void Application::searchForServices(void) {
	std::cout << "Searching for Services.\n";

	// search in Open Twin Services
	const std::string path = "C:\\OT\\OpenTwin\\Services";
	
	std::list<Service> services;

	try {
		std::list<std::string> allServices = ot::FileSystem::getFiles(path, { ".vcxproj" }, ot::FileSystem::FileSystemOption::Recursive);

//		std::cout << "\nC++-Files:\n";
		for (const std::string& file : allServices) {
//			std::cout << "  " << file << "\n";

			// get the name of the service
			fs::path p = file;
			std::string serviceName = p.stem().string();
//			std::cout << "Name of the service: " << serviceName << "\n";

			// create the service
			Service service;
			service.setName(serviceName);

			std::cout << "Created Service " << service.getName() << ".\n";

			searchIncludeAndSrcDirectoryFiles(file, service);

			service.printService();

			// add the service to the list of services if it has endpoints
			if (!service.getEndpoints().empty()) {
				addService(service);
			}
		}
		// output: Services\...
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
	// print the list of services
	std::cout << "\n" << m_services.size() << " Services added to List of services: \n";
	for (const Service& service : m_services) {
		service.printService();
		// print the endpoints contained in the service
		for (const Endpoint& endpoint : service.getEndpoints()) {
			endpoint.printEndpoint();
		}
	}
}

void Application::searchIncludeAndSrcDirectoryFiles(const std::string& _file, Service& _service) {
	std::cout << "Searching for include und src directories of the given file: " << _file << "\n";
//	std::cout << "The service is " << _service.getName() << ". \n";

	// get the path to the .vcxproj file
	fs::path p = _file;
	fs::path parentDirectory = p.parent_path();

//	std::cout << "The path to " << p << " is " << parentDirectory << '\n';
	// output: The parent path ... is "C:\\OT\\OpenTwin\\Services\\AuthorisationService"

	// get the path to the include-directory
	fs::path includeDir = parentDirectory / "include";

//	std::cout << "Path to include directory is: " << includeDir << "\n";

	if (fs::exists(includeDir) && fs::is_directory(includeDir)) {
		searchIncludeDirectoryFiles(includeDir.string(), _service);
	}

	// get the path to the src-directory
	fs::path srcDir = parentDirectory / "src";

//	std::cout << "Path to src directory is: " << srcDir << "\n";

	if (fs::exists(srcDir) && fs::is_directory(srcDir)) {
		searchSrcDirectoryFiles(srcDir.string(), _service);
	}
}

void Application::searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service) {
//	std::cout << "The service is " << _service.getName() << ". \n";
	try {
		std::list<std::string>  includeFiles = ot::FileSystem::getFiles(_includeDirectory, {});
//		std::cout << "Collected Files in include-directory\n";

		for (const std::string& file : includeFiles) {
//			std::cout << "  " << file << "\n";
			parseFile(file, _service);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}

void Application::searchSrcDirectoryFiles(const std::string& _srcDirectory, Service& _service) {
//	std::cout << "The service is " << _service.getName() << ". \n";
	try {
		std::list<std::string> srcFiles = ot::FileSystem::getFiles(_srcDirectory, {});
		std::cout << "Collected Files in src-directory\n";

		for (const std::string& file : srcFiles) {
//			std::cout << "  " << file << "\n";
			parseFile(file, _service);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}

void Application::parseFile(const std::string& _file, Service& _service) {
//	std::cout << "The service is " << _service.getName() << ". \n";
	
	std::cout << "Parsing file: " << _file << "\n";
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
//		std::cout << "Read lines: " << "\n";

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
//			std::cout << trimmedLine << "\n";

			std::string lowerCaseTrimmedLine = ot::String::toLower(trimmedLine);
//			std::cout << "Lower case: " << trimmedLine << "\n";

			// check if the parser is in an api documentation block
			if (startsWith(lowerCaseTrimmedLine, "//api")) {
				if (!inApiBlock) {
					inApiBlock = true;
					endpoint = Endpoint();  // new endpoint
					std::cout << "-----------------------------------------------------------------------------------\n";
					std::cout << "Detected start of api documentation block.\n";
				}

				// remove "//api " prefix
				std::string apiContent = trimmedLine.substr(5);
				if (!apiContent.empty()) {
					apiContent = ot::String::removePrefixSuffix(apiContent, " ");
				}
				std::cout << apiContent << "\n";

				// check which "@commando" is given
				if (startsWith(apiContent, "@security")) {
					std::string security = apiContent.substr(10);
//					std::cout << "[SECURITY] -> " << security << "\n";

					if (security == "TLS" || security == "tls") {
						endpoint.setMessageType(Endpoint::TLS);
						std::cout << "Message Type TLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
					}
					else if (security == "mTLS" || security == "mtls") {
						endpoint.setMessageType(Endpoint::mTLS);
						std::cout << "Message Type mTLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
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

//  				std::cout << "[ACTION] -> >>" << action << "<<" << "\n";
					endpoint.setAction(action);
					std::cout << "Action set in endpoint: " << endpoint.getAction() << "\n";

					std::string actionName = m_actionMacros.at(action);
//					std::cout << "[ACTIONNAME] -> " << actionName << "\n";
					endpoint.setName(actionName);
					std::cout << "Name set in endpoint: " << endpoint.getName() << "\n";
				
					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@brief")) {
					std::string brief = apiContent.substr(7);
//					std::cout << "[BRIEF] -> " << brief << "\n";
					endpoint.setBriefDescription(brief);
					std::cout << "Brief description set in endpoint: " << endpoint.getBriefDescription() << "\n";
					
					inBriefDescriptionBlock = true;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@param")) {
					parameter = Parameter();
					std::string parameterType = "Function parameter";

					std::string param = apiContent.substr(7);
//					std::cout << "[PARAM] -> " << param << "\n";

					parseParameter(parameter, param, endpoint, parameterType);

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = true;
					inReturnParameterBlock = false;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (startsWith(apiContent, "@return")) {
					std::string response = apiContent.substr(8);
//					std::cout << "[RETURN] -> " << response << "\n";
					endpoint.addResponseDescription(response);
					std::cout << "Response set in endpoint:\n";
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
					std::string parameterType = "Return parameter";

					std::string rparam = apiContent.substr(8);
//					std::cout << "[RETURNPARAM] -> " << rparam << "\n";

					parseParameter(parameter, rparam, endpoint, parameterType);

					inBriefDescriptionBlock = false;
					inResponseDescriptionBlock = false;
					inParameterBlock = false;
					inReturnParameterBlock = true;
					inNoteBlock = false;
					inWarningBlock = false;
				}
				else if (inBriefDescriptionBlock || inResponseDescriptionBlock) {
					if (startsWith(apiContent, "@note")) {
						std::cout << "Detected @note." << "\n";
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
						std::cout << "Detected @warning." << "\n";						
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
//						std::cout << "Detailed Description: >>" << apiContent << "<<" << "\n";

						if (inBriefDescriptionBlock) {
							endpoint.addDetailedDescription(apiContent);
						}
						else {
							endpoint.addResponseDescription(apiContent);
						}
					}
					if (inBriefDescriptionBlock) {
						std::cout << "Detailed description set in endpoint: " << "\n";
						endpoint.printDetailedDescription();
					}
					else {
						std::cout << "Response description set in endpoint: " << "\n";
						endpoint.printResponseDescription();
					}
					std::cout << "---\n";
				}
				else if (inParameterBlock || inReturnParameterBlock) {
					if (startsWith(apiContent, "@note")) {
						std::cout << "Detected @note." << "\n";
						inNoteBlock = true;
						inWarningBlock = false;
						
						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
						}
					}
					else if (startsWith(apiContent, "@warning")) {
						std::cout << "Detected @warning." << "\n";
						inWarningBlock = true;
						inNoteBlock = false;

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
						}
					}
					else if (startsWith(apiContent, "@detail")) {
						apiContent = apiContent.substr(8);

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
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
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
						}
					}
					else if (inWarningBlock) {
						apiContent = "@warning " + apiContent;

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
						}
					}
					else {
//						std::cout << "Detailed parameter description: >>" << apiContent << "<<" << "\n";

						std::list<Parameter>& paramList = inParameterBlock ?
							endpoint.getParameters() :
							endpoint.getResponseParameters();

						if (!paramList.empty()) {
							Parameter& lastParam = paramList.back();
//							std::cout << "Last parameter: " << lastParam.getMacro() << "\n";

							lastParam.addDescription(apiContent);
							std::cout << "Added description to " << lastParam.getMacro() << ":" << "\n";
							lastParam.printDescription();
							std::cout << "---\n";
						}
					}
				}
				else {
					std::cout << "[UNKNOWN] -> " << apiContent << "\n";
				}
			}
			else {
				if (inApiBlock) {
					// end of api block, add endpoint to service
					std::cout << "The parsed endpoint is: \n";
					endpoint.printEndpoint();				
					
					std::cout << "The service is " << _service.getName() << ". \n";
					_service.addEndpoint(endpoint);
					std::cout << "Added endpoint to service. \n";
					_service.printService();

					inApiBlock = false;
					std::cout << "Detected end of api documentation block.\n";
					std::cout << "-----------------------------------------------------------------------------------\n";
				}
			}
		}
	}
	catch (const ot::FileOpenException& e) {
		std::cerr << "Error by readLines: " << e.what() << "\n";
	}
	catch (const std::ios_base::failure& e) {
		std::cerr << "IO-failure: " << e.what() << '\n';
	}
}

// returns true if the line starts with the given prefix
bool Application::startsWith(const std::string& _line, const std::string& _prefix) {
	return _line.compare(0, _prefix.size(), _prefix) == 0;
}

void Application::parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, const std::string& _parameterType) {
	std::cout << "Parsing parameter: " << _param << "\n";
	std::list<std::string> splittedParamList = ot::String::split(_param, " ");
	std::vector<std::string> splittedParamVector(splittedParamList.begin(), splittedParamList.end());

	std::string macro = splittedParamVector[0];
//	std::cout << "The first string is: " << macro << "\n";
	std::size_t sizeOfMacroString = macro.size() + 1;
//	std::cout << sizeOfMacroString << "\n";

	_parameter.setMacro(macro);
//	std::cout << "Macro set in parameter: " << _parameter.getMacro() << "\n";

	std::string name = m_actionMacros.at(macro);
	_parameter.setName(name);
//	std::cout << "Macroname set in parameter: " << _parameter.getName() << "\n";

	std::string dataType = splittedParamVector[1];

	dataType = ot::String::toLower(dataType);
//	std::cout << "Lowercase string: " << dataType << "\n";

	// data type is Unsigned Integer 64
	if (dataType == "unsigned") {
		std::string dataType2 = splittedParamVector[2];
		std::string dataType3 = splittedParamVector[3];
		std::string unsignedInt64 = dataType + " " + dataType2 + " " + dataType3;
//		std::cout << "The second string is: " << unsignedInt64 << "\n";

		std::size_t sizeOfUnsignedInt64String = unsignedInt64.size() + 1;
//		std::cout << sizeOfUnsignedInt64String << "\n";

		_parameter.setDataType(Parameter::UnsignedInteger64);
//		std::cout << "Data type Unsigned Integer 64 set in parameter: " << _parameter.getDataTypeString() << "\n";

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfUnsignedInt64String);
//		std::cout << "The third string is: " << paramDescription << "\n";

		_parameter.addDescription(paramDescription);
//		std::cout << "Description set in parameter: " << _parameter.printDescription() << "\n";
	}
	else {
//		std::cout << "The second string is: " << dataType << "\n";

		std::size_t sizeOfDataTypeString = dataType.size() + 1;
//		std::cout << sizeOfDataTypeString << "\n";

		if (dataType == "uid") {
			_parameter.setDataType(Parameter::UnsignedInteger64);
//			std::cout << "Data type Unsigned Integer 64 set in parameter: " << _parameter.getDataTypeString() << "\n";
		}

		else if (dataType == "boolean" || dataType == "bool") {
			_parameter.setDataType(Parameter::Boolean);
//			std::cout << "Data type Boolean set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "char" || dataType == "character") {
			_parameter.setDataType(Parameter::Char);
//			std::cout << "Data type Char set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "integer" || dataType == "int") {
			_parameter.setDataType(Parameter::Integer);
//			std::cout << "Data type Integer set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "float") {
			_parameter.setDataType(Parameter::Float);
//			std::cout << "Data type Float set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "double") {
			_parameter.setDataType(Parameter::Double);
//			std::cout << "Data type Double set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "string" || dataType == "str") {
			_parameter.setDataType(Parameter::String);
//			std::cout << "Data type String set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "array") {
			_parameter.setDataType(Parameter::Array);
//			std::cout << "Data type Array set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "object" || dataType == "obj") {
			_parameter.setDataType(Parameter::Object);
//			std::cout << "Data type Object set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "enum") {
			_parameter.setDataType(Parameter::Enum);
//			std::cout << "Data type Enum set in parameter: " << _parameter.getDataTypeString() << "\n";
		}

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfDataTypeString);
//		std::cout << "The third string is: " << paramDescription << "\n";
		_parameter.addDescription(paramDescription);
//		std::cout << "Description set in parameter: " << _parameter.printDescription() << "\n";
	}
	_parameter.printParameter();
	if (_parameterType == "Function parameter") {
		_endpoint.addParameter(_parameter);
		std::cout << "Added Parameter to parameters.\n";
	}
	else if (_parameterType == "Return parameter") {
		_endpoint.addResponseParameter(_parameter);
		std::cout << "Added Parameter to response parameters.\n";
	}
}

void Application::addService(const Service& _service) {
	std::cout << "Adding Service " << _service.getName() << " to List of Services.\n";
	m_services.push_back(_service);
}

void Application::searchInLibrary(void) {
	// search in ActionTypes.h in Open Twin OTCommunication Library
	const std::string pathToActionTypesHeaderFile = "C:\\OT\\OpenTwin\\Libraries\\OTCommunication\\include\\OTCommunication\\ActionTypes.h";
	std::string blackList = " \t\n";

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(pathToActionTypesHeaderFile);
//		std::cout << "Read lines: " << "\n";

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
			//std::cout << trimmedLine << "\n";

			// detect Macro-definition
			if (startsWith(trimmedLine, "#define")) {
				// remove "#define " prefix
				std::string content = trimmedLine.substr(8);

				parseMacroDefinition(content);
			}
		}

		std::cout << "The parsed map of macro-name and -definition is :\n";
		for (const auto macro : m_actionMacros) {
			std::cout << macro.first << " : >>" << macro.second << "<<" << "\n";
		}
	}
	catch (const ot::FileOpenException& e) {
		std::cerr << "Error by readLines: " << e.what() << "\n";
	}
	catch (const std::ios_base::failure& e) {
		std::cerr << "IO-failure: " << e.what() << '\n';
	}
}

/*
// testing special cases
void Application::searchInLibrary(void) {
	// search in ActionTypes.h in Open Twin OTCommunication Library
	const std::string pathToActionTypesHeaderFile = "C:\\OT\\OpenTwin\\Libraries\\OTCommunication\\include\\OTCommunication\\ActionTypes.h";
	std::string blackList = " \t\n";
	
	std::list<std::string> lines;
	lines.push_back("	#define OT_ACTION_PASSWORD_SUBTEXT \"Password\"");
	lines.push_back("	#define OT_ACTION_RETURN_INDICATOR_Error \"ERROR: \"");
	lines.push_back("	#define OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE \"CST Studio Suite\"");
	lines.push_back("	#define OT_ACTION_RETURN_UnknownError OT_ACTION_RETURN_INDICATOR_Error \"Unknown error\"");
	lines.push_back("	#define OT_PARAM_AUTH_PASSWORD OT_ACTION_PASSWORD_SUBTEXT");
	lines.push_back("	#define OT_ACTION_CMD_PROJ_Save \"Project.Save\"");
	lines.push_back("	#define OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD \"LoggedInUser\" OT_ACTION_PASSWORD_SUBTEXT");

	std::cout << "Read lines: " << "\n";

	for (const std::string& line : lines) {
		std::string trimmedLine = ot::String::removePrefix(line, blackList);
		std::cout << trimmedLine << "\n";

		// detect Macro-definition
		if (startsWith(trimmedLine, "#define")) {
			// remove "#define " prefix
			std::string content = trimmedLine.substr(8);

			parseMacroDefinition(content);
		}
	}

	std::cout << "The parsed map of macro-name and -definition is :\n";
	for (const auto macro : m_actionMacros) {
		std::cout << macro.first << " : >>" << macro.second << "<<" << "\n";
	}

}
*/

void Application::parseMacroDefinition(const std::string& _content) {
///	std::cout << "Parsing content: " << _content << "\n";
	std::string blackList = " ";

	// macro definition contains macro definition in quotes
	if (_content.find("\"") != std::string::npos) {
		std::list<std::string> splittedContentList = ot::String::split(_content, "\"");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = ot::String::removeSuffix(splittedContentVector[0], blackList);
		std::string macroDefinition = splittedContentVector[1];
//		std::cout << ">>" << macroName << "<<:>>" << macroDefinition << "<<" << "\n";

		// macro definition contains two macro names at the beginning
		if (macroName.find(" ") != std::string::npos) {
			std::list<std::string> splittedMacroNameList = ot::String::split(macroName, " ");
			std::vector<std::string> splittedMacroNameVector(splittedMacroNameList.begin(), splittedMacroNameList.end());
			
			macroName = splittedMacroNameVector[0];

//			std::cout << ">>" << macroName << "<<:>>" << splittedMacroNameVector[1] << "<<:>>" << macroDefinition << "<<" << "\n";
			
			// second macro name is OT_ACTION_RETURN_INDICATOR_Error, replace with "Error: "
			if (splittedMacroNameVector[1] == "OT_ACTION_RETURN_INDICATOR_Error") {
				macroDefinition = "Error: " + macroDefinition;

//				std::cout << ">>" << macroName << "<<:>>" << macroDefinition << "<<" << "\n";
				
				m_actionMacros[macroName] = macroDefinition;
///				std::cout << "Added " << macroName << " and " << macroDefinition << " to Map.\n";
			}
		}
		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		else if (splittedContentVector.size() > 2 && splittedContentVector[2] != "") {
			splittedContentVector[2] = ot::String::removePrefix(splittedContentVector[2], blackList);
//			std::cout << splittedContentVector[2] << "\n";
			
			if (splittedContentVector[2] == "OT_ACTION_PASSWORD_SUBTEXT") {
				macroDefinition = splittedContentVector[1] + "Password";

//				std::cout << ">>" << macroName << "<<:>>" << macroDefinition << "<<" << "\n";

				m_actionMacros[macroName] = macroDefinition;
///				std::cout << "Added " << macroName << " and " << macroDefinition << " to Map.\n";
			}	
		}
		else {
			m_actionMacros[macroName] = macroDefinition;
///			std::cout << "Added " << macroName << " and " << macroDefinition << " to Map.\n";
		}
	}
	// macro definition contains no macro definition in quotes
	else {
		std::list<std::string> splittedContentList = ot::String::split(_content, " ");
		std::vector<std::string> splittedContentVector(splittedContentList.begin(), splittedContentList.end());
		
		std::string macroName = splittedContentVector[0];
//		std::cout << ">>" << macroName << "<<:>>" << splittedContentVector[1] << "<<" "\n";

		// macro definition contains OT_ACTION_PASSWORD_SUBTEXT at the end, replace with "Password"
		if (splittedContentVector[1] == "OT_ACTION_PASSWORD_SUBTEXT") {
			std::string macroDefinition = "Password";

//			std::cout << ">>" << macroName << "<<:>>" << macroDefinition << "<<" << "\n";

			m_actionMacros[macroName] = macroDefinition;
///			std::cout << "Added " << macroName << " and " << macroDefinition << " to Map.\n";
		}
	}
}
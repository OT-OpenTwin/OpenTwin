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
	searchForServices();
}

void Application::searchForServices(void) {
	std::cout << "Searching for Services.\n";

/*
	// search in Open Twin Services
	const std::string path = "C:\\OT\\OpenTwin\\Services";

	try {
		std::list<std::string> allServices = ot::FileSystem::getFiles(path, { ".vcxproj" }, ot::FileSystem::FileSystemOption::Recursive);

		std::cout << "\nC++-Files:\n";
		for (const std::string& file : allServices) {
			std::cout << "  " << file << "\n";

			// get the name of the service
			fs::path p = file;
			std::string serviceName = p.stem().string();
			std::cout << "Name of the service: " << serviceName << "\n";

			// create the service
			Service service;
			service.setName(serviceName);

			std::cout << "Created Service " << service.getName() << ".\n";

			searchIncludeAndSrcDirectoryFiles(file, service);
		}
		// output: Services\...
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
*/

		// search in one Service
		const std::string pathToAuthorisationService = "C:\\OT\\OpenTwin\\Services\\AuthorisationService";

		try {
			std::list<std::string> allServiceFiles = ot::FileSystem::getFiles(pathToAuthorisationService, { ".vcxproj" });

			std::cout << "\nC++-Files:\n";
			for (const std::string& file : allServiceFiles) {
				std::cout << "  " << file << "\n";

				// get the name of the service
				fs::path p = file;
				std::string serviceName = p.stem().string();
				std::cout << "Name of the service: " << serviceName << "\n";

				// create the service
				Service service;
				service.setName(serviceName);

				std::cout << "Created Service " << service.getName() << ".\n";

				searchIncludeAndSrcDirectoryFiles(file, service);
			}
			// output: C:\OT\OpenTwin\Services\AuthorisationService\AuthorisationService.vcxproj
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error by getFiles: " << e.what() << "\n";
		}
}

void Application::searchIncludeAndSrcDirectoryFiles(const std::string& _file, const Service& _service) {
	std::cout << "Searching for include und src directories of the given file: " << _file << "\n";
	std::cout << "The service is " << _service.getName() << ". \n";

	// get the path to the .vcxproj file
	fs::path p = _file;
	fs::path parentDirectory = p.parent_path();

	std::cout << "The path to " << p << " is " << parentDirectory << '\n';
	// output: The parent path ... is "C:\\OT\\OpenTwin\\Services\\AuthorisationService"

	// get the path to the include-directory
	fs::path includeDir = parentDirectory / "include";

	std::cout << "Path to include directory is: " << includeDir << "\n";

	if (fs::exists(includeDir) && fs::is_directory(includeDir)) {
		searchIncludeDirectoryFiles(includeDir.string());
	}

	// get the path to the src-directory
	fs::path srcDir = parentDirectory / "src";

	std::cout << "Path to src directory is: " << srcDir << "\n";

	if (fs::exists(srcDir) && fs::is_directory(srcDir)) {
		searchSrcDirectoryFiles(srcDir.string());
	}
}

void Application::searchIncludeDirectoryFiles(const std::string& _includeDirectory) {
	try {
		std::list<std::string>  includeFiles = ot::FileSystem::getFiles(_includeDirectory, {});
		std::cout << "Collected Files in include-directory\n";

		for (const std::string& file : includeFiles) {
			std::cout << "  " << file << "\n";
			parseFile(file);
		}
		// output: Collected Files in include-directory 
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}

void Application::searchSrcDirectoryFiles(const std::string& _srcDirectory) {
	try {
		std::list<std::string> srcFiles = ot::FileSystem::getFiles(_srcDirectory, {});
		std::cout << "Collected Files in src-directory\n";

		for (const std::string& file : srcFiles) {
			std::cout << "  " << file << "\n";
			parseFile(file);
		}
		// output: Collected Files in src-directory C:\OT\OpenTwin\Services\AuthorisationService\src\dllmain.cpp
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}

void Application::parseFile(const std::string& _file) {
	std::cout << "Parsing file: " << _file << "\n";
	std::string blackList = " \t\n";
	
	Service service;
	Endpoint endpoint;
	Parameter parameter;
	bool inApiBlock = false;

	// read lines from given file and parse them
	try {
		std::list<std::string> lines = ot::FileSystem::readLines(_file);
		std::cout << "Read lines: " << "\n";

		for (const std::string& line : lines) {
			std::string trimmedLine = ot::String::removePrefix(line, blackList);
			std::cout << trimmedLine << "\n";

			if (startsWith(trimmedLine, "//api")) {
				if (!inApiBlock) {
					inApiBlock = true;
					endpoint = Endpoint();  // new endpoint
					std::cout << "Detected start of api documentation block.\n";
				}

				// remove "//api " prefix
				std::string apiContent = trimmedLine.substr(6);
				std::cout << apiContent << "\n";

				// check which "@commando" is given
				if (startsWith(apiContent, "@security")) {
					std::string security = apiContent.substr(10);
					std::cout << "[SECURITY] -> " << security << "\n";

					if (security == "TLS" || security == "tls") {
						endpoint.setMessageType(Endpoint::TLS);
						std::cout << "Message Type TLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
					}
					else if (security == "mTLS" || security == "mtls") {
						endpoint.setMessageType(Endpoint::mTLS);
						std::cout << "Message Type mTLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
					}
				}
				else if (startsWith(apiContent, "@action")) {
					std::string action = apiContent.substr(8);
					std::cout << "[ACTION] -> " << action << "\n";
					endpoint.setAction(action);
					std::cout << "Action set in endpoint: " << endpoint.getAction() << "\n";
				}
				else if (startsWith(apiContent, "@brief")) {
					std::string brief = apiContent.substr(7);
					std::cout << "[BRIEF] -> " << brief << "\n";
					endpoint.setBriefDescription(brief);
					std::cout << "Brief description set in endpoint: " << endpoint.getBriefDescription() << "\n";
				}
				else if (startsWith(apiContent, "@param")) {
					parameter = Parameter();
					std::string parameterType = "Function parameter";

					std::string param = apiContent.substr(7);
					std::cout << "[PARAM] -> " << param << "\n";

					parseParameter(parameter, param, endpoint, parameterType);
				}
				else if (startsWith(apiContent, "@return")) {
					std::string response = apiContent.substr(8);
					std::cout << "[RETURN] -> " << response << "\n";
					endpoint.setResponseDescription(response);
					std::cout << "Response set in endpoint: " << endpoint.getResponseDescription() << "\n";
				}
				else if (startsWith(apiContent, "@rparam")) {
					parameter = Parameter();
					std::string parameterType = "Return parameter";

					std::string rparam = apiContent.substr(8);
					std::cout << "[RETURNPARAM] -> " << rparam << "\n";

					parseParameter(parameter, rparam, endpoint, parameterType);
				}
				else {
					std::cout << "[UNKNOWN] -> " << apiContent << "\n";
				}
			}
			else {
				if (inApiBlock) {
					// end of api block, add endpoint to service
					endpoint.printEndpoint();
					service.addEndpoint(endpoint);
					inApiBlock = false;
					std::cout << "Detected end of api documentation block.\n";
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

/*	// test the parser with simulated Api code documentation blocks
	std::list<std::string> lines;
	lines.push_back("Some text...");
	lines.push_back("\n");
	lines.push_back("	//api @security tls");
	lines.push_back("	//api @action OT_ACTION_CMD_MyAction");
	lines.push_back("	//api @brief Another short brief description.");
	lines.push_back("	//api @param OT_ACTION_PARAM_MyParam unsigned integer 64 My parameter description.");
	lines.push_back("	//api @param OT_ACTION_PARAM_MyParam2 Float My parameter description 2.");
	lines.push_back("	//api @rparam OT_ACTION_PARAM_MyParam int My return parameter description.");
	lines.push_back("	//api @rparam OT_ACTION_PARAM_MyParam2 array My return parameter description 2.");
	lines.push_back("	//api @return In case of success it will contain the project data.");
	lines.push_back("\n");
	lines.push_back("Some text...");
	lines.push_back("Some text...");
	lines.push_back("\n");
	lines.push_back("	//api @security mtls");
	lines.push_back("	//api @action OT_ACTION_CMD_MyAction");
	lines.push_back("	//api @brief Another short brief description.");
	lines.push_back("	//api @param OT_ACTION_PARAM_MyParam char My parameter description.");
	lines.push_back("	//api @param OT_ACTION_PARAM_MyParam2 Char My parameter description 2.");
	lines.push_back("	//api @rparam OT_ACTION_PARAM_MyParam bool My return parameter description.");
	lines.push_back("	//api @rparam OT_ACTION_PARAM_MyParam2 obj My return parameter description 2.");
	lines.push_back("	//api @return In case of success it will contain the project data.");
	lines.push_back("\n");
	lines.push_back("Some text...");

	Service service;
	Endpoint endpoint;
	Parameter parameter;
	bool inApiBlock = false;

	for (const std::string& line : lines) {
		std::string trimmedLine = ot::String::removePrefix(line, blackList);
		std::cout << trimmedLine << "\n";

		if (startsWith(trimmedLine, "//api")) {
			if (!inApiBlock) {
				inApiBlock = true;
				endpoint = Endpoint();  // new endpoint
				std::cout << "Detected start of api documentation block.\n";
			}

			// remove "//api " prefix
			std::string apiContent = trimmedLine.substr(6);
			std::cout << apiContent << "\n";

			// check which "@commando" is given
			if (startsWith(apiContent, "@security")) {
				std::string security = apiContent.substr(10);
				std::cout << "[SECURITY] -> " << security << "\n";
					
				if (security == "TLS" || security == "tls") {
					endpoint.setMessageType(Endpoint::TLS);
					std::cout << "Message Type TLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
				}
				else if (security == "mTLS" || security == "mtls") {
					endpoint.setMessageType(Endpoint::mTLS);
					std::cout << "Message Type mTLS set in endpoint: " << endpoint.getMessageTypeString() << "\n";
				}
			}
			else if (startsWith(apiContent, "@action")) {
				std::string action = apiContent.substr(8);
				std::cout << "[ACTION] -> " << action << "\n";
				endpoint.setAction(action);
				std::cout << "Action set in endpoint: " << endpoint.getAction() << "\n";
			}
			else if (startsWith(apiContent, "@brief")) {
				std::string brief = apiContent.substr(7);
				std::cout << "[BRIEF] -> " << brief << "\n";
				endpoint.setBriefDescription(brief);
				std::cout << "Brief description set in endpoint: " << endpoint.getBriefDescription() << "\n";
			}
			else if (startsWith(apiContent, "@param")) {
				parameter = Parameter();
				std::string parameterType = "Function parameter";

				std::string param = apiContent.substr(7);
				std::cout << "[PARAM] -> " << param << "\n";

				parseParameter(parameter, param, endpoint, parameterType);
			}
			else if (startsWith(apiContent, "@return")) {
				std::string response = apiContent.substr(8);
				std::cout << "[RETURN] -> " << response << "\n";
				endpoint.setResponseDescription(response);
				std::cout << "Response set in endpoint: " << endpoint.getResponseDescription() << "\n";
			}
			else if (startsWith(apiContent, "@rparam")) {
				parameter = Parameter();
				std::string parameterType = "Return parameter";

				std::string rparam = apiContent.substr(8);
				std::cout << "[RETURNPARAM] -> " << rparam << "\n";

				parseParameter(parameter, rparam, endpoint, parameterType);
			}	
			else {
				std::cout << "[UNKNOWN] -> " << apiContent << "\n";
			}
		}
		else {
			if (inApiBlock) {
				// end of api block, add endpoint to service
				endpoint.printEndpoint();
				service.addEndpoint(endpoint);
				inApiBlock = false;
				std::cout << "Detected end of api documentation block.\n";
			}
		}
	}*/
}

// check if the parser is in an api documentation block
// returns true if the line starts with "//api "
bool Application::startsWith(const std::string& _line, const std::string& _prefix) {
	return _line.compare(0, _prefix.size(), _prefix) == 0;
}

void Application::parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, const std::string& _parameterType) {
	std::cout << "Parsing parameter: " << _param << "\n";
	std::list<std::string> splittedParamList = ot::String::split(_param, " ");
	std::vector<std::string> splittedParamVector(splittedParamList.begin(), splittedParamList.end());

	std::string macro = splittedParamVector[0];
	std::cout << "The first string is: " << macro << "\n";
	std::size_t sizeOfMacroString = macro.size() + 1;
	std::cout << sizeOfMacroString << "\n";

	_parameter.setMacro(macro);
	std::cout << "Macro set in parameter: " << _parameter.getMacro() << "\n";

	std::string dataType = splittedParamVector[1];
	// data type is Unsigned Integer 64
	if (dataType == "Unsigned" || dataType == "unsigned") {
		std::string dataType2 = splittedParamVector[2];
		std::string dataType3 = splittedParamVector[3];
		std::string unsignedInt64 = dataType + " " + dataType2 + " " + dataType3;
		std::cout << "The second string is: " << unsignedInt64 << "\n";

		std::size_t sizeOfUnsignedInt64String = unsignedInt64.size() + 1;
		std::cout << sizeOfUnsignedInt64String << "\n";

		_parameter.setDataType(Parameter::UnsignedInteger64);
		std::cout << "Data type Unsigned Integer 64 set in parameter: " << _parameter.getDataTypeString() << "\n";

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfUnsignedInt64String);
		std::cout << "The third string is: " << paramDescription << "\n";

		_parameter.setDescription(paramDescription);
		std::cout << "Description set in parameter: " << _parameter.getDescription() << "\n";
	}
	else {
		std::cout << "The second string is: " << dataType << "\n";

		std::size_t sizeOfDataTypeString = dataType.size() + 1;
		std::cout << sizeOfDataTypeString << "\n";

		if (dataType == "UID" || dataType == "uid") {
			_parameter.setDataType(Parameter::UnsignedInteger64);
			std::cout << "Data type Unsigned Integer 64 set in parameter: " << _parameter.getDataTypeString() << "\n";
		}

		else if (dataType == "Boolean" || dataType == "boolean" || dataType == "bool") {
			_parameter.setDataType(Parameter::Boolean);
			std::cout << "Data type Boolean set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Char" || dataType == "char") {
			_parameter.setDataType(Parameter::Char);
			std::cout << "Data type Char set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Integer" || dataType == "integer" || dataType == "int") {
			_parameter.setDataType(Parameter::Integer);
			std::cout << "Data type Integer set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Float" || dataType == "float") {
			_parameter.setDataType(Parameter::Float);
			std::cout << "Data type Float set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Double" || dataType == "double") {
			_parameter.setDataType(Parameter::Double);
			std::cout << "Data type Double set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "String" || dataType == "string" || dataType == "str") {
			_parameter.setDataType(Parameter::String);
			std::cout << "Data type String set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Array" || dataType == "array") {
			_parameter.setDataType(Parameter::Array);
			std::cout << "Data type Array set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Object" || dataType == "object" || dataType == "obj") {
			_parameter.setDataType(Parameter::Object);
			std::cout << "Data type Object set in parameter: " << _parameter.getDataTypeString() << "\n";
		}
		else if (dataType == "Enum" || dataType == "enum") {
			_parameter.setDataType(Parameter::Enum);
			std::cout << "Data type Enum set in parameter: " << _parameter.getDataTypeString() << "\n";
		}

		std::string paramDescription = _param.substr(sizeOfMacroString + sizeOfDataTypeString);
		std::cout << "The third string is: " << paramDescription << "\n";
		_parameter.setDescription(paramDescription);
		std::cout << "Description set in parameter: " << _parameter.getDescription() << "\n";
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

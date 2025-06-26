// Open Twin header
#include "OTSystem/FileSystem.h"

//project header
#include "Application.h"

// std header
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


void Application::run(void)
{
	std::cout << "Application is running.\n";
	searchForServices();
}

void Application::searchForServices(void)
{
	std::cout << "Searching for Services.\n";


	// search in Open Twin Services
	const std::string path = "C:\\OT\\OpenTwin\\Services";

	try {
		std::list<std::string> allServices = ot::FileSystem::getFiles(path,{".vcxproj"}, ot::FileSystem::FileSystemOption::Recursive);

		std::cout << "\nC++-Files:\n";
		for (const std::string& file : allServices) {
			std::cout << "  " << file << "\n";
			searchIncludeAndSrcDirectoryFiles(file);
		}
		// output: Services\...
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}

/*
	// search in one Service
	const std::string pathToAuthorisationService = "C:\\OT\\OpenTwin\\Services\\AuthorisationService";

	try {
		std::list<std::string> allServiceFiles = ot::FileSystem::getFiles(pathToAuthorisationService, { ".vcxproj" });

		std::cout << "\nC++-Files:\n";
		for (const std::string& file : allServiceFiles) {
			std::cout << "  " << file << "\n";
			searchIncludeAndSrcDirectoryFiles(file);
		}
		// output: C:\OT\OpenTwin\Services\AuthorisationService\AuthorisationService.vcxproj
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
*/
}

void Application::searchIncludeAndSrcDirectoryFiles(const std::string& _file)
{
	std::cout << "Searching for include und src directories of the given file: " << _file << "\n";
	
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

void Application::searchSrcDirectoryFiles(const std::string& _srcDirectory)
{
	try {
		std::list<std::string> srcFiles = ot::FileSystem::getFiles(_srcDirectory, {});
		std::cout << "Collected Files in src-directory\n";

		for (const std::string& datei : srcFiles) {
			std::cout << "  " << datei << "\n";
		}
		// output: Collected Files in src-directory C:\OT\OpenTwin\Services\AuthorisationService\src\dllmain.cpp
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}

void Application::searchIncludeDirectoryFiles(const std::string& _includeDirectory)
{
	try {
		std::list<std::string>  includeFiles = ot::FileSystem::getFiles(_includeDirectory, {});
		std::cout << "Collected Files in include-directory\n";

		for (const std::string& datei : includeFiles) {
			std::cout << "  " << datei << "\n";
		}
		// output: Collected Files in include-directory 
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error by getFiles: " << e.what() << "\n";
	}
}



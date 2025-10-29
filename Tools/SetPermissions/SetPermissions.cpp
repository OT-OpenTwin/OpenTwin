// @otlicense

// std header
#include <iostream>
#include <string>
#include <fstream>

#include "OTSystem/SystemProcess.h"

void setPermissions(const std::string& path)
{
	std::string exeName = "C:\\Windows\\System32\\icacls.exe";
	std::string commandLine = exeName + " \"" + path + "\" /grant NETWORKSERVICE:(OI)(CI)F /T";

	HANDLE processHandle = OT_INVALID_PROCESS_HANDLE;
	ot::RunResult result =	ot::SystemProcess::runApplication(exeName, commandLine, processHandle);
	if (result.isOk())
	{
		std::cout << "Successfully set permissions for NETWORK SERVICE on " << path << std::endl;
	}
	else
	{
		std::cout << "Failed to set permissions for " << path << std::endl;
		std::cout << "Error code: " << result.getErrorCode() << " error message: " << result.getErrorMessage() << std::endl;
	}
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cout << "Usage: SetPermissions.exe <db_path> <log_path>" << std::endl;
		return 1;
	}

	std::string db_path  = argv[1];
	std::string log_path = argv[2];

	setPermissions(db_path);
	setPermissions(log_path);

	return 0;
}
#pragma once

// Required headers
#include <string>

// OS dependent header
#if defined _WIN32 || defined _WIN64

#define OS_IS_WINDOWS 1
#include <Windows.h>
#elif defined unix || defined __unix || defined __unix__



#endif
namespace sys {
	namespace cmd {
		bool runApp(
			const std::string				_filePath,
			bool							_waitForResponse = true
		);

		bool runApp(
			const std::string				_applicationPath,
			const std::string				_commandLine,
			HANDLE						   &_hProcess
		);

		std::string getCurrentExecutableDirectory(void);
	}
}

//! @file AppExitCodes.h
//! @author Alexander Kuester (alexk95)
//! @date April 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemAPIExport.h"

// std header
#include <string>

namespace ot {

	//! @class AppExitCode
	//! @brief Exit codes used by OpenTwin applications.
	class OT_SYS_API_EXPORT AppExitCode {
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Framework

		//! @enum FrameworkExitCode
		//! @brief Framework exit codes (100 - 999).
		//! Exit codes used by the OpenTwin framework.
		//! These codes are used to indicate specific errors or conditions that occur within the framework itself.
		//! They are not intended for use by individual applications or modules.
		enum FrameworkExitCode : int {
			GeneralFrameworkError      = 100 //! @brief General framework error.
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// IO

		//! @brief IO exit codes (1000 - 1099).
		//! IO exit codes are used to indicate specific errors or conditions that occur during input/output operations (e.g. while sending a request or reading a file).
		enum IOExitCode : int {
			GeneralIOError             = 1000, //! @brief General IO error.
			FileNotFound               = 1001, //! @brief File not found error.
			InvalidFileFormat          = 1002, //! @brief Invalid file format error.
			PermissionDenied           = 1003, //! @brief Permission denied error.
			OutOfMemory                = 1004, //! @brief Out of memory error.
			InvalidPath                = 1005, //! @brief Invalid path error.
			SendFailed                 = 1006  //! @brief Send failed error.
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Service Data

		//! @brief Service data exit codes (1100 - 1199).
		//! Exit codes generated related to service data conflicts/errors.
		enum ServiceDataExitCode : int {
			GeneralServiceDataError    = 1100, //! @brief General service data error.
			ServiceUrlMissing          = 1101, //! @brief Service URL is missing.
			ServiceUrlInvalid          = 1102, //! @brief Service URL is invalid.

			AuthUrlMissing             = 1111, //! @brief Authentication URL is missing.
			AuthUrlInvalid             = 1112, //! @brief Authentication URL is invalid.
			GSSUrlMissing              = 1121, //! @brief Global Session Service URL is missing.
			GSSUrlInvalid              = 1122, //! @brief Global Session Service URL is invalid.
			LSSUrlMissing              = 1131, //! @brief Local Session Service URL is missing.
			LSSUrlInvalid              = 1132, //! @brief Local Session Service URL is invalid.
			GDSUrlMissing              = 1141, //! @brief Global Directory Service URL is missing.
			GDSUrlInvalid              = 1142, //! @brief Global Directory Service URL is invalid.
			LDSUrlMissing              = 1151, //! @brief Local Directory Service URL is missing.
			LDSUrlInvalid              = 1152  //! @brief Local Directory Service URL is invalid.

		};
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Service State

		//! @brief Service state exit codes (1200 - 1299).
		//! Exit codes generated related to service state errors.
		enum ServiceStateExitCode : int {
			GeneralServiceStateError   = 1200, //! @brief General service state error.
			AuthRegistrationFailed     = 1201, //! @brief Authentication Service registration failed.
			GSSRegistrationFailed      = 1202, //! @brief Global Session Service registration failed.
			LSSRegistrationFailed      = 1203, //! @brief Local Session Service registration failed.
			GDSRegistrationFailed      = 1204, //! @brief Global Directory Service registration failed.
			LDSRegistrationFailed      = 1205, //! @brief Local Directory Service registration failed.

			AuthNotRunning             = 1206, //! @brief Authentication Service is not running.
			GSSNotRunning              = 1207, //! @brief Global Session Service is not running.
			LSSNotRunning              = 1208, //! @brief Local Session Service is not running.
			GDSNotRunning              = 1209, //! @brief Global Directory Service is not running.
			LDSNotRunning              = 1210  //! @brief Local Directory Service is not running.
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Base

		//! @brief Data Base related exit codes (1800 - 1899).
		enum DataBaseExitCode : int {
			GeneralDataBaseError       = 1800, //! @brief General database error.
			DataBaseUrlMissing         = 1801, //! @brief Database URL is missing.
			DataBaseUrlInvalid         = 1802, //! @brief Database URL is invalid.
			DataBaseConnectionFailed   = 1803, //! @brief Database connection failed.
			DataBaseInvalidCredentials = 1804  //! @brief Invalid database credentials.
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// General

		//! @brief General exit codes (7700+).
		//! Contains all general and uncategorized exit codes.
		enum GeneralExitCode : int {
			Success                    = 0,    //! @brief General success exit code.

			GeneralError               = 7700, //! @brief General error exit code.
			UnknownError               = 7701, //! @brief Unknown error exit code.
			EmergencyShutdown          = 7702, //! @brief Emergency shutdown exit code.
			EnvironmentError           = 7703, //! @brief Environment error exit code.
			ConfigurationBroken        = 7704, //! @brief Configuration broken exit code.
			OpenGLError                = 7705, //! @brief OpenGL error exit code.
			AssetError                 = 7706, //! @brief Asset error exit code.
			ComponentError             = 7707, //! @brief Component error exit code.

			LogInError                 = 7800, //! @brief Log in error exit code.
			FailedToConvertPort        = 7801, //! @brief Failed to convert port exit code.
			KeepAliveFailed            = 7802, //! @brief Keep alive failed exit code.
			InitializationTimeout      = 7803  //! @brief Initialization timeout exit code.
		};

		//! @brief Returns a humanly readable string representation of the given exit code.
		static std::string toString(int _exitCode);

	private:
		AppExitCode() = delete;
	};

}
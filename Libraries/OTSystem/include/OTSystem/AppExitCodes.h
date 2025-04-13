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

	//! @brief Exit codes used by OpenTwin applications.
	class OT_SYS_API_EXPORT AppExitCode {
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Framework

		//! @brief Framework exit codes (100 - 999).
		//! Exit codes used by the OpenTwin framework.
		//! These codes are used to indicate specific errors or conditions that occur within the framework itself.
		//! They are not intended for use by individual applications or modules.
		enum FrameworkExitCode : int {
			GeneralFrameworkError      = 100
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// IO

		//! @brief IO exit codes (1000 - 1099).
		enum IOExitCode : int {
			GeneralIOError             = 1000,
			FileNotFound               = 1001,
			InvalidFileFormat          = 1002,
			PermissionDenied           = 1003,
			OutOfMemory                = 1004,
			InvalidPath                = 1005,
			SendFailed                 = 1006
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Service Data

		//! @brief Service data exit codes (1100 - 1199).
		//! Exit codes generated related to service data conflicts/errors.
		enum ServiceDataExitCode : int {
			GeneralServiceDataError    = 1100,
			ServiceUrlMissing          = 1101,
			ServiceUrlInvalid          = 1102,

			AuthUrlMissing             = 1111,
			AuthUrlInvalid             = 1112,
			GSSUrlMissing              = 1121,
			GSSUrlInvalid              = 1122,
			LSSUrlMissing              = 1131,
			LSSUrlInvalid              = 1132,
			GDSUrlMissing              = 1141,
			GDSUrlInvalid              = 1142,
			LDSUrlMissing              = 1151,
			LDSUrlInvalid              = 1152

		};
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Service State

		//! @brief Service state exit codes (1200 - 1299).
		//! Exit codes generated related to service state errors.
		enum ServiceStateExitCode : int {
			GeneralServiceStateError   = 1200,
			AuthRegistrationFailed     = 1201,
			GSSRegistrationFailed      = 1202,
			LSSRegistrationFailed      = 1203,
			GDSRegistrationFailed      = 1204,
			LDSRegistrationFailed      = 1205,

			AuthNotRunning             = 1206,
			GSSNotRunning              = 1207,
			LSSNotRunning              = 1208,
			GDSNotRunning              = 1209,
			LDSNotRunning              = 1210
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Base

		//! @brief Data Base related exit codes (1800 - 1899).
		enum DataBaseExitCode : int {
			GeneralDataBaseError       = 1800,
			DataBaseUrlMissing         = 1801,
			DataBaseUrlInvalid         = 1802,
			DataBaseConnectionFailed   = 1803,
			DataBaseInvalidCredentials = 1804
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// General

		//! @brief General exit codes (7000+).
		enum GeneralExitCode : int {
			//! @brief General success exit code (0).
			Success                    = 0,

			GeneralError               = 7700,
			UnknownError               = 7701,
			EmergencyShutdown          = 7702,
			EnvironmentError           = 7703,
			ConfigurationBroken        = 7704,
			OpenGLError                = 7705,
			AssetError                 = 7706,
			ComponentError             = 7707,

			LogInError                 = 7800,
			FailedToConvertPort        = 7801,
			KeepAliveFailed            = 7802,
			InitializationTimeout      = 7803
		};

		//! @brief Returns a humanly readable string representation of the given exit code.
		static std::string toString(int _exitCode);

	private:
		AppExitCode() = delete;
	};

}
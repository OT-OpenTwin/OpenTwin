// @otlicense
// File: AppExitCodes.h
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

		//! @enum StateExitCode
		//! @brief State exit codes (0 - 499).
		//! Exit codes used to indicate the result of a state or operation.
		enum StateExitCode : int
		{
			Success                    = 0, //! @brief Success exit code (exit 0).
			ResultTrue                 = 1, //! @brief Result true.
			ResultFalse                = 2, //! @brief Result false.
			ResultRetry                = 3, //! @brief Result retry.
			ResultCancel               = 4, //! @brief Result cancel.
			ResultSkipped              = 5, //! @brief Result skipped.
			ResultPartial              = 6, //! @brief Result partial.
			ResultPending              = 7, //! @brief Result pending.
			ResultAlreadyDone          = 8, //! @brief Result already done.
			ResultNotRequired          = 9, //! @brief Result not required.

			StateExitCode_IteratorFirst = 0,
			StateExitCode_IteratorLast = 499
		};

		//! @enum FrameworkExitCode
		//! @brief Framework exit codes (500 - 999).
		//! Exit codes used by the OpenTwin framework.
		//! These codes are used to indicate specific errors or conditions that occur within the framework itself.
		//! They are not intended for use by individual applications or modules.
		enum FrameworkExitCode : int
		{
			GeneralFrameworkError      = 500,

			FrameworkExitCode_IteratorFirst = 500,
			FrameworkExitCode_IteratorLast  = 999
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// IO

		//! @brief IO error codes (1000 - 1099).
		//! IO exit codes are used to indicate specific errors or conditions that occur during input/output operations (e.g. while sending a request or reading a file).
		enum IOErrorCode : int {
			GeneralIOError             = 1000, //! @brief General IO error.
			FileNotFound               = 1001, //! @brief File not found error.
			InvalidFileFormat          = 1002, //! @brief Invalid file format error.
			PermissionDenied           = 1003, //! @brief Permission denied error.
			OutOfMemory                = 1004, //! @brief Out of memory error.
			InvalidPath                = 1005, //! @brief Invalid path error.
			SendFailed                 = 1006, //! @brief Send failed error.

			IOExitCode_IteratorFirst   = 1000,
			IOExitCode_IteratorLast    = 1099
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Service Data

		//! @brief Service data error codes (1100 - 1199).
		//! Exit codes generated related to service data conflicts/errors.
		enum ServiceDataErrorCode : int {
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
			LDSUrlInvalid              = 1152, //! @brief Local Directory Service URL is invalid.

			ServiceDataExitCode_IteratorFirst = 1100,
			ServiceDataExitCode_IteratorLast  = 1199
		};
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Service State

		//! @brief Service state exit codes (1200 - 1299).
		//! Exit codes generated related to service state errors.
		enum ServiceStateErrorCode : int {
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
			LDSNotRunning              = 1210, //! @brief Local Directory Service is not running.

			ServiceStateExitCode_IteratorFirst = 1200,
			ServiceStateExitCode_IteratorLast  = 1299
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Base

		//! @brief Data Base related exit codes (1800 - 1899).
		enum DataBaseErrorCode : int {
			GeneralDataBaseError       = 1800, //! @brief General database error.
			DataBaseUrlMissing         = 1801, //! @brief Database URL is missing.
			DataBaseUrlInvalid         = 1802, //! @brief Database URL is invalid.
			DataBaseConnectionFailed   = 1803, //! @brief Database connection failed.
			DataBaseInvalidCredentials = 1804, //! @brief Invalid database credentials.

			DataBaseExitCode_IteratorFirst = 1800,
			DataBaseExitCode_IteratorLast  = 1899
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// General

		//! @brief General exit codes (7700-7999).
		//! Contains all general and uncategorized exit codes.
		enum GeneralErrorCode : int {
			GeneralError               = 7700, //! @brief General error.
			UnknownError               = 7701, //! @brief Unknown error.
			EmergencyShutdown          = 7702, //! @brief Emergency shutdown.
			EnvironmentError           = 7703, //! @brief Environment error.
			ConfigurationBroken        = 7704, //! @brief Configuration broken.
			OpenGLError                = 7705, //! @brief OpenGL error.
			AssetError                 = 7706, //! @brief Asset error.
			ComponentError             = 7707, //! @brief Component error.
			GeneralTimeout		       = 7708, //! @brief General timeout.

			LogInError                 = 7800, //! @brief Log in error.
			FailedToConvertPort        = 7801, //! @brief Failed to convert port.
			KeepAliveFailed            = 7802, //! @brief Keep alive failed.
			InitializationTimeout      = 7803, //! @brief Initialization timeout.
			RequestBlocked             = 7900, //! @brief Request blocked.
			RequestIgnored             = 7901, //! @brief Request ignored.
			QueueExecutionBlocked	   = 7902, //! @brief Queue execution blocked.

			GeneralExitCode_IteratorFirst = 7700,
			GeneralExitCode_IteratorLast  = 7999
		};

		//! @brief Returns a humanly readable string representation of the given exit code.
		static std::string toString(int _exitCode);

	private:
		AppExitCode() = delete;
	};

}
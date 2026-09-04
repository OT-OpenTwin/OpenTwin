// @otlicense
// File: AppExitCodes.cpp
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

// OpenTwin header
#include "OTSystem/AppExitCodes.h"

// std header
#include <cassert>

std::string ot::AppExitCode::toString(int _exitCode) {
	switch (_exitCode) {
	case StateExitCode::Success: return "Success";
	case StateExitCode::ResultTrue: return "Result true";
	case StateExitCode::ResultFalse: return "Result false";
	case StateExitCode::ResultRetry: return "Result retry";
	case StateExitCode::ResultCancel: return "Result cancel";
	case StateExitCode::ResultSkipped: return "Result skipped";
	case StateExitCode::ResultPartial: return "Result partial";
	case StateExitCode::ResultPending: return "Result pending";
	case StateExitCode::ResultAlreadyDone: return "Result already done";
	case StateExitCode::ResultNotRequired: return "Result not required";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Framework

	case FrameworkExitCode::GeneralFrameworkError: return "General framework error";

	// ###########################################################################################################################################################################################################################################################################################################################

	// IO

	case IOErrorCode::GeneralIOError: return "General IO error";
	case IOErrorCode::FileNotFound: return "File not found";
	case IOErrorCode::InvalidFileFormat: return "Invalid file format";
	case IOErrorCode::PermissionDenied: return "Permission denied";
	case IOErrorCode::OutOfMemory: return "Out of memory";
	case IOErrorCode::InvalidPath: return "Invalid path";
	case IOErrorCode::SendFailed: return "Send failed";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service Data

	case ServiceDataErrorCode::GeneralServiceDataError: return "General service data error";
	case ServiceDataErrorCode::ServiceUrlMissing: return "Service URL is missing";
	case ServiceDataErrorCode::ServiceUrlInvalid: return "Service URL is invalid";
	case ServiceDataErrorCode::AuthUrlMissing: return "Authentication URL is missing";
	case ServiceDataErrorCode::AuthUrlInvalid: return "Authentication URL is invalid";
	case ServiceDataErrorCode::GSSUrlMissing: return "Global Session Service URL is missing";
	case ServiceDataErrorCode::GSSUrlInvalid: return "Global Session Service URL is invalid";
	case ServiceDataErrorCode::LSSUrlMissing: return "Local Session Service URL is missing";
	case ServiceDataErrorCode::LSSUrlInvalid: return "Local Session Service URL is invalid";
	case ServiceDataErrorCode::GDSUrlMissing: return "Global Directory Service URL is missing";
	case ServiceDataErrorCode::GDSUrlInvalid: return "Global Directory Service URL is invalid";
	case ServiceDataErrorCode::LDSUrlMissing: return "Local Directory Service URL is missing";
	case ServiceDataErrorCode::LDSUrlInvalid: return "Local Directory Service URL is invalid";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service State

	case ServiceStateErrorCode::GeneralServiceStateError: return "General service state error";
	case ServiceStateErrorCode::AuthRegistrationFailed: return "Authentication Service registration failed";
	case ServiceStateErrorCode::GSSRegistrationFailed: return "Global Session Service registration failed";
	case ServiceStateErrorCode::LSSRegistrationFailed: return "Local Session Service registration failed";
	case ServiceStateErrorCode::GDSRegistrationFailed: return "Global Directory Service registration failed";
	case ServiceStateErrorCode::LDSRegistrationFailed: return "Local Directory Service registration failed";

	case ServiceStateErrorCode::AuthNotRunning: return "Authentication Service is not running";
	case ServiceStateErrorCode::GSSNotRunning: return "Global Session Service is not running";
	case ServiceStateErrorCode::LSSNotRunning: return "Local Session Service is not running";
	case ServiceStateErrorCode::GDSNotRunning: return "Global Directory Service is not running";
	case ServiceStateErrorCode::LDSNotRunning: return "Local Directory Service is not running";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data Base

	case DataBaseErrorCode::GeneralDataBaseError: return "General database error";
	case DataBaseErrorCode::DataBaseUrlMissing: return "Database URL is missing";
	case DataBaseErrorCode::DataBaseUrlInvalid: return "Database URL is invalid";
	case DataBaseErrorCode::DataBaseConnectionFailed: return "Database connection failed";
	case DataBaseErrorCode::DataBaseInvalidCredentials: return "Invalid database credentials";

	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	case GeneralErrorCode::GeneralError: return "General error";
	case GeneralErrorCode::UnknownError: return "Unknown error";
	case GeneralErrorCode::EmergencyShutdown: return "Emergency shutdown";
	case GeneralErrorCode::EnvironmentError: return "Environment error";
	case GeneralErrorCode::ConfigurationBroken: return "Configuration is broken";
	case GeneralErrorCode::OpenGLError: return "OpenGL error";
	case GeneralErrorCode::AssetError: return "Asset error";
	case GeneralErrorCode::ComponentError: return "Component error";
	case GeneralErrorCode::GeneralTimeout: return "General timeout";

	case GeneralErrorCode::LogInError: return "Log in error";
	case GeneralErrorCode::FailedToConvertPort: return "Failed to convert port";
	case GeneralErrorCode::KeepAliveFailed: return "Keep alive failed";
	case GeneralErrorCode::InitializationTimeout: return "Initialization timeout";

	default:
		assert(0);
		return "<Unknown exit code>";
	}
}

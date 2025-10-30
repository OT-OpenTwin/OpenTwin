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
	case GeneralExitCode::Success: return "Success";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Framework

	case FrameworkExitCode::GeneralFrameworkError: return "General framework error";

	// ###########################################################################################################################################################################################################################################################################################################################

	// IO

	case IOExitCode::GeneralIOError: return "General IO error";
	case IOExitCode::FileNotFound: return "File not found";
	case IOExitCode::InvalidFileFormat: return "Invalid file format";
	case IOExitCode::PermissionDenied: return "Permission denied";
	case IOExitCode::OutOfMemory: return "Out of memory";
	case IOExitCode::InvalidPath: return "Invalid path";
	case IOExitCode::SendFailed: return "Send failed";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service Data

	case ServiceDataExitCode::GeneralServiceDataError: return "General service data error";
	case ServiceDataExitCode::ServiceUrlMissing: return "Service URL is missing";
	case ServiceDataExitCode::ServiceUrlInvalid: return "Service URL is invalid";

	case ServiceDataExitCode::AuthUrlMissing: return "Authentication URL is missing";
	case ServiceDataExitCode::AuthUrlInvalid: return "Authentication URL is invalid";
	case ServiceDataExitCode::GSSUrlMissing: return "Global Session Service URL is missing";
	case ServiceDataExitCode::GSSUrlInvalid: return "Global Session Service URL is invalid";
	case ServiceDataExitCode::LSSUrlMissing: return "Local Session Service URL is missing";
	case ServiceDataExitCode::LSSUrlInvalid: return "Local Session Service URL is invalid";
	case ServiceDataExitCode::GDSUrlMissing: return "Global Directory Service URL is missing";
	case ServiceDataExitCode::GDSUrlInvalid: return "Global Directory Service URL is invalid";
	case ServiceDataExitCode::LDSUrlMissing: return "Local Directory Service URL is missing";
	case ServiceDataExitCode::LDSUrlInvalid: return "Local Directory Service URL is invalid";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service State

	case ServiceStateExitCode::GeneralServiceStateError: return "General service state error";
	case ServiceStateExitCode::AuthRegistrationFailed: return "Authentication Service registration failed";
	case ServiceStateExitCode::GSSRegistrationFailed: return "Global Session Service registration failed";
	case ServiceStateExitCode::LSSRegistrationFailed: return "Local Session Service registration failed";
	case ServiceStateExitCode::GDSRegistrationFailed: return "Global Directory Service registration failed";
	case ServiceStateExitCode::LDSRegistrationFailed: return "Local Directory Service registration failed";

	case ServiceStateExitCode::AuthNotRunning: return "Authentication Service is not running";
	case ServiceStateExitCode::GSSNotRunning: return "Global Session Service is not running";
	case ServiceStateExitCode::LSSNotRunning: return "Local Session Service is not running";
	case ServiceStateExitCode::GDSNotRunning: return "Global Directory Service is not running";
	case ServiceStateExitCode::LDSNotRunning: return "Local Directory Service is not running";

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data Base

	case DataBaseExitCode::GeneralDataBaseError: return "General database error";
	case DataBaseExitCode::DataBaseUrlMissing: return "Database URL is missing";
	case DataBaseExitCode::DataBaseUrlInvalid: return "Database URL is invalid";
	case DataBaseExitCode::DataBaseConnectionFailed: return "Database connection failed";
	case DataBaseExitCode::DataBaseInvalidCredentials: return "Invalid database credentials";

	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	case GeneralExitCode::GeneralError: return "General error";
	case GeneralExitCode::UnknownError: return "Unknown error";
	case GeneralExitCode::EmergencyShutdown: return "Emergency shutdown";
	case GeneralExitCode::EnvironmentError: return "Environment error";
	case GeneralExitCode::ConfigurationBroken: return "Configuration is broken";
	case GeneralExitCode::OpenGLError: return "OpenGL error";
	case GeneralExitCode::AssetError: return "Asset error";
	case GeneralExitCode::ComponentError: return "Component error";
	case GeneralExitCode::GeneralTimeout: return "General timeout";

	case GeneralExitCode::LogInError: return "Log in error";
	case GeneralExitCode::FailedToConvertPort: return "Failed to convert port";
	case GeneralExitCode::KeepAliveFailed: return "Keep alive failed";
	case GeneralExitCode::InitializationTimeout: return "Initialization timeout";

	default:
		assert(0);
		return "<Unknown exit code>";
	}
}

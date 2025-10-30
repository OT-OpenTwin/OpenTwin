// @otlicense
// File: ExternalLibrary.cpp
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

// OToolkit header
#include "ExternalLibrary.h"

// OToolkit API header
#include "OToolkitAPI/APIFunctions.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// std header
#include <codecvt>

#define EXTL_LOG(___message) OTOOLKIT_LOG("External Library", ___message)
#define EXTL_LOGW(___message) OTOOLKIT_LOGW("External Library", ___message)
#define EXTL_LOGE(___message) OTOOLKIT_LOGE("External Library", ___message)

ExternalLibrary::ExternalLibrary()
	: m_instance(nullptr)
{

}

ExternalLibrary::~ExternalLibrary() {
	if (m_instance) {
		FreeLibrary(m_instance);
		m_instance = nullptr;
	}
}

bool ExternalLibrary::load(const QString& _path) {
	// Ensure library not loaded
	if (m_instance) {
		EXTL_LOGE("Library already loaded");
		return false;
	}

	std::wstring path = _path.toStdWString();

	// Load library
	m_instance = LoadLibrary(path.c_str());
	if (!m_instance) {
		EXTL_LOGE("Failed to load library \"" + _path + "\"");
		return false;
	}

	// Load entry point
	otoolkit::ImportToolsFunction importFun = (otoolkit::ImportToolsFunction)GetProcAddress(m_instance, OTOOLKIT_CreateToolsFunctionName);
	if (!importFun) {
		EXTL_LOGE("Create tools function not found { \"Function\": \"" OTOOLKIT_CreateToolsFunctionName "\", \"Library\": \"" + _path + "\" }");
		FreeLibrary(m_instance);
		m_instance = nullptr;
		return false;
	}

	// Let library create the tools
	importFun(m_tools);

	return true;
}
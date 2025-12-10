// @otlicense
// File: FixturePythonManifest.h
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

// Google Test header
#include <gtest/gtest.h>

// OpenTwin header
#include "FileHelper.h"
#include "EntityPythonManifest.h"

class FixturePythonManifest : public testing::Test
{
public:
	std::list<std::string> getPackageList(const std::string _text)
	{
		EntityPythonManifest manifest;
		return manifest.getPackageList(_text).value_or(std::list<std::string>{});;
	}
	bool environmentHasChanged(const std::string& _oldContent, const std::string& _newContent){
		EntityPythonManifest manifest;
		manifest.m_manifestText = _oldContent;
		return manifest.environmentHasChanged(_newContent);
	}
	//! @brief Base manifest with multiple packages
	std::string getManifestA()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestA.txt");
		return manifest;
	}
	
	//! @brief Derived from ManifestA without the scipy package
	std::string getManifestB()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestB.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but matplotlib has different version
	std::string getManifestC()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestC.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but with empty lines tabs and spaces
	std::string getManifestD()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestD.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but with broken syntax
	std::string getManifestE()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestE.txt");
		return manifest;
	}
};

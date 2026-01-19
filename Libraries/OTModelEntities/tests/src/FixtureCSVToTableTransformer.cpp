// @otlicense
// File: FixtureCSVToTableTransformer.cpp
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

#include "FixtureCSVToTableTransformer.h"
#include "FileHelper.h"
#include <fstream>
#include "OTSystem/OperatingSystem.h"


FixtureCSVToTableTransformer::FixtureCSVToTableTransformer()
	:m_filePath(FileHelper::getFilePath())
{
}

std::string FixtureCSVToTableTransformer::loadFileContent(std::string fullPath)
{
	auto fileContent = FileHelper::extractFileContentAsBinary(fullPath);
	std::string fileText(fileContent.begin(), fileContent.end());
	return fileText;
}

ot::GenericDataStructMatrix FixtureCSVToTableTransformer::getTable(const std::string& text, CSVProperties& properties)
{
	ot::GenericDataStructMatrix table = m_extractor(text, properties);
	return table;
}

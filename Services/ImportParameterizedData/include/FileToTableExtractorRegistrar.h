// @otlicense
// File: FileToTableExtractorRegistrar.h
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
#include "FileToTableExtractorFactory.h"
#include <string>

//! @brief Class for registrating a new file type. 
//! If a TableExtractor shall support a new file ending, all it needs is a static FileToTableExtractorRegistrar with the string of the new supported file extension.
template<class T>
class FileToTableExtractorRegistrar
{
public:
	FileToTableExtractorRegistrar(std::string fileExtension)
	{
		FileToTableExtractorFactory::GetInstance()->RegisterFactoryFunction(fileExtension, [](void)->TableExtractor * {return new T(); });
	}
};

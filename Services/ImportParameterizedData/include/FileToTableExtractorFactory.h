// @otlicense
// File: FileToTableExtractorFactory.h
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
#include <map>
#include <functional>
#include <string>
#include <memory>
#include "TableExtractor.h"
#include <mutex>

//! @brief Singleton which the FileToTableExtractorRegistrar class uses to register a new file eding. Registered classes can be created via the Create function.
class FileToTableExtractorFactory
{
public:
	static FileToTableExtractorFactory* GetInstance();
	std::shared_ptr<TableExtractor> Create(std::string name) const;
	void RegisterFactoryFunction(std::string name, std::function<TableExtractor*(void)> classFactoryFunction);

private:
	std::map<std::string, std::function<TableExtractor*(void)>> factoryFunctionRegistry;
	std::mutex mtx;
	FileToTableExtractorFactory() {};
	~FileToTableExtractorFactory() {};
	FileToTableExtractorFactory(const FileToTableExtractorFactory& other) = delete;
	FileToTableExtractorFactory& operator=(const FileToTableExtractorFactory& other) = delete;
};
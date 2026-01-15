// @otlicense
// File: UniqueFileName.h
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
#ifndef INCLUDE_UNIQUEFILENAME_H_
#define INCLUDE_UNIQUEFILENAME_H_

#include <iostream>
#include <string>

using string = std::string;

namespace DataStorageAPI
{
	class __declspec(dllexport) UniqueFileName
	{

	public:
		 string GetUniqueFilePath(string directory, string fileName, string fileExtension);
		 string GetUniqueFilePath(string filePath);
		 string GetUniqueFilePathUsingDirectory(string directory, string fileExtension = ".dat");
		 string GetRelativeFilePath(string absolutePath);
		 bool CheckFilePathExists(string filePath);

	private:
		bool DirectoryExists(std::string directory);
	};
};

#endif

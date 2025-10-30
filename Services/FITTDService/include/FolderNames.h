// @otlicense
// File: FolderNames.h
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
#include <string>

class FolderNames
{
public:
	static const std::string GetFolderNameSignalType() { return folderNameSignalType; }
	static const std::string GetFolderNameSolver() { return folderNameSolver; }
	static const std::string GetFolderNamePorts() { return folderNamePorts; }
	static const std::string GetFolderNameMonitors() { return folderNameMonitors; }
	static const std::string GetFolderNameRawResultBase() { return folderNameRawResultBase; }

private:
	static const std::string folderNameSignalType;
	static const std::string folderNameSolver;
	static const std::string folderNamePorts;
	static const std::string folderNameMonitors;
	static const std::string folderNameRawResultBase;
};
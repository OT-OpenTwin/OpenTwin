// @otlicense
// File: FMNewProjectInfo.h
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
#include "OTFMC/FMCTypes.h"
#include "OTFMC/FMIgnoreFile.h"

namespace ot {

	class FMNewProjectInfo {
		OT_DECL_NOCOPY(FMNewProjectInfo)
		OT_DECL_DEFMOVE(FMNewProjectInfo)
	public:
		FMNewProjectInfo() = default;
		~FMNewProjectInfo() = default;

		void setRootDirectory(const std::string& _rootDirectory) { m_rootDirectory = _rootDirectory; };
		const std::string& getRootDirectory() const { return m_rootDirectory; };

		void setReplaceIgnoreFile(bool _replaceIgnoreFile) { m_replaceIgnoreFile = _replaceIgnoreFile; };
		bool getReplaceIgnoreFile() const { return m_replaceIgnoreFile; };

		void setIgnoreFile(const FMIgnoreFile& _ignoreFile) { m_ignoreFile = _ignoreFile; };
		void setIgnoreFile(FMIgnoreFile&& _ignoreFile) { m_ignoreFile = std::move(_ignoreFile); };
		const FMIgnoreFile& getIgnoreFile() const { return m_ignoreFile; };

	private:
		bool m_replaceIgnoreFile = false;
		std::string m_rootDirectory;
		FMIgnoreFile m_ignoreFile;
		
	};

}
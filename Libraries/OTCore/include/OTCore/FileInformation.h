// @otlicense
// File: FileInformation.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"

// std header
#include <filesystem>

namespace ot {

	class OT_CORE_API_EXPORT FileInformation : public Serializable {
		OT_DECL_DEFCOPY(FileInformation)
		OT_DECL_DEFMOVE(FileInformation)
	public:
		static FileInformation fromFileSystem(const std::string& _path) { return fromFileSystem(std::filesystem::path(_path)); };
		static FileInformation fromFileSystem(const std::filesystem::path& _path);

		FileInformation();
		FileInformation(const ConstJsonObject& _jsonObject);
		virtual ~FileInformation() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setPath(const std::string& _path) { m_path = _path; };
		const std::string& getPath() const { return m_path; };

		void setSize(uint64_t _size) { m_size = _size; };
		uint64_t getSize() const { return m_size; };

		void setLastModified(size_t _lastModified) { m_lastModified = _lastModified; };
		size_t getLastModified() const { return m_lastModified; };

	private:
		std::string m_path;
		uint64_t m_size;
		size_t m_lastModified;

	};

}
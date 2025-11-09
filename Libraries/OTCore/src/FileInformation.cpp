// @otlicense
// File: FileInformation.cpp
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
#include "OTCore/FileInformation.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Static helper

ot::FileInformation ot::FileInformation::fromFileSystem(const std::filesystem::path& _path) {
	// Get file information from file system
	FileInformation fileInfo;

	if (std::filesystem::exists(_path)) {
		fileInfo.setPath(_path.string());
		fileInfo.setSize(static_cast<uint64_t>(std::filesystem::file_size(_path)));

		// Get last modification time
		std::filesystem::file_time_type ftime = std::filesystem::last_write_time(_path);

		// Convert filesystem time to time_t (seconds since epoch)

		// We do not use file_time_type::time_since_epoch() directly as it may not be based on system_clock

		std::chrono::system_clock::time_point sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
		);

		std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
		fileInfo.setLastModified(static_cast<size_t>(cftime));
	}

	return fileInfo;
}

ot::FileInformation::FileInformation()
	: m_lastModified(0), m_size(0)
{

}

ot::FileInformation::FileInformation(const ConstJsonObject& _jsonObject) : FileInformation() {
	setFromJsonObject(_jsonObject);
}

void ot::FileInformation::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Path", JsonString(m_path, _allocator), _allocator);
	_jsonObject.AddMember("Size", m_size, _allocator);
	_jsonObject.AddMember("LastModified", m_lastModified, _allocator);
}

void ot::FileInformation::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_path = json::getString(_jsonObject, "Path");
	m_size = json::getUInt64(_jsonObject, "Size");
	m_lastModified = json::getUInt64(_jsonObject, "LastModified");
}

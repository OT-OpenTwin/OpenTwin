// @otlicense
// File: File.h
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
#include "OTSystem/SystemTypes.h"

// std header
#include <string>
#include <memory>
#include <filesystem>

namespace ot
{

	class OT_SYS_API_EXPORT File
	{
	public:
		File() = default;
		File(const std::filesystem::path& _filePath, const std::string& _uniqueName = "") : m_filePath(_filePath), m_uniqueName(_uniqueName) {};
		File(const File&) = delete;
		File(File&& _other) noexcept;
		~File() = default;

		File& operator=(const File&) = delete;
		File& operator=(File&& _other) noexcept;

		bool isValid() const { return !m_filePath.empty(); };
		bool isEmpty() const { return m_rawDataSize == 0; };

		void setUniqueName(const std::string& _uniqueName) { m_uniqueName = _uniqueName; };
		const std::string& getUniqueName() const { return m_uniqueName; };

		void setFilePath(const std::filesystem::path& _filePath) { m_filePath = _filePath; };
		const std::filesystem::path& getFilePath() const { return m_filePath; };

		void setRawData(std::unique_ptr<uint8_t[]>&& _data, size_t _rawDataSize) { m_rawData = std::move(_data); m_rawDataSize = _rawDataSize; };
		const uint8_t* getRawData() const { return m_rawData.get(); };
		size_t getRawDataSize() const { return m_rawDataSize; };

		[[nodiscard]] bool deleteFile();

		[[nodiscard]] bool readFile();
		[[nodiscard]] static File readFile(const std::filesystem::path& _filePath, const std::string& _uniqueName = "");

		[[nodiscard]] bool writeFile();
		[[nodiscard]] static bool writeFile(const std::filesystem::path& _filePath, const uint8_t* _data, size_t _dataSize);

	private:
		std::string m_uniqueName;
		std::filesystem::path m_filePath;
		std::unique_ptr<uint8_t[]> m_rawData;
		size_t m_rawDataSize = 0;
	};

}
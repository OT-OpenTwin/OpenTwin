// @otlicense
// File: GridFSFileInfo.h
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
#include "DataStorageAPIExport.h"
#include "OTCore/Serializable.h"

namespace ot {

	//! @brief The GridFSFileInfo class holds information about a file stored in GridFS.
	class DATASTORAGE_API GridFSFileInfo : public Serializable {
		OT_DECL_DEFCOPY(GridFSFileInfo)
		OT_DECL_DEFMOVE(GridFSFileInfo)
	public:
		GridFSFileInfo() = default;
		GridFSFileInfo(const ConstJsonObject& _jsonObject);

		virtual ~GridFSFileInfo() = default;

		// Serializable interface

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// Setter / Getter

		//! @brief Set the name used to store the file in GridFS.
		//! @param _fileName Name of the file.
		void setFileName(const std::string& _fileName) { m_fileName = _fileName; };

		//! @brief Returns the name used to store the file in GridFS.
		const std::string& getFileName() const { return m_fileName; };

		//! @brief Set the document ID assigned to the file in GridFS.
		//! @param _documentId Document ID as string.
		void setDocumentId(const std::string& _documentId) { m_documentId = _documentId; };

		//! @brief Returns the document ID assigned to the file in GridFS.
		const std::string& getDocumentId() const { return m_documentId; };

		//! @brief Set whether the file is stored compressed in GridFS.
		//! @param _uncompressedSize Size of the file when uncompressed.
		void setFileCompressed(size_t _uncompressedSize) { 
			m_isCompressed = true; 
			m_uncompressedSize = _uncompressedSize; 
		};

		//! @brief Returns whether the file is stored compressed in GridFS.
		bool isFileCompressed() const { return m_isCompressed; };

		//! @brief Returns the size of the file when uncompressed.
		//! Is only valid if the file is compressed.
		size_t getUncompressedSize() const { return m_uncompressedSize; };

	private:
		std::string m_fileName;
		std::string m_documentId;
		size_t m_uncompressedSize = 0;
		bool m_isCompressed = false;

	};

}
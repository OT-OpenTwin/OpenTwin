// @otlicense
// File: TemporaryDir.h
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
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qdatetime.h>
#include <QtCore/qtemporarydir.h>

// std header
#include <list>

namespace ot {

	//! @brief The TemporaryDir class is a helper class to create and manage a temporary directory with files.
	class OT_WIDGETS_API_EXPORT TemporaryDir {
		OT_DECL_NOCOPY(TemporaryDir)
		OT_DECL_NOMOVE(TemporaryDir)
		OT_DECL_NODEFAULT(TemporaryDir)
	public:
		struct FileEntry {
			std::string entityName;
			QString initialFileName;
			QString fullPath;
			QDateTime lastModified;
		};

		//! @brief Constructor.
		//! Creates a temporary directory inside the given parent directory.
		//! The temporary directory and all its contents will be removed when the TemporaryDir instance is destroyed.
		//! @param _parentDir Path to the parent directory where the temporary directory will be created.
		TemporaryDir(const QString& _parentDir);
		~TemporaryDir() = default;

		//! @brief Checks whether the temporary directory was created successfully.
		bool isValid() const { return m_tempDir.isValid(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// File manipulation

		//! @brief Adds a file to the temporary directory.
		//! The file will be removed when the TemporaryDir instance is destroyed.
		//! @param _entityName Entity name of the file to add.
		//! @param _fileName Name of the file to add (with extension, without path).
		//! @param _data Data to write into the file.
		//! @param _replaceExisting If true, any existing file with the same name will be replaced. If false, the function will fail if a file with the same name already exists.
		//! @return True if the file was added successfully, false otherwise.
		bool addFile(const std::string& _entityName, const QString& _fileName, const QByteArray& _data, bool _replaceExisting = true);

		//! @brief Returns the file entry for the given entity name.
		//! @param _entityName Entity name of the file to retrieve.
		FileEntry getFileEntry(const std::string& _entityName) const;
		
		//! @brief Removes the file with the given entity name from the temporary directory.
		//! @param _entityName Entity name of the file to remove.
		//! @return True if the file was removed successfully, false if the file did not exist or the remove operation failed.
		bool removeFile(const std::string& _entityName);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Information gathering

		//! @brief Returns a list of all files that have been added to the temporary directory.
		const std::list<FileEntry>& getAllFiles() const { return m_files; };

		//! @brief Returns a list of all files that have been modified since they were added to the temporary directory.
		std::list<FileEntry> getModifiedFiles() const;

		//! @brief Will read the file data for the given entity name.
		//! @param _entityName Entity name of the file to read.
		//! @param _updateModified If true, the last modified timestamp of the file entry will be updated to the current file's last modified timestamp.
		QByteArray readFileData(const std::string& _entityName, bool _updateModified);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		FileEntry* getFileEntryRef(const std::string& _entityName);

		//! @brief Cleans the given directory path.
		//! @param _dir Directory path to clean.
		//! @return Cleaned directory path matching the OS standards and having a separator appended.
		static QString cleanDir(const QString& _dir);

		QTemporaryDir m_tempDir;
		std::list<FileEntry> m_files;

	};

}